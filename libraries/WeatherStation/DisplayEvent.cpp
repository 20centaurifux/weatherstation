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

#include <Events.h>
#include <extern.h>
#include <config.h>

unsigned long DisplayEvent::operator()()
{
	unsigned long interval = DISPLAY_INTERVAL;

	voltmeter.write(0, 0, 0);

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

void DisplayEvent::setBacklight(int light)
{
	bool on = light <= DisplayEvent::DARK;

	leds.backlight(on);
}

void DisplayEvent::setDisplayBrightness(int light)
{
	bool bright = light >= DisplayEvent::BRIGHT;

	display.bright(bright);
}

bool DisplayEvent::measure()
{
	bool success;
	
	success = sensors.now(_dt) && sensors.measure(_value);

	computePressureTendency();
	computeAverageUV();
	
	return success;
}

void DisplayEvent::computePressureTendency()
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

void DisplayEvent::computeAverageUV()
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

void DisplayEvent::showTime()
{
	leds.set(WEATHER_LED_TIME);

	if(sensors.now(_dt))
	{
		uint32_t phase = moonPhase(_dt.year(), _dt.month(), _dt.day());

		voltmeter.write(mapMoonPhase(phase), 0, 30);

		display.showTime(_dt.hour(), _dt.minute());
	}
	else
	{
		display.showError(ERROR_READ_RTC);
	}
}

void DisplayEvent::showTemperature()
{
	leds.set(WEATHER_LED_TEMPERATURE);

	if(LOG_VALUE_TEMPERATURE_ERROR(_value))
	{
		display.showError(LOG_VALUE_TEMPERATURE_ERROR_CODE(_value));
	}
	else
	{
		voltmeter.write(dailyTemperature.average(), -16, 32);
		display.showTemperature(LOG_VALUE_DECODE_TEMPERATURE(_value));
	}
}

void DisplayEvent::showPressure()
{
	leds.set(WEATHER_LED_PRESSURE);

	if(LOG_VALUE_PRESSURE_ERROR(_value))
	{
		display.showError(LOG_VALUE_PRESSURE_ERROR_CODE(_value));
	}
	else
	{
		voltmeter.write(mapPressureTendency(_pressureTendency), 0, 30);
		display.showNumber(LOG_VALUE_DECODE_PRESSURE(_value));
	}
}

void DisplayEvent::showHumidity()
{
	leds.set(WEATHER_LED_HUMIDITY);

	if(LOG_VALUE_HUMIDITY_ERROR(_value))
	{
		display.showError(LOG_VALUE_HUMIDITY_ERROR_CODE(_value));
	}
	else
	{
		voltmeter.write(dailyHumidity.average(), 0, 100);
		display.showNumber(LOG_VALUE_DECODE_HUMIDITY(_value));
	}
}

void DisplayEvent::showUV()
{
	leds.set(WEATHER_LED_UV);

	if(LOG_VALUE_UV_ERROR(_value))
	{
		display.showError(LOG_VALUE_UV_ERROR_CODE(_value));
	}
	else
	{
		voltmeter.write(mapUV(_avgUV), 0, 30);
		display.showFloat(LOG_VALUE_DECODE_UV(_value));
	}
}

void DisplayEvent::off()
{
	voltmeter.write(0, 0, 0);
	leds.off();
	display.off();
}

void DisplayEvent::CalculateAverageUV::operator()(float uv)
{
	_uv += uv;
	++_count;
}

float DisplayEvent::CalculateAverageUV::average()
{
	float avg = 0.0;

	if(_count)
	{
		avg = _uv / _count;
	}

	return avg;
}

DisplayEvent::CalculatePressureTendency::CalculatePressureTendency(uint32_t timestamp, int pressure)
{
	_tendency.start(timestamp, pressure);
}

void DisplayEvent::CalculatePressureTendency::operator()(const LogValue& value)
{
	_tendency.update(LOG_VALUE_DECODE_TIMESTAMP(value), LOG_VALUE_DECODE_PRESSURE(value));
}

uint8_t DisplayEvent::CalculatePressureTendency::tendency()
{
	return _tendency.tendency();
}

