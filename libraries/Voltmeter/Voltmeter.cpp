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

#include "Voltmeter.h"

#include <Arduino.h>
#include <Tools.h>

void Voltmeter::begin()
{
	int v = (255 / 5) * _maxVoltage;

	int start = v - 50;

	if(start < 0)
	{
		start = 0;
	}

	int end = v + 50;

	if(end > 255)
	{
		end = 255;
	}

	calibrate(start, end);
}

void Voltmeter::write(int value, int min, int max) const
{
	int v = mapValue(value, min, max);

	analogWrite(_outPin, v);
}

void Voltmeter::calibrate(int start, int end)
{
	_maxValue = start;

	for(int i = start; i < end; ++i)
	{
		analogWrite(_outPin, i);

		delay(50);

		float measured = averageAnalogRead(_refPin, 255) * (5.0 / 1023.0);

		if(measured > _maxVoltage)
		{
			break;
		}

		_maxValue = i;
	}
}

int Voltmeter::mapValue(int value, int min, int max) const
{
	if(value < min)
	{
		value = min;
	}
	else if(value > max)
	{
		value = max;
	}

	float a = value - min;
	float b = max - min;

	return (a / b) * _maxValue;
}

