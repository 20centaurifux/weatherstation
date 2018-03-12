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

#include <Tools.h>

#include "StationModel.h"

static const int DAY_SUMS[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

static int dayOfYear(int year, int month, int day)
{
	int days = DAY_SUMS[month - 1];

	if(days > 59 && isLeapYear(year))
	{
		++days;
	}

	days += day;

	return days;
}

uint8_t
moonPhase(int year, int month, int day)
{
        int goldn = (year % 19) + 1;
        int epact = (11 * goldn + 18) % 30;

        if((epact == 25 && goldn > 11) || epact == 24)
	{
		++epact;
	}

	int yday = dayOfYear(year, month, day);

        return (((((yday + epact) * 6) + 11) % 177) / 22) & 7;
}

