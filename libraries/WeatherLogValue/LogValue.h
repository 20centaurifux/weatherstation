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

#ifndef WEATHER_LOG_VALUE_H
#define WEATHER_LOG_VALUE_H

#ifndef ARDUINO
#include <cstdint>
#else
#include "Arduino.h"
#endif

typedef struct __attribute__((packed))
{
	uint32_t timestamp;
	uint8_t uv;
	uint32_t errors:4;
	uint32_t hum:7;
	uint32_t temp:10;
	uint32_t hPa:9;
} LogValue;

#define LOG_VALUE_TEMPERATURE_MIN  -30.0
#define LOG_VALUE_TEMPERATURE_MAX  50.0

#define LOG_VALUE_PRESSURE_MIN 589
#define LOG_VALUE_PRESSURE_MAX 1100

#define LOG_VALUE_UV_MIN 0
#define LOG_VALUE_UV_MAX 15.0

#define LOG_VALUE_HUMIDITY_MIN 0
#define LOG_VALUE_HUMIDITY_MAX 100

#define LOG_VALUE_TEMPERATURE_ERROR(v) (v.errors & 1)
#define LOG_VALUE_TEMPERATURE_ERROR_CODE(v) v.temp

#define LOG_VALUE_PRESSURE_ERROR(v) ((v.errors & 2) == 2)
#define LOG_VALUE_PRESSURE_ERROR_CODE(v) v.hPa

#define LOG_VALUE_UV_ERROR(v) ((v.errors & 4) == 4)
#define LOG_VALUE_UV_ERROR_CODE(v) v.uv

#define LOG_VALUE_HUMIDITY_ERROR(v) ((v.errors & 8) == 8)
#define LOG_VALUE_HUMIDITY_ERROR_CODE(v) v.hum

#define LOG_VALUE_DECODE_TIMESTAMP(v) v.timestamp
#define LOG_VALUE_DECODE_TEMPERATURE(v) (v.temp + (LOG_VALUE_TEMPERATURE_MIN * 10)) / 10
#define LOG_VALUE_DECODE_PRESSURE(v) v.hPa + LOG_VALUE_PRESSURE_MIN
#define LOG_VALUE_DECODE_UV(v) (float)v.uv / 10
#define LOG_VALUE_DECODE_HUMIDITY(v) v.hum

#define LOG_VALUE_MAX_ERROR 63

class LogValueBuilder
{
	public:
		LogValueBuilder();
		void reset();
		void setTimestamp(uint32_t seconds);
		bool setTemperature(float cel);
		bool setTemperatureFailure(uint8_t errCode);
		bool setPressure(uint16_t hPa);
		bool setPressureFailure(uint8_t errCode);
		bool setUV(float uv);
		bool setUVFailure(uint8_t errCode);
		bool setHumidity(uint8_t hum);
		bool setHumidityFailure(uint8_t errCode);
		inline LogValue build() const { return _value; }

	private:
		LogValue _value;
};

#define SERIALIZED_LOG_VALUE_SIZE 9

void SerializeLogValue(const LogValue& value, uint8_t *dst);
void DeserializeLogValue(const uint8_t *src, LogValue& value);

#endif

