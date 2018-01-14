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

/*
 * This library controls status LEDs and backlight of the weather station. The
 * LEDs are connected to the following PINs of a 74HC595 shift register:
 *
 * Q0 backlight
 * Q1 time
 * Q2 temperature
 * Q3 atmospheric pressure
 * Q4 humidity
 * Q5 UV level
 */

#ifndef WEATHER_LEDS_H
#define WEATHER_LEDS_H

typedef enum
{
	WEATHER_LED_NONE = 0,
	WEATHER_LED_TIME = 2,
	WEATHER_LED_TEMPERATURE = 4,
	WEATHER_LED_PRESSURE = 8,
	WEATHER_LED_HUMIDITY = 16,
	WEATHER_LED_UV = 32
} WeatherLED;

class WeatherLEDs
{
	public:
		WeatherLEDs(int ds, int sh_cp, int st_cp);
		void begin();
		void off();
		void set(WeatherLED led);
		void backlight(bool on);

	private:
		void write(int led);

		int _ds;
		int _sh_cp;
		int _st_cp;
		WeatherLED _led;
		bool _backlight;
};
#endif

