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
		Voltmeter(int outPin, int refPin, float maxVoltage)
			: _outPin(outPin),
			_refPin(refPin),
			_maxVoltage(maxVoltage),
			_maxValue(0) {}

		void begin();
		void write(int value, int min, int max) const;
		void calibrate();

	private:
		int _outPin;
		int _refPin;
		float _maxVoltage;
		int _maxValue;

		void computeCalibrationRange(int& start, int& end) const;
		int mapValue(int value, int min, int max) const;
};

#endif

