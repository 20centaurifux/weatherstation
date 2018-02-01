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

#ifndef WEATHER_LOG_H
#define WEATHER_LOG_H

#include <LogValue.h>
#include <Delta.h>

class ProcessLogValue
{
	public:
		virtual void operator()(const LogValue& value) = 0;
};

template<size_t SIZE>
class WeatherLog
{
	public:
		WeatherLog(uint32_t interval)
		        : _interval(interval)
			, _deltaBuilder(interval)
		{
			clear();
		}

		bool append(const LogValue& value)
		{
			uint8_t bytes[SERIALIZED_LOG_VALUE_SIZE];
			size_t len = SERIALIZED_LOG_VALUE_SIZE;
			bool success = false;

			if(empty())
			{
				SerializeLogValue(value, bytes);
			}
			else
			{
				_deltaBuilder.build(_lastValue, value);

				if(_deltaBuilder.hasDelta())
				{
					len = _deltaBuilder.copy(bytes, SERIALIZED_LOG_VALUE_SIZE);
				}
				else
				{
					SerializeLogValue(value, bytes);
				}
			}

			if(available() < len)
			{
				size_t shrinked = shrink(len);

				if(shrinked >= len)
				{
					if(empty())
					{
						SerializeLogValue(value, bytes);
						len = SERIALIZED_LOG_VALUE_SIZE;
					}

					success = true;
				}
			}
			else
			{
				success = true;
			}

			if(success)
			{
				push(bytes, len);
				_lastValue = value;
			}

			return success;
		}

		void clear()
		{
			_next = _bytes;
			_tail = nullptr;
		}

		bool empty() const
		{
			return _tail == nullptr;
		}

		void forEach(ProcessLogValue& f) const
		{
			if(!empty())
			{
				const uint8_t *ptr = _bytes;

				LogValue value;

				while(ptr < _tail)
				{
					if(*ptr & 0x80)
					{
						DeserializeLogValue(ptr, value);
						ptr += SERIALIZED_LOG_VALUE_SIZE;
					}
					else
					{
						ptr += ApplyDelta(value, ptr, _interval);
					}

					f(value);
				}
			}
		}

	private:
		uint32_t _interval;
		DeltaBuilder _deltaBuilder;
		uint8_t _bytes[SIZE];
		uint8_t *_next;
		uint8_t *_tail;
		LogValue _lastValue;

		size_t available() const
		{
			size_t size;

			if(empty())
			{
				size = SIZE;
			}
			else
			{
				size = SIZE - (_next - _bytes);
			}

			return size;
		}

		size_t shrink(size_t requiredSize)
		{
			size_t total = available();
			size_t shrinked;

			do
			{
				shrinked = pop();
				total += shrinked;
			} while(shrinked > 0 && total < requiredSize);

			return total;
		}

		size_t pop()
		{
			size_t shrinked = 0;

			if(!empty())
			{
				if(containsOnlyOneValue())
				{
					clear();
					shrinked = SERIALIZED_LOG_VALUE_SIZE;
				}
				else
				{
					uint8_t *next = _bytes + SERIALIZED_LOG_VALUE_SIZE;

					if(*next & 0x80)
					{
						memmove(_bytes, _bytes + SERIALIZED_LOG_VALUE_SIZE, (_tail - _bytes) - SERIALIZED_LOG_VALUE_SIZE);
						shrinked = SERIALIZED_LOG_VALUE_SIZE;
					}
					else
					{
						LogValue value;

						DeserializeLogValue(_bytes, value);

						shrinked = ApplyDelta(value, _bytes + SERIALIZED_LOG_VALUE_SIZE, _interval);

						SerializeLogValue(value, _bytes);

						memmove(_bytes + SERIALIZED_LOG_VALUE_SIZE,
							_bytes + SERIALIZED_LOG_VALUE_SIZE + shrinked,
							(_tail - _bytes) - SERIALIZED_LOG_VALUE_SIZE - shrinked);
					}

					_tail -= shrinked;
					_next -= shrinked;
				}
			}

			return shrinked;
		}

		bool containsOnlyOneValue() const
		{
			return _next == _bytes + SERIALIZED_LOG_VALUE_SIZE;
		}

		void push(const uint8_t *bytes, size_t len)
		{
			if(empty())
			{
				memcpy(_bytes, bytes, len);
				_next = _bytes + len;
			}
			else
			{
				memcpy(_next, bytes, len);
				_next += len;
			}

			if(_next > _tail)
			{
				_tail = _next;
			}
		}
};

#endif

