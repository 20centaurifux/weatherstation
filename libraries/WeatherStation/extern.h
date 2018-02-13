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

#ifndef EXTERN_H
#define EXTERN_H

#include <WeatherLog.h>

extern WeatherLog<512> weatherLog;

#include <StationModel.h>

extern DailyAverage<int32_t> dailyHumidity;
extern DailyAverage<float> dailyTemperature;

#include <config.h>
#include <CircularBuffer.h>

extern CircularBuffer<float, UV_BUFFER_SIZE> uvBuffer;

#include <WeatherSensors.h>

extern WeatherSensors sensors;

#include <WeatherLEDs.h>

extern WeatherLEDs leds;

#include <WeatherDisplay.h>

extern WeatherDisplay display;

#include <RFTransmitter.h>

extern RFTransmitter transmitter;

#endif

