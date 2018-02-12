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

#ifndef STATION_MODEL_H
#define STATION_MODEL_H

#ifdef ARDUINO
#include <Arduino.h>
#include <RTClib.h>
#else
#include <stdint.h>
#include <cstddef>
#endif

class PressureTendency
{
	public:
		PressureTendency() { start(0, 0); }
		void start(uint32_t timestamp, int pressure);
		void update(uint32_t timestamp, int pressure);
		int tendency() const;

	private:
		uint32_t _timestamp;
		uint32_t _lastTimestamp;
		int _pressure;
		uint32_t _sums[3];
		size_t _count[3];
};

#ifdef ARDUINO
template<typename T>
class DailyAverage
{
	public:
		DailyAverage() : _count(0), _day(0), _lastTimestamp(0) {}

		void update(uint32_t timestamp, T val)
		{
			if(timestamp > _lastTimestamp)
			{
				DateTime dt(timestamp);

				uint8_t day = dt.day();

				if(day == _day)
				{
					_sum += val;
					++_count;
				}
				else
				{
					_day = day;
					_sum = val;
					_count = 1;
				}

				_lastTimestamp = timestamp;
			}
		}

		T average() const
		{
			if(_count > 0)
			{
				return _sum / _count;
			}

			return 0;
		}

	private:
		T _sum;
		size_t _count;
		uint8_t _day;
		uint32_t _lastTimestamp;
};
#endif

uint8_t MoonPhase(int year, int month, int day);

#endif

