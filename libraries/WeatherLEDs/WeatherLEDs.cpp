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

#include "Arduino.h"
#include "WeatherLEDs.h"

WeatherLEDs::WeatherLEDs(int ds, int sh_cp, int st_cp)
{
	_led = WEATHER_LED_NONE;
	_backlight = false;
	_busy = false;
	_ds = ds;
	_sh_cp = sh_cp;
	_st_cp = st_cp;
}

void
WeatherLEDs::begin()
{
	pinMode(_ds, OUTPUT);
	pinMode(_sh_cp, OUTPUT);
	pinMode(_st_cp, OUTPUT);

	off();
}

void
WeatherLEDs::off()
{
	_led = WEATHER_LED_NONE;
	_backlight = false;
	_busy = false;

	this->write(0);
}

void
WeatherLEDs::set(WeatherLED led)
{
	int bg = 0;
	int busy = 0;

	if(_backlight)
	{
		bg = 1;
	}

	if(_busy)
	{
		busy = 64;
	}

	this->write(led|bg|busy);

	_led = led;
}

void
WeatherLEDs::backlight(bool on)
{
	if(_backlight != on)
	{
		_backlight = on;
		set(_led);
	}
}

void
WeatherLEDs::busy(bool on)
{
	if(_busy != on)
	{
		_busy = on;
		set(_led);
	}
}

void
WeatherLEDs::write(int n)
{
	digitalWrite(_st_cp, 0);
	shiftOut(_ds, _sh_cp, MSBFIRST, n);
	digitalWrite(_st_cp, 1);
}

