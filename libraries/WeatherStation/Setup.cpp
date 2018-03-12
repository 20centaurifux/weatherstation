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

#include <extern.h>
#include <Tools.h>

#include "Setup.h"

bool Setup::required() const
{
	return sensors.rtcLostPower();
}

void Setup::run() const
{
	turnOffDevices();
	setDateTime();
	resetLogs();
	turnOffDevices();
}

void Setup::turnOffDevices() const
{
	leds.off();
	display.off();
	voltmeter.write(0, 0, 0);
}

void Setup::setDateTime() const
{
	int year = enterNumber(2018, 3000, ENTER_NUMBER, 2018, 0);
	int month = enterNumber(1, 12, ENTER_NUMBER, 1, 0);

	int maxDays = daysOfMonth(year, month);

	int day = enterNumber(1, maxDays, ENTER_NUMBER, 1, 0);

	int hours = enterNumber(0, 23, ENTER_HOURS, 0, 0);
	int minutes = enterNumber(0, 59, ENTER_MINUTES, 0, hours);
	int seconds = enterNumber(0, 59, ENTER_SECONDS, 0, 0);

	DateTime dt = DateTime(year, month, day, hours, minutes, seconds);

	sensors.setDateTime(dt);
}

int Setup::enterNumber(int min, int max, EnterNumberFormat format, int n, int timePart) const
{
	bool on = true;
	unsigned long lastChange = millis();

	display.bright(true);

	while(1)
	{
		if(millis() - lastChange > 100)
		{
			if(on)
			{
				display.on();

				if(format == ENTER_NUMBER || format == ENTER_SECONDS)
				{
					display.showNumber(n);
				}
				else if(format == ENTER_HOURS)
				{
					display.showTime(n, timePart);
				}
				else if(format == ENTER_MINUTES)
				{
					display.showTime(timePart, n);
				}
			}
			else
			{
				display.off();
			}

			on = !on;

			lastChange = millis();
		}

		if(btnUp.pressed())
		{
			if(n == max)
			{
				n = min;
			}
			else
			{
				n++;
			}
		}
		else if(btnDown.pressed())
		{
			if(n == min)
			{
				n = max;
			}
			else
			{
				n--;
			}
		}
		else if(btnSet.pressed())
		{
			break;
		}
	}

	display.on();

	leds.busy(true);
	delay(500);
	leds.busy(false);

	return n;
}

void Setup::resetLogs() const
{
	weatherLog.clear();
	dailyHumidity.clear();
	dailyTemperature.clear();
	uvBuffer.clear();
}

