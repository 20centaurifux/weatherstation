/***************************************************************************
    begin........: January 2018
    copyright....: Sebastian Fedrau
    email........: sebastian.fedrau@gmail.com
 ***************************************************************************/

/***************************************************************************
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License v3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License v3 for more details.
 ***************************************************************************/

#include <WeatherSensors.h>
#include <WeatherLog.h>
#include <WeatherLEDs.h>
#include <WeatherDisplay.h>
#include <Button.h>
#include <Event.h>
#include <StationModel.h>
#include <CircularBuffer.h>
#include <RFTransmitter.h>

#define DHT11_DIO  12
#define UV_PIN     A0
#define REF_3V_PIN A1
#define LDR_PIN    A2
#define RF_PIN     10
#define VOLT_PIN   11

#define TM1637_CLK 3
#define TM1637_DIO 4

#define DS         7
#define ST_CP      6
#define SH_CP      5

#define BTN_SET    2

#define MEASURE_INTERVAL  60000ul
#define TRANSMIT_INTERVAL 300000ul
#define DISPLAY_INTERVAL  8000ul

#define TRANSMIT_TIME_FILTER 900

#define RF_NODE_ID       1
#define RF_PULSE_WIDTH   302
#define RF_BACKOFF_DELAY 20
#define RF_RESEND_COUNT  1

WeatherLog<512> weatherLog(MEASURE_INTERVAL / 1000);

DailyAverage<int32_t> dailyHumidity;
DailyAverage<float> dailyTemperature;
CircularBuffer<float, 900000 / MEASURE_INTERVAL> uvBuffer;

WeatherSensors sensors(DHT11_DIO, UV_PIN, REF_3V_PIN, LDR_PIN);
WeatherLEDs leds = WeatherLEDs(DS, SH_CP, ST_CP);
WeatherDisplay display = WeatherDisplay(TM1637_CLK, TM1637_DIO);

RFTransmitter transmitter(RF_PIN, RF_NODE_ID, RF_PULSE_WIDTH, RF_BACKOFF_DELAY, RF_RESEND_COUNT);

EventLoop<3> events;

class Measure : public EventCallback
{
	public:
		unsigned long operator()()
		{
			leds.busy(true);

			LogValue value;
 
			if(sensors.measure(value))
			{
				weatherLog.append(value);

				uint32_t timestamp = LOG_VALUE_DECODE_TIMESTAMP(value);

				if(!LOG_VALUE_TEMPERATURE_ERROR(value))
				{
					dailyTemperature.update(timestamp, LOG_VALUE_DECODE_TEMPERATURE(value));
				}

				if(!LOG_VALUE_HUMIDITY_ERROR(value))
				{
					dailyHumidity.update(timestamp, LOG_VALUE_DECODE_HUMIDITY(value));
				}

				if(!LOG_VALUE_UV_ERROR(value))
				{
					uvBuffer.append(LOG_VALUE_DECODE_UV(value));
				}
			}

			leds.busy(false);

			return MEASURE_INTERVAL;
		}
};

Measure measureEvent;

class Transmit: public EventCallback, public ProcessLogValue
{
	public: 
		unsigned long operator()()
		{
			DateTime n;

			if(sensors.now(n))
			{
				_now = n.unixtime();

				leds.busy(true);
				weatherLog.forEach(*this);
				leds.busy(false);
			}

			return TRANSMIT_INTERVAL;
		}

		void operator()(const LogValue& value)
		{
			uint32_t timestamp = LOG_VALUE_DECODE_TIMESTAMP(value);

			if(timestamp <= _now)
			{
				uint32_t seconds = _now - timestamp;

				if(seconds <= TRANSMIT_TIME_FILTER)
				{
					uint8_t buffer[SERIALIZED_LOG_VALUE_SIZE];

					SerializeLogValue(value, buffer);

					transmitter.send(buffer, SERIALIZED_LOG_VALUE_SIZE);
					transmitter.resend(buffer, SERIALIZED_LOG_VALUE_SIZE);
				}
			}
		}

	private:
		uint32_t _now;
};

Transmit transmitEvent;

#define DARK   250
#define BRIGHT 450

class DisplayLogValue : public EventCallback
{
	public: 
		unsigned long operator()()
		{
			unsigned long interval = DISPLAY_INTERVAL;

			analogWrite(VOLT_PIN, 0);

			if(!_state && !measure())
			{
				interval = 0;
			}
			else if(_state < 5)
			{
				int light = sensors.light();

				setBacklight(light);
				setDisplayBrightness(light);

				if(_state == 0)
				{
					showTime();
				}
				else if(_state == 1)
				{
					showTemperature();
				}
				else if(_state == 2)
				{
					showPressure();
				}
				else if(_state == 3)
				{
					showHumidity();
				}
				else
				{
					showUV();
				}

				++_state;
			}
			else
			{
				off();
				interval = 0;
				_state = 0;
			}

			return interval;
		}

	private:
		class CalculateAverageUV : public ProcessBufferValue<float>
		{
			public:
				CalculateAverageUV() : _uv(0.0), _count(0) {}

