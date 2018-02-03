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
 * This library is based on https://github.com/avishorp/TM1637
 */

#ifndef WEATHER_DISPLAY_H
#define WEATHER_DISPLAY_H

class WeatherDisplay
{
	public:
		WeatherDisplay(int clkPin, int dioPin);
		void begin();
		void off();
		void on();
		inline bool isOn() { return _on; }
		void showNumber(int n);
		void showTime(uint8_t hour, uint8_t minute);
		void showTemperature(int c);
		void showError(int err);
		void bright(bool bright);

	private:
		typedef enum
		{
			WEATHER_DISPLAY_FORMAT_NUMBER,
			WEATHER_DISPLAY_FORMAT_TIME,
			WEATHER_DISPLAY_FORMAT_TEMPERATURE,
			WEATHER_DISPLAY_FORMAT_ERROR
		} WeatherDisplayFormat;

		int _clkPin;
		int _dioPin;
		bool _on;
		int _val;
		bool _bright = false;
		WeatherDisplayFormat _fmt;

		static uint8_t encodeDigit(uint8_t digit);

		void update();
		void writeDec(int num, uint8_t dots, bool leading_zero);
		void write2DigitsWithSuffix(int n, uint8_t suffix);
		void write2DigitsWithPrefix(int n, uint8_t prefix);
		void setSegments(const uint8_t segments[], uint8_t length);
		void start();
		void stop();
		bool writeByte(uint8_t byte);
};
#endif

