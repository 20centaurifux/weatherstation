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
#include <cstring>
#include <cstdlib>
#else
#include "Arduino.h"
#endif

#include <Delta.h>

void DeltaBuilder::build(const LogValue& from, const LogValue& to)
{
	_from = &from;
	_to = &to;

	reset();

	if(calcAndVerifyDiffs())
	{
		writeHeader();
		writePayload();

		_success = true;
	}
}

size_t DeltaBuilder::size() const
{
	size_t index, left;

	mapOffset(index, left);

	if(left)
	{
		++index;
	}

	return index;
}

size_t DeltaBuilder::copy(uint8_t *dst, size_t max) const
{
	size_t bytes = size();

	if(bytes <= max)
	{
		memcpy(dst, _delta, bytes);
	}
	else
	{
		bytes = 0;
	}

	return bytes;
}

void DeltaBuilder::reset()
{
	memset(&_diffs, 0, sizeof(int16_t) * 5);
	memset(&_delta, 0, DELTA_MAX_SIZE);

	_offset = 0;
	_success = false;
}

bool DeltaBuilder::calcAndVerifyDiffs()
{
	if(_from->timestamp >= _to->timestamp)
	{
		return false;
	}

	uint32_t seconds = _to->timestamp - _from->timestamp;

	if(seconds < _interval)
	{
		return false;
	}

	seconds -= _interval;

	if(seconds > DELTA_SECONDS_MAX)
	{
		return false;
	}

	_diffs[0] = seconds;

	_diffs[1] = _to->temp - _from->temp;

	if(_diffs[1] < DELTA_TEMPERATURE_MIN || _diffs[1] > DELTA_TEMPERATURE_MAX)
	{
		return false;
	}

	_diffs[2] = _to->hPa - _from->hPa;

	if(_diffs[2] < DELTA_PRESSURE_MIN || _diffs[2] > DELTA_PRESSURE_MAX)
	{
		return false;
	}

	_diffs[3] = _to->uv - _from->uv;

	if(_diffs[3] < DELTA_UV_MIN || _diffs[3] > DELTA_UV_MAX)
	{
		return false;
	}

	_diffs[4] = _to->hum - _from->hum;

	if(_diffs[4] < DELTA_HUMIDITY_MIN || _diffs[4] > DELTA_HUMIDITY_MAX)
	{
		return false;
	}

	return true;
}

void DeltaBuilder::writeHeader()
{
	#define PTR_LOG_VALUE_TEMPERATURE_ERROR(v) (v->errors & 1)

	_delta[0] = 0;
	_offset = 11; // type flag + seconds + values

	if(_diffs[1] || PTR_LOG_VALUE_TEMPERATURE_ERROR(_from) != PTR_LOG_VALUE_TEMPERATURE_ERROR(_to))
	{
		_delta[0] |= 1;

		appendBit(PTR_LOG_VALUE_TEMPERATURE_ERROR(_to));
		appendBit(_diffs[1] > 0);
		appendBit(isBigTemperatureDiff(_diffs[1]));
	}

	#define PTR_LOG_VALUE_PRESSURE_ERROR(v) ((v->errors & 2) == 2)

	if(_diffs[2] || PTR_LOG_VALUE_PRESSURE_ERROR(_from) != PTR_LOG_VALUE_PRESSURE_ERROR(_to))
	{
		_delta[1] |= 0x80;

		appendBit(PTR_LOG_VALUE_PRESSURE_ERROR(_to));
		appendBit(_diffs[2] > 0);
	}

	#define PTR_LOG_VALUE_UV_ERROR(v) ((v->errors & 4) == 4)

	if(_diffs[3] || PTR_LOG_VALUE_UV_ERROR(_from) != PTR_LOG_VALUE_UV_ERROR(_to))
	{
		_delta[1] |= 0x40;

		appendBit(PTR_LOG_VALUE_UV_ERROR(_to));
		appendBit(_diffs[3] > 0);
		appendBit(isBigUVDiff(_diffs[3]));
	}

	#define PTR_LOG_VALUE_HUMIDITY_ERROR(v) ((v->errors & 8) == 8)

	if(_diffs[4] || PTR_LOG_VALUE_HUMIDITY_ERROR(_from) != PTR_LOG_VALUE_HUMIDITY_ERROR(_to))
	{
		_delta[1] |= 0x20;

		appendBit(PTR_LOG_VALUE_HUMIDITY_ERROR(_to));
		appendBit(_diffs[4] > 0);
		appendBit(isBigHumidityDiff(_diffs[4]));
	}
}

