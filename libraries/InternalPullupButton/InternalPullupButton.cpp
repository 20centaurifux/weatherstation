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

#include <Arduino.h>

#include "InternalPullupButton.h"

InternalPullupButton::InternalPullupButton(int pin)
{
	_pin = pin;
	_lastDebounce = 0;
	_pressed = false;

	pinMode(pin, INPUT_PULLUP);
	digitalWrite(pin, HIGH);
}

bool InternalPullupButton::pressed()
{
	bool pressed = digitalRead(_pin) == LOW;

	unsigned long interval = millis() - _lastDebounce;

	if(interval > 50)
	{
		_pressed = pressed;
	}

	return _pressed;
}

