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

#ifndef EVENTS_H
#define EVENTS_H

#include <Event.h>
#include <CircularBuffer.h>
#include <StationModel.h>
#include <WeatherLog.h>

class MeasureEvent : public EventCallback
{
	public:
		unsigned long operator()();
};

class TransmitEvent : public EventCallback, ProcessLogValue
{
	public:
		unsigned long operator()();
		void operator()(const LogValue& value);

	private:
		uint32_t _now;
};

class DisplayEvent : public EventCallback
{
	public:
		static const int DARK = 250;
		static const int BRIGHT = 450;

		DisplayEvent() : _state(0) {}
		unsigned long operator()();

	private:
		class CalculateAverageUV : public ProcessBufferValue<float>
		{
			public:
				CalculateAverageUV() : _uv(0.0), _count(0) {}

				void operator()(float uv);
				float average();

			private:
				float _uv;
				size_t _count;
		};

		class CalculatePressureTendency : public ProcessLogValue
		{
			public:
				CalculatePressureTendency(uint32_t timestamp, int pressure);
				void operator()(const LogValue& value);
				uint8_t tendency();

			private:
				PressureTendency _tendency;
		};

		uint8_t _state = 0;
		LogValue _value;
		DateTime _dt;
		float _avgUV;
		uint8_t _pressureTendency;

		void setBacklight(int light);
		void setDisplayBrightness(int light);
		bool measure();
		void computePressureTendency();
		void computeAverageUV();
		void showTime();
		void showTemperature();
		void showPressure();
		void showHumidity();
		void showUV();
		void off();
};

#endif

