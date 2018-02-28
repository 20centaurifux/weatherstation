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
	analogWrite(_outPin, 0);
}

void Voltmeter::write(int value, int min, int max) const
{
	int v = mapValue(value, min, max);

	analogWrite(_outPin, v);
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

	return (a / b) * (255 / 5) * _maxVoltage;
}

