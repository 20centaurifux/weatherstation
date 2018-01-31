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

template<int MAX_EVENTS>
class EventLoop
{
	public:
		EventLoop()
		{
			memset(_events, 0, sizeof(Event) * MAX_EVENTS);
		}

		int timeout(EventCallback *cb, uint16_t ms)
		{
			int id = findNextIndex();

			if(id != -1)
			{
				_events[id].cb = cb;
				_events[id].nextDue = getTime() + ms;
			}

			return id;
		}

		void clear(int id)
		{
			_events[id].cb = nullptr;
		}

		void iterate()
		{
			unsigned long ms = getTime();

			for(uint8_t i = 0; i < _tail; ++i)
			{
				if(_events[i].active() && _events[i].nextDue <= ms)
				{
					unsigned long interval = (*_events[i].cb)();

					if(interval)
					{
						_events[i].nextDue = getTime() + interval;
					}
					else
					{
						clear(i);
					}
				}
			}
		}

	private:
		typedef struct
		{
			EventCallback *cb;
			unsigned long nextDue;

			bool active()
			{
				return cb != nullptr;
			}
		} Event;

		Event _events[MAX_EVENTS];
		int _tail = 0;

		int findNextIndex()
		{
			int index = -1;

			if(_tail < MAX_EVENTS)
			{
				index = _tail;
				++_tail;
			}
			else
			{
				for(uint8_t i = 0; i < MAX_EVENTS && index == -1; ++i)
				{
					if(_events[i].active())
					{
						index = i;
					}
				}
			}

			return index;
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

