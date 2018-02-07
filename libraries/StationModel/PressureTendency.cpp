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

#ifndef ARDUINO
#include <cstring>
#endif

#include "StationModel.h"

void PressureTendency::start(uint32_t timestamp, int pressure)
{
	_timestamp = timestamp;
	_pressure = pressure;

	memset(_sums, 0, sizeof(uint32_t) * 3);
	memset(_count, 0, sizeof(size_t) * 3);
}

void PressureTendency::update(uint32_t timestamp, int pressure)
{
	if(_timestamp >= timestamp)
	{
		int seconds = _timestamp - timestamp;

		if(seconds < 10800)
		{
			int index = 2 - seconds / 3600;

			uint32_t sum = _sums[index] + pressure;

			if(sum > _sums[index])
			{
				_sums[index] = sum;
				++_count[index];
			}
		}
	}
}

int PressureTendency::tendency() const
{
	int avg[3] = {0};
	int tendency = 4;

	for(int i = 0; i < 3; ++i)
	{
		if(_count[i])
		{
			avg[i] = _sums[i] / _count[i];
		}
	}

	#define ONE_HOUR    2
	#define TWO_HOURS   1
	#define THREE_HOURS 0

	if(_pressure > avg[THREE_HOURS])
	{
		if(avg[TWO_HOURS] > _pressure && avg[ONE_HOUR] < _pressure)
		{
			tendency = 0;
		}
		else if(avg[TWO_HOURS] > _pressure && avg[ONE_HOUR] == _pressure)
		{
			tendency = 1;
		}
		else if(avg[TWO_HOURS] > _pressure && avg[ONE_HOUR] > _pressure)
		{
			tendency = 2;
		}
		else if(avg[TWO_HOURS] < _pressure && avg[ONE_HOUR] > _pressure)
		{
			tendency = 3;
		}
	}
	else if(_pressure < avg[THREE_HOURS])
	{
		if(avg[TWO_HOURS] < _pressure && avg[ONE_HOUR] > _pressure)
		{
			tendency = 5;
		}
		else if(avg[TWO_HOURS] < _pressure && avg[ONE_HOUR] == _pressure)
		{
			tendency = 6;
		}
		else if(avg[TWO_HOURS] < _pressure && avg[ONE_HOUR] < _pressure)
		{
			tendency = 7;
		}
		else if(avg[TWO_HOURS] > _pressure && avg[ONE_HOUR] < _pressure)
		{
			tendency = 8;
		}
	}

	return tendency;
}

