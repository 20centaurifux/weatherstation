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

#include <config.h>
#include <pins.h>

#include <WeatherSensors.h>
#include <WeatherLog.h>
#include <WeatherLEDs.h>
#include <WeatherDisplay.h>
#include <Voltmeter.h>
#include <Button.h>
#include <Events.h>
#include <CircularBuffer.h>
#include <RFTransmitter.h>

WeatherLog<512> weatherLog(MEASURE_INTERVAL / 1000);

DailyAverage<int32_t> dailyHumidity;
DailyAverage<float> dailyTemperature;
CircularBuffer<float, UV_BUFFER_SIZE> uvBuffer;

WeatherSensors sensors(DHT11_DIO, UV_PIN, REF_3V_PIN, LDR_PIN);
WeatherLEDs leds = WeatherLEDs(DS, SH_CP, ST_CP);
WeatherDisplay display = WeatherDisplay(TM1637_CLK, TM1637_DIO);
Voltmeter voltmeter(VOLT_PIN, VOLT_REF, 3.0);

RFTransmitter transmitter(RF_PIN, RF_NODE_ID, RF_PULSE_WIDTH, RF_BACKOFF_DELAY, RF_RESEND_COUNT);

static EventLoop<3> events;

static MeasureEvent measureEvent;
static TransmitEvent transmitEvent;

void calibration()
{
	leds.backlight(true);
	leds.busy(true);
	leds.set(WEATHER_LED_TIME);
	display.showNumber(9999);
	display.bright(true);

	voltmeter.calibrate();

	leds.off();
	display.off();
	voltmeter.write(0, 0, 0);
}

void setup()
{
	Serial.begin(9600);

	display.begin();
	sensors.begin();
	leds.begin();
	voltmeter.begin();

	calibration();

	events.timeout(&measureEvent, 0);
	events.timeout(&transmitEvent, TRANSMIT_INTERVAL);
}

int main(void) 
{
	init();
	setup();

	Button<INPUT_PULLUP> btnSet(BTN_SET);

	DisplayEvent displayEvent;
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

