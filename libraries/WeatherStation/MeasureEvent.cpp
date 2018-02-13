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

#include <extern.h>
#include <config.h>
#include <Events.h>

unsigned long MeasureEvent::operator()()
{
	leds.busy(true);

	LogValue value;

	if(sensors.measure(value))
	{
		weatherLog.append(value);

		uint32_t timestamp = LOG_VALUE_DECODE_TIMESTAMP(value);

		if(!LOG_VALUE_TEMPERATURE_ERROR(value))
		{
			dailyTemperature.update(timestamp, LOG_VALUE_DECODE_TEMPERATURE(value));
		}

		if(!LOG_VALUE_HUMIDITY_ERROR(value))
		{
			dailyHumidity.update(timestamp, LOG_VALUE_DECODE_HUMIDITY(value));
		}

		if(!LOG_VALUE_UV_ERROR(value))
		{
			uvBuffer.append(LOG_VALUE_DECODE_UV(value));
		}
	}

	leds.busy(false);

	return MEASURE_INTERVAL;
}

