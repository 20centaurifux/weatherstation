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
 * This code depends on the following libraries:
 *   https://github.com/adafruit/RTClib
 *   https://github.com/sparkfun/BMP180_Breakout
 *   https://github.com/winlinvip/SimpleDHT
 */

#ifndef WEATHER_SENSORS_H
#define WEATHER_SENSORS_H

#include <RTClib.h>
#include <SFE_BMP180.h>
#include <SimpleDHT.h>
#include <LogValue.h>

#define ERROR_NO_SENSOR                1
#define ERROR_BMP180_START_TEMPERATURE 2
#define ERROR_BMP180_GET_TEMPERATURE   3
#define ERROR_BMP180_START_PRESSURE    4
#define ERROR_BMP180_GET_PRESSURE      5

#define BMP180_OVERSAMPLING 3

class WeatherSensors
{
	public:
		WeatherSensors(int dht11Pin, int uvPin, int ref3VPin)
			: _rtcFound(false)
		        , _bmp180Found(false)
		        , _dht11Pin(dht11Pin)
		        , _uvPin(uvPin)
		        , _ref3VPin(ref3VPin) {}

		void begin();
		bool now(DateTime& now);
		bool measure(LogValue& value);

	private:
		bool _rtcFound;
		RTC_DS3231 _rtc;
		bool _bmp180Found;
		SFE_BMP180 _bmp180;
		int _dht11Pin;
		SimpleDHT11 _dht11;
		int _uvPin;
		int _ref3VPin;

		LogValueBuilder _builder;

		void measureBMP180();
		bool measureBMP180_Temperature(double& temperature);
		void measureBMP180_Pressure(double temperature);
		void measureDHT11();
		void measureUV();
};
#endif


