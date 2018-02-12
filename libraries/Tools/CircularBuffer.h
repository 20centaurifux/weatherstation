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

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstddef>
#endif

template<typename T>
class ProcessBufferValue
{
	public:
		virtual void operator()(T value) = 0;
};

template<typename T, size_t SIZE>
class CircularBuffer
{
	public:
		CircularBuffer() : _count(0), _head(0) {}

		void append(T value)
		{
			if(_count < SIZE)
			{
				_values[_count] = value;
				++_count;
			}
			else
			{
				_values[_head] = value;

				if(_head == SIZE - 1)
				{
					_head = 0;
				}
				else
				{
					++_head;
				}
			}
		}

		size_t count()
		{
			return _count;
		}

		void forEach(ProcessBufferValue<T>& f) const
		{
			size_t offset = _head;

			while(offset < _count)
			{
				f(_values[offset]);
				++offset;
			}

			if(_head > 0)
			{
				offset = 0;

				while(offset < _head)
				{
					f(_values[offset]);
					++offset;
				}
			}
		}

	private:
		T _values[SIZE];
		size_t _count;
		size_t _head;
};

#endif

