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

#ifndef EVENT_H
#define EVENT_H

#ifndef ARDUINO
#include <cstdint>
#include <cstring>
#include <ctime>
#else
#include "Arduino.h"
#endif

class EventCallback
{
	public:
		virtual unsigned long operator()() = 0;
};

typedef size_t EventId;

template<size_t MAX_EVENTS>
class EventLoop
{
	public:
		EventLoop()
		{
			memset(_events, 0, sizeof(Event) * MAX_EVENTS);
		}

		EventId timeout(EventCallback *cb, unsigned long ms)
		{
			size_t index = 0;
			EventId id = 0;
			
			if(findNextIndex(index))
			{
				_events[index].cb = cb;
				_events[index].start = getTime();
				_events[index].timeout = ms;

				id = index + 1;
			}

			return id;
		}

		void clear(EventId id)
		{
			if(id > 0 && id <= _count)
			{
				_events[id - 1].cb = nullptr;
			}
		}

		void iterate()
		{
			unsigned long ms = getTime();

			for(uint8_t i = 0; i < _count; ++i)
			{
				if(_events[i].active() && _events[i].due(ms))
				{
					unsigned long interval = (*_events[i].cb)();

					if(interval)
					{
						_events[i].start = getTime();
						_events[i].timeout = interval;
					}
					else
					{
						clear(i + 1);
					}
				}
			}
		}

	private:
		typedef struct
		{
			EventCallback *cb;
			unsigned long start;
			unsigned long timeout;

			bool active()
			{
				return cb != nullptr;
			}

			bool due(unsigned long ms)
			{
				return ms - start >= timeout;
			}
		} Event;

		Event _events[MAX_EVENTS];
		size_t _count = 0;

		bool findNextIndex(size_t& index)
		{
			bool found = false;

			for(size_t i = 0; i < MAX_EVENTS && !found; ++i)
			{
				if(!_events[i].active())
				{
					index = i;
					found = true;
				}
			}

			if(found && index > _count)
			{
				_count = index + 1;
			}

			return found;
		}

		unsigned long getTime()
		{
			#ifdef ARDUINO
			return millis();
			#else
			struct timespec tp;

			if(clock_gettime(CLOCK_MONOTONIC, &tp))
			{
				return 0;
			}
			else
			{
				return (tp.tv_sec * 1000) + (tp.tv_nsec / 1000000);
			}
			#endif
		}
};

#endif

