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

unsigned long TransmitEvent::operator()()
{
	DateTime n;

	if(sensors.now(n))
	{
		_now = n.unixtime();

		leds.busy(true);
		weatherLog.forEach(*this);
		leds.busy(false);
	}

	return TRANSMIT_INTERVAL;
}

void TransmitEvent::operator()(const LogValue& value)
{
	uint32_t timestamp = LOG_VALUE_DECODE_TIMESTAMP(value);

	if(timestamp <= _now)
	{
		uint32_t seconds = _now - timestamp;

		if(seconds <= TRANSMIT_TIME_FILTER)
		{
			uint8_t buffer[SERIALIZED_LOG_VALUE_SIZE];

			SerializeLogValue(value, buffer);

			transmitter.send(buffer, SERIALIZED_LOG_VALUE_SIZE);
			transmitter.resend(buffer, SERIALIZED_LOG_VALUE_SIZE);
		}
	}
}

