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
#else
#include <stdint.h>
#include <cstddef>
#endif

class PressureTendency
{
	public:
		void start(uint32_t timestamp, int pressure);
		void update(uint32_t timestamp, int pressure);
		int tendency() const;

	private:
		uint32_t _timestamp;
		int _pressure;
		uint32_t _sums[3];
		size_t _count[3];
};

uint8_t MoonPhase(int year, int month, int day);

#endif

