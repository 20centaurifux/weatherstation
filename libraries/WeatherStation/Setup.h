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

#ifndef SETUP_H
#define SETUP_H

class Setup
{
	public:
		bool required() const;
		void run() const;

	private:
		typedef enum
		{
			ENTER_NUMBER,
			ENTER_HOURS,
			ENTER_MINUTES,
			ENTER_SECONDS
		} EnterNumberFormat;

		void turnOffDevices() const;
		void setDateTime() const;
		void resetLogs() const;
		int enterNumber(int min, int max, EnterNumberFormat format, int n, int timePart) const;
};

#endif

