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

#include "StationModel.h"

static int mapInteger(int v, int min, int max)
{
	if(v < min)
	{
		v = min;
	}
	else if(v > max)
	{
		v = max;
	}

	float a = v - min;
	float b = max - min;

	return (a / b) * 0xff;
}

int mapMoonPhase(uint8_t moonPhase)
{
	if(moonPhase > 7)
	{
		moonPhase = 0;
	}

	return moonPhase * 32 + 14;
}

int mapTemperature(float c)
{
	return mapInteger((int)c, -10, 40);
}

int mapPressureTendency(uint8_t tendency)
{
	if(tendency > 8)
	{
		tendency = 0;
	}

	return mapInteger((tendency + 1) * 5, 0, 50);
}

int mapHumidity(uint8_t hum)
{
	return mapInteger(hum, 0, 10);
}

int mapUV(float uv)
{
	int v = 42;

	if(uv >= 0.9)
	{
		v = 204;
	}

	return v;
}

