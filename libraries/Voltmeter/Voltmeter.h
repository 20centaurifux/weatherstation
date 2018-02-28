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

#ifndef VOLTMETER_H
#define VOLTMETER_H

class Voltmeter
{
	public:
		Voltmeter(int outPin, float maxVoltage)
			: _outPin(outPin)
			, _maxVoltage(maxVoltage) {}

		void begin();
		void write(int value, int min, int max) const;

	private:
		int _outPin;
		float _maxVoltage;

		int mapValue(int value, int min, int max) const;
};

#endif

