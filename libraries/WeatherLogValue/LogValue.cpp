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
#include <cstdint>
#include <cstring>
#else
#include "Arduino.h"
#endif

#include "LogValue.h"

LogValueBuilder::LogValueBuilder()
{
	reset();
}

void LogValueBuilder::reset()
{
	memset(&_value, 0, sizeof(LogValue));
}

void LogValueBuilder::setTimestamp(uint32_t seconds)
{
	_value.timestamp = seconds;
}

bool LogValueBuilder::setTemperature(float t)
{
	bool success = false;

	if(t >= LOG_VALUE_TEMPERATURE_MIN && t <= LOG_VALUE_TEMPERATURE_MAX)
	{
		t -= LOG_VALUE_TEMPERATURE_MIN;
		t *= 10;

		_value.temp = t;
		_value.errors &= 0xfffe;

		success = true;
	}

	return success;
}

void LogValueBuilder::setTemperatureFailure(uint8_t errcode)
{
	_value.temp = errcode;
	_value.errors |= 1;
}

bool LogValueBuilder::setPressure(uint16_t hPa)
{
	bool success = false;

	if(hPa >= LOG_VALUE_PRESSURE_MIN && hPa <= LOG_VALUE_PRESSURE_MAX)
	{
		_value.hPa = hPa - LOG_VALUE_PRESSURE_MIN;
		_value.errors &= 0xfffd;

		success = true;
	}

	return success;
}

void LogValueBuilder::setPressureFailure(uint8_t errcode)
{
	_value.hPa = errcode;
	_value.errors |= 2;
}

bool LogValueBuilder::setUV(float t)
{
	bool success = false;

	if(t >= LOG_VALUE_UV_MIN && t <= LOG_VALUE_UV_MAX)
	{
		t *= 10;

		_value.uv = t;
		_value.errors &= 0x773;

		success = true;
	}

	return success;
}

void LogValueBuilder::setUVFailure(uint8_t errcode)
{
	_value.uv = errcode;
	_value.errors |= 4;
}

bool LogValueBuilder::setHumidity(uint8_t hum)
{
	bool success = false;

	if(hum <= LOG_VALUE_HUMIDITY_MAX)
	{
		_value.hum = hum - LOG_VALUE_HUMIDITY_MIN;
		_value.errors &= 0xfff7;

		success = true;
	}

	return success;
}

bool LogValueBuilder::setHumidityFailure(uint8_t errcode)
{
	bool success = false;

	if(errcode <= 0x7f)
	{
		_value.hum = errcode;
		_value.errors |= 8;

		success = true;
	}

	return success;
}

void
SerializeLogValue(const LogValue& value, uint8_t *dst)
{
	dst[0] = 0x80 | value.hum;
	dst[1] = value.uv;
	dst[2] = (value.timestamp >> 24) & 0xff;
	dst[3] = (value.timestamp >> 16) & 0xff;
	dst[4] = (value.timestamp >> 8) & 0xff;
	dst[5] = value.timestamp & 0xff;
	dst[6] = (value.errors << 4) | ((0x1e0 & value.hPa) >> 5);
	dst[7] = ((value.hPa << 3) & 0xf8) | (value.temp >> 7);
	dst[8] = (0x7f & value.temp) << 1;
}

void
DeserializeLogValue(const uint8_t *src, LogValue& value)
{
	value.hum = 0x7f & src[0];
	value.timestamp = (uint32_t)src[2] << 24 | (uint32_t)src[3] << 16 | (uint32_t)src[4] << 8 | src[5];
	value.uv = src[1];
	value.errors = (src[6] >> 4) & 0xf;
	value.hPa = ((0xf & src[6]) << 5) | ((src[7] >> 3) & 0x1f);
	value.temp = ((0x7 & src[7]) << 7) | ((src[8] >> 1) & 0x7f);
}

