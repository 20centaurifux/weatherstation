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

/*
 * Deltas are stored in arrays of at most 6 bytes.
 *
 * Field                          Length (bits)
 * =====================================
 * flag                         | 1 (always 0)
 * seconds                      | 6
 * changed values               | 4
 * temperature flags (optional) | 3
 * pressure flags (optional)    | 2
 * uv flags (optional)          | 3
 * humidity flags (optional)    | 3
 * temperature (optional)       | 4 or 8
 * pressure (optional)          | 4
 * uv (optional)                | 2 or 6
 * humidity (optional)          | 4 or 6
 *
 * Values have 2 or 3 flags. If the given flag is set the stored value
 * 1. is an error code.
 * 2. is a positive number.
 * 3. requires the maximum supported bit length (see table above).
 */

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
		size_t calcSize();
		size_t size() const;
		inline uint8_t seconds() const { return _seconds; }
		inline bool hasTemperature() const { return _ptr != nullptr && _ptr[0] & 0x1; }
		inline bool temperatureFailed() const { return _tempFlags & VALUE_FLAG_ERROR; }
		inline int8_t temperature() const { return _temp; }
		inline bool hasPressure() const { return _ptr != nullptr && (_ptr[1] & 0x80); }
		inline bool pressureFailed() const { return _pressureFlags & VALUE_FLAG_ERROR; }
		inline int8_t pressure() const { return _pressure; }
		inline bool hasUV() const { return _ptr != nullptr && (_ptr[1] & 0x40); }
		inline bool uvFailed() const { return _uvFlags & VALUE_FLAG_ERROR; }
		inline int8_t uv() const { return _uv; }
		inline bool hasHumidity() const { return _ptr != nullptr && _ptr[1] & 0x20; }
		inline bool humidityFailed() const { return _humidityFlags & VALUE_FLAG_ERROR; }
		inline int8_t humidity() const { return _humidity; }

	private:
		const uint8_t *_ptr;
		bool _read;
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

		void start();
		void reset();
		void readValueFlags();
		void readSeconds();
		void readValues();
		void mapOffset(size_t& index, size_t& left) const;
		int8_t readBits(size_t count);
};

size_t ApplyDelta(LogValue& value, const uint8_t* delta, uint32_t interval);

size_t DeltaSize(const uint8_t *delta);

#endif

