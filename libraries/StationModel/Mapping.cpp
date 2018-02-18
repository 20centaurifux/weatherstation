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

int mapMoonPhase(uint8_t moonPhase)
{
	if(moonPhase > 7)
	{
		moonPhase = 0;
	}

	return moonPhase * 4 + 1;
}

int mapPressureTendency(uint8_t tendency)
{
	if(tendency > 8)
	{
		tendency = 0;
	}

	return (tendency + 1) * 3;
}

int mapUV(float uv)
{
	return uv >= 0.9 ? 20 : 10;
}