void DeltaBuilder::writePayload()
{
	writeSeconds();
	writeTemperature();
	writePressure();
	writeUV();
	writeHumidity();
}

void DeltaBuilder::writeSeconds()
{
	_delta[0] |= 0x7e & (_diffs[0] << 1);
}

void DeltaBuilder::writeTemperature()
{
	if(_delta[0] & 1)
	{
		int8_t count = 4;

		if(isBigTemperatureDiff(_diffs[1]))
		{
			count = 8;
		}

		uint8_t val = abs(_diffs[1]);

		appendBits(val, count);
	}
}

void DeltaBuilder::writePressure()
{
	if(_delta[1] & 0x80)
	{
		uint8_t val = abs(_diffs[2]);

		appendBits(val, 4);
	}
}

void DeltaBuilder::writeUV()
{
	if(_delta[1] & 0x40)
	{
		uint8_t val = abs(_diffs[3]);

		if(isBigUVDiff(_diffs[3]))
		{
			appendBits(val, 6);
		}
		else
		{
			appendBits(val, 2);
		}
	}
}

void DeltaBuilder::writeHumidity()
{
	if(_delta[1] & 0x20)
	{
		uint8_t val = abs(_diffs[4]);

		if(isBigHumidityDiff(_diffs[4]))
		{
			appendBits(val, 6);
		}
		else
		{
			appendBits(val, 4);
		}
	}
}

void DeltaBuilder::mapOffset(size_t& index, size_t& left) const
{
	index = _offset / 8;
	left = _offset - index * 8;
}

void DeltaBuilder::appendBit(bool set)
{
	if(set)
	{
		size_t index, left;

		mapOffset(index, left);

		_delta[index] |= (1 << (7 - left));
	}

	++_offset;
}

void DeltaBuilder::appendBits(uint8_t bits, uint8_t count)
{
	size_t index, left;

	mapOffset(index, left);

	size_t available = 8 - left;

	bits <<= 8 - count;

	if(available < count)
	{
		_delta[index] |= bits >> left;
		_delta[index + 1] |= bits << available;

		_offset += count;
	}
	else
	{
		_delta[index] |= bits >> left;

		_offset += count;
	}
}

DeltaReader::DeltaReader()
{
	reset();
}

void DeltaReader::setSource(const uint8_t *bytes)
{
	reset();

	_ptr = bytes;
}

void DeltaReader::read()
{
	if(_ptr && !_read)
	{
		start();

		readValueFlags();
		readSeconds();
		readValues();

		_read = true;
	}
}

size_t DeltaReader::calcSize()
{
	size_t size = 0;

	if(_ptr)
	{
		start();

		readValueFlags();

		size_t bits = 11;

		if(hasTemperature())
		{
			if(_tempFlags & VALUE_FLAG_LARGE)
			{
				bits += 11;
			}
			else
			{
				bits += 7;
			}
		}

		if(hasPressure())
		{
			bits += 6;
		}

		if(hasUV())
		{
			if(_uvFlags & VALUE_FLAG_LARGE)
			{
				bits += 9;
			}
			else
			{
				bits += 5;
			}
		}

		if(hasHumidity())
		{
			if(_humidityFlags & VALUE_FLAG_LARGE)
			{
				bits += 9;
			}
			else
			{
				bits += 7;
			}
		}

		size = bits / 8;

		if(bits - size * 8)
		{
			++size;
		}
	}

	return size;
}

size_t DeltaReader::size() const
{
	size_t size = 0;

	if(_read)
	{
		size_t index, left;

		mapOffset(index, left);

		if(left)
		{
			++index;
		}

		size = index;
	}

	return size;
}

void DeltaReader::start()
{
	_offset = 11;
}

void DeltaReader::reset()
{
	_ptr = nullptr;
	_read = false;
	_offset = 0;
	_errors = 0;
	_seconds = 0;
	_temp = 0;
	_tempFlags = 0;
	_pressure = 0;
	_pressureFlags = 0;
	_uv = 0;
	_uvFlags = 0;
	_humidity = 0;
	_humidityFlags = 0;
}