				void operator()(float uv)
				{
					_uv += uv;
					++_count;
				}

				float average()
				{
					float avg = 0.0;

					if(_count)
					{
						avg = _uv / _count;
					}

					return avg;
				}

			private:
				float _uv;
				size_t _count;
		};

		class CalculatePressureTendency : public ProcessLogValue
		{
			public:
				CalculatePressureTendency(uint32_t timestamp, int pressure)
				{
					_tendency.start(timestamp, pressure);
				}

				void operator()(const LogValue& value)
				{
					_tendency.update(LOG_VALUE_DECODE_TIMESTAMP(value), LOG_VALUE_DECODE_PRESSURE(value));
				}
	
				uint8_t tendency()
				{
					return _tendency.tendency();
				}

			private:
				PressureTendency _tendency;
		};

		uint8_t _state = 0;
		LogValue _value;
		DateTime _dt;
		float _avgUV;
		uint8_t _pressureTendency;

		void setBacklight(int light)
		{
			bool on = light <= DARK;

			leds.backlight(on);
		}

		void setDisplayBrightness(int light)
		{
			bool bright = light >= BRIGHT;

			display.bright(bright);
		}

		bool measure()
		{
			bool success;
			
			success = sensors.now(_dt) && sensors.measure(_value);

			computePressureTendency();
			computeAverageUV();
			
			return success;
		}

		void computePressureTendency()
		{
			if(LOG_VALUE_PRESSURE_ERROR(_value))
			{
				_pressureTendency = 0;
			}
			else
			{
				CalculatePressureTendency f(_dt.unixtime(), LOG_VALUE_DECODE_PRESSURE(_value));

				weatherLog.forEach(f);
				_pressureTendency = f.tendency();
			}
		}

		void computeAverageUV()
		{
			if(LOG_VALUE_PRESSURE_ERROR(_value))
			{
				_avgUV = 0.0;
			}
			else
			{
				CalculateAverageUV f;

				uvBuffer.forEach(f);
				_avgUV = f.average();
			}
		}

		void showTime()
		{
			leds.set(WEATHER_LED_TIME);

			if(sensors.now(_dt))
			{
				uint32_t phase = moonPhase(_dt.year(), _dt.month(), _dt.day());

				analogWrite(VOLT_PIN, mapMoonPhase(phase));

				display.showTime(_dt.hour(), _dt.minute());
			}
			else
			{
				display.showError(ERROR_READ_RTC);
			}
		}

		void showTemperature()
		{
			leds.set(WEATHER_LED_TEMPERATURE);

			if(LOG_VALUE_TEMPERATURE_ERROR(_value))
			{
				display.showError(LOG_VALUE_TEMPERATURE_ERROR_CODE(_value));
			}
			else
			{
				analogWrite(VOLT_PIN, mapTemperature(dailyTemperature.average()));
				display.showTemperature(LOG_VALUE_DECODE_TEMPERATURE(_value));
			}
		}

		void showPressure()
		{
			leds.set(WEATHER_LED_PRESSURE);

			if(LOG_VALUE_PRESSURE_ERROR(_value))
			{
				display.showError(LOG_VALUE_PRESSURE_ERROR_CODE(_value));
			}
			else
			{
				analogWrite(VOLT_PIN, mapPressureTendency(_pressureTendency));
				display.showNumber(LOG_VALUE_DECODE_PRESSURE(_value));
			}
		}

		void showHumidity()
		{
			leds.set(WEATHER_LED_HUMIDITY);

			if(LOG_VALUE_HUMIDITY_ERROR(_value))
			{
				display.showError(LOG_VALUE_HUMIDITY_ERROR_CODE(_value));
			}
			else
			{
				analogWrite(VOLT_PIN, mapHumidity(dailyHumidity.average()));
				display.showNumber(LOG_VALUE_DECODE_HUMIDITY(_value));
			}
		}

		void showUV()
		{
			leds.set(WEATHER_LED_UV);

			if(LOG_VALUE_UV_ERROR(_value))
			{
				display.showError(LOG_VALUE_UV_ERROR_CODE(_value));
			}
			else
			{
				analogWrite(VOLT_PIN, mapUV(_avgUV));
				display.showFloat(LOG_VALUE_DECODE_UV(_value));
			}
		}

		void off()
		{
			analogWrite(VOLT_PIN, 0);
			leds.off();
			display.off();
		}
};

DisplayLogValue displayEvent;

void setup()
{
	Serial.begin(9600);

	pinMode(VOLT_PIN, OUTPUT);

	sensors.begin();
	leds.begin();
	display.off();

	events.timeout(&measureEvent, 0);
	events.timeout(&transmitEvent, TRANSMIT_INTERVAL);
}

int main(void) 
{
	init();
	setup();

	Button<INPUT_PULLUP> btnSet(BTN_SET);

	EventId displayEventId = 0;

	while(1)
	{
		if(btnSet.pressed() && !displayEventId)
		{
			displayEventId = events.timeout(&displayEvent, 0);
		}

		if(displayEvent.completed())
		{
			displayEventId = 0; 
		}
		
		events.iteration();
	}

	return 0;
}

