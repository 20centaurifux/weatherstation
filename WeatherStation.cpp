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
#include <Setup.h>

WeatherLog<512> weatherLog(MEASURE_INTERVAL / 1000);

DailyAverage<int32_t> dailyHumidity;
DailyAverage<float> dailyTemperature;
CircularBuffer<float, UV_BUFFER_SIZE> uvBuffer;

WeatherSensors sensors(DHT11_DIO, UV_PIN, REF_3V_PIN, LDR_PIN);
WeatherLEDs leds = WeatherLEDs(DS, SH_CP, ST_CP);
WeatherDisplay display = WeatherDisplay(TM1637_CLK, TM1637_DIO);
Voltmeter voltmeter(VOLT_PIN, 3.0);

Button<INPUT_PULLUP> btnSet(BTN_SET);
Button<INPUT_PULLUP> btnUp(BTN_UP);
Button<INPUT_PULLUP> btnDown(BTN_DOWN);

RFTransmitter transmitter(RF_PIN, RF_NODE_ID, RF_PULSE_WIDTH, RF_BACKOFF_DELAY, RF_RESEND_COUNT);

static EventLoop<3> events;

static MeasureEvent measureEvent;
static TransmitEvent transmitEvent;

void setupDevices()
{
	Serial.begin(9600);

	analogReference(EXTERNAL);

	display.begin();
	sensors.begin();
	leds.begin();
	voltmeter.begin();

	events.timeout(&measureEvent, 0);
	events.timeout(&transmitEvent, TRANSMIT_INTERVAL);
}

int main(void) 
{
	init();
	setupDevices();

	Setup setup;

	if(setup.required())
	{
		setup.run();
	}

	DisplayEvent displayEvent;
	EventId displayEventId = 0;

	while(1)
	{
		if(btnSet.pressed())
		{
			if(displayEventId)
			{
				events.clear(displayEventId);
				displayEvent.reset();
				displayEventId = 0;

				setup.run();
			}
			else
			{
				displayEventId = events.timeout(&displayEvent, 0);
			}
		}

		if(displayEvent.completed())
		{
			displayEventId = 0; 
		}
		
		events.iteration();
	}

	return 0;
}