void DeltaReader::readValueFlags()
{
	if(hasTemperature())
	{
		_tempFlags = readBits(3);
	}

	if(hasPressure())
	{
		_pressureFlags = readBits(2) << 1;
	}

	if(hasUV())
	{
		_uvFlags = readBits(3);
	}

	if(hasHumidity())
	{
		_humidityFlags = readBits(3);
	}
}

void DeltaReader::readSeconds()
{
	_seconds = _ptr[0] >> 1;
}

void DeltaReader::readValues()
{
	if(hasTemperature())
	{
		size_t size = 4;

		if(_tempFlags & VALUE_FLAG_LARGE)
		{
			size *= 2;
		}

		_temp = readBits(size);

		if(!(_tempFlags & VALUE_FLAG_POS))
		{
			_temp *= -1;
		}
	}

	if(hasPressure())
	{
		_pressure = readBits(4);

		if(!(_pressureFlags & VALUE_FLAG_POS))
		{
			_pressure *= -1;
		}
	}

	if(hasUV())
	{
		size_t size = 2;

		if(_uvFlags & VALUE_FLAG_LARGE)
		{
			size = 6;
		}

		_uv = readBits(size);

		if(!(_uvFlags & VALUE_FLAG_POS))
		{
			_uv *= -1;
		}
	}

	if(hasHumidity())
	{
		size_t size = 4;

		if(_humidityFlags & VALUE_FLAG_LARGE)
		{
			size = 6;
		}

		_humidity = readBits(size);

		if(!(_humidityFlags & VALUE_FLAG_POS))
		{
			_humidity *= -1;
		}
	}
}

void DeltaReader::mapOffset(size_t& index, size_t& left) const
{
	index = _offset / 8;
	left = _offset - index * 8;
}

int8_t DeltaReader::readBits(size_t count)
{
	size_t index, left;

	mapOffset(index, left);

	size_t available = 8 - left;

	uint8_t mask = 0xff >> (8 - available);
	uint8_t byte = _ptr[index] & mask;

	if(available < count)
	{
		byte <<= count - available;
		byte |= (_ptr[index + 1] >> (8 - (count - available)));
	}
	else if(count < available)
	{
		byte >>= available - count;
	}

	_offset += count;

	return byte;
}

size_t ApplyDelta(LogValue& value, const uint8_t* delta, uint32_t interval)
{
	DeltaReader reader;

	reader.setSource(delta);
	reader.read();

	LogValueBuilder builder;

	builder.setTimestamp(LOG_VALUE_DECODE_TIMESTAMP(value) + interval + reader.seconds());

	if(reader.hasTemperature())
	{
		if(reader.temperatureFailed())
		{
			builder.setTemperatureFailure(value.temp + reader.temperature());
		}
		else
		{
			double temp = (double)reader.temperature() / 10;

			builder.setTemperature(LOG_VALUE_DECODE_TEMPERATURE(value) + temp);
		}
	}
	else
	{
		builder.setTemperature(LOG_VALUE_DECODE_TEMPERATURE(value));
	}

	if(reader.hasPressure())
	{
		if(reader.pressureFailed())
		{
			builder.setPressureFailure(value.hPa + reader.pressure());
		}
		else
		{
			builder.setPressure(LOG_VALUE_DECODE_PRESSURE(value) + reader.pressure());
		}
	}
	else
	{
		builder.setPressure(LOG_VALUE_DECODE_PRESSURE(value));
	}

	if(reader.hasHumidity())
	{
		if(reader.humidityFailed())
		{
			builder.setHumidityFailure(value.hum + reader.humidity());
		}
		else
		{
			builder.setHumidity(LOG_VALUE_DECODE_HUMIDITY(value) + reader.humidity());
		}
	}
	else
	{
		builder.setHumidity(LOG_VALUE_DECODE_HUMIDITY(value));
	}

	if(reader.hasUV())
	{
		if(reader.uvFailed())
		{
			builder.setUVFailure(value.uv + reader.uv());
		}
		else
		{
			double uv = (double)reader.uv() / 10;

			builder.setUV(LOG_VALUE_DECODE_UV(value) + uv);
		}
	}
	else
	{
		builder.setUV(LOG_VALUE_DECODE_UV(value));
	}

	value = builder.build();

	return reader.size();
}

