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

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

template<int PIN_MODE>
class Button
{
	public:
		Button(int pin)
		{
			_pin = pin;
			_lastDebounce = 0;
			_pressed = false;

			pinMode(pin, PIN_MODE);

			if(PIN_MODE == OUTPUT)
			{
				digitalWrite(pin, LOW);
			}
			else
			{
				digitalWrite(pin, HIGH);
			}
		}

		bool pressed()
		{
			bool pressed = false;
			int state = digitalRead(_pin);

			if(state == (PIN_MODE == OUTPUT) ? HIGH : LOW)
			{
				pressed = true;
			}

			unsigned long interval = millis() - _lastDebounce;

			if(interval > 50)
			{
				_pressed = pressed;
			}

			return _pressed;
		}

	private:
		int _pin;
		unsigned long _lastDebounce;
		bool _pressed;
};

#endif

