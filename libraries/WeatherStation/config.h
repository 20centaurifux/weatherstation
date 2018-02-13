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

#ifndef CONFIG_H
#define CONFIG_H

#define MEASURE_INTERVAL     60000ul
#define TRANSMIT_INTERVAL    300000ul
#define DISPLAY_INTERVAL     8000ul

#define UV_BUFFER_SIZE       (900000 / MEASURE_INTERVAL)

#define RF_NODE_ID           1
#define RF_PULSE_WIDTH       302
#define RF_BACKOFF_DELAY     20
#define RF_RESEND_COUNT      1

#define TRANSMIT_TIME_FILTER 900

#endif

