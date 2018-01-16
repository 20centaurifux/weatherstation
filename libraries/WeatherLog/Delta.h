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

#ifndef DELTA_H
#define DELTA_H

#include <LogValue.h>

#define DELTA_SECONDS_MAX 63

#define DELTA_TEMPERATURE_MIN INT8_MIN
#define DELTA_TEMPERATURE_MAX INT8_MAX

#define DELTA_PRESSURE_MIN -8
#define DELTA_PRESSURE_MAX 8

#define DELTA_UV_MIN -63
#define DELTA_UV_MAX 63

#define DELTA_HUMIDITY_MIN -63
#define DELTA_HUMIDITY_MAX 63

#define DELTA_MAX_SIZE 6

class DeltaBuilder
{
	public:
		DeltaBuilder(uint32_t interval) : _interval(interval), _success(false) {}
		void build(const LogValue& from, const LogValue& to);
		bool hasDelta() const { return _success; }
		size_t size() const;
		size_t copy(uint8_t *dst, size_t max) const;

	private:
		uint32_t _interval;
		uint8_t _delta[DELTA_MAX_SIZE];
		bool _success;
		size_t _offset;
		const LogValue *_from;
		const LogValue *_to;
		int16_t _diffs[5];

		void reset();
		bool calcAndVerifyDiffs();
		void writeSeconds();
		void writeHeader();
		void writePayload();
		inline bool isBigTemperatureDiff(int16_t diff) { return diff < -15 || diff > 15; }
		void writeTemperature();
		void writePressure();
		inline bool isBigUVDiff(int16_t diff) { return diff < -3 || diff > 3; }
		void writeUV();
		inline bool isBigHumidityDiff(int16_t diff) { return diff < -15 || diff > 15; }
		void writeHumidity();
		void mapOffset(size_t& index, size_t& left) const;
		void appendBit(bool set);
		void appendBits(uint8_t bits, uint8_t count);
};

class DeltaReader
{
	public:
		DeltaReader();
		void setSource(const uint8_t *bytes);
		void read();
		uint8_t seconds() const;
		bool hasTemperature() const;
		bool temperatureFailed() const;
		int8_t temperature() const;
		bool hasPressure() const;
		bool pressureFailed() const;
		int8_t pressure() const;
		bool hasUV() const;
		bool uvFailed() const;
		int8_t uv() const;
		bool hasHumidity() const;
		bool humidityFailed() const;
		int8_t humidity() const;

	private:
		const uint8_t *_ptr;
		size_t _offset;
		size_t _size;
		int8_t _errors;
		uint8_t _seconds;
		int16_t _temp;
		uint8_t _tempFlags;
		int8_t _pressure;
		uint8_t _pressureFlags;
		int8_t _uv;
		uint8_t _uvFlags;
		int8_t _humidity;
		uint8_t _humidityFlags;

		const uint8_t VALUE_FLAG_LARGE = 1;
		const uint8_t VALUE_FLAG_POS   = 2;
		const uint8_t VALUE_FLAG_ERROR = 4;

		void reset();
		void readValueFlags();
		void readSeconds();
		void readValues();
		void mapOffset(size_t& index, size_t& left) const;
		int8_t readBits(size_t count);
};

size_t ApplyDelta(LogValue& value, const uint8_t* delta);

#endif

