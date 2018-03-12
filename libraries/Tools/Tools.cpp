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

#include <Tools.h>

int averageAnalogRead(int pinToRead, byte numberOfReadings)
{
	uint32_t runningValue = 0;

	for(byte i = 0; i < numberOfReadings; ++i)
	{
		runningValue += analogRead(pinToRead);
	}

	return runningValue / numberOfReadings;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int daysOfMonth(int year, int month)
{
	int days = 30;

	if(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
	{
		days = 31;
	}
	else if(month == 2)
	{
		if(isLeapYear(year))
		{
			days = 29;
		}
		else
		{
			days = 28;
		}
	}

	return days;
}

