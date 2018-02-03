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

/*
 * This library is based on https://github.com/avishorp/TM1637
 */

#include "Arduino.h"
#include "WeatherDisplay.h"

#define TM1637_I2C_COMM1 0x40
#define TM1637_I2C_COMM2 0xC0
#define TM1637_I2C_COMM3 0x80

#define WEATHER_DISPLAY_DARK   0x2
#define WEATHER_DISPLAY_BRIGHT 0x7

#define BIT_DELAY delayMicroseconds(50)

static const uint8_t digitToSegment[] =
{
	0b00111111, 0b00000110,
	0b01011011, 0b01001111,
	0b01100110, 0b01101101,
	0b01111101, 0b00000111,
	0b01111111, 0b01101111
};

#define SEGMENT_C     0b00111001
#define SEGMENT_E     0b01111001
#define SEGMENT_MINUS 0b01000000

WeatherDisplay::WeatherDisplay(int clkPin, int dioPin)
{
	_clkPin = clkPin;
	_dioPin = dioPin;

	_on = false;
	_val = 0;
	_fmt = WEATHER_DISPLAY_FORMAT_NUMBER;
}

void WeatherDisplay::begin()
{
	pinMode(_clkPin, INPUT);
	pinMode(_dioPin,INPUT);

	digitalWrite(_clkPin, LOW);
	digitalWrite(_dioPin, LOW);
}

void WeatherDisplay::on()
{
	_on = true;
	update();
}

void WeatherDisplay::off()
{
	_on = false;
	update();
}

void WeatherDisplay::showNumber(int n)
{
	_val = n;
	_fmt = WEATHER_DISPLAY_FORMAT_NUMBER;

	on();
}

void WeatherDisplay::showTime(uint8_t hour, uint8_t minute)
{
	_val = hour * 100 + minute;
	_fmt = WEATHER_DISPLAY_FORMAT_TIME;

	on();
}

void WeatherDisplay::showTemperature(int c)
{
	_val = c;
	_fmt = WEATHER_DISPLAY_FORMAT_TEMPERATURE;

	on();
}

void WeatherDisplay::bright(bool bright)
{
	_bright = bright;

	if(isOn())
	{
		update();
	}
}

void WeatherDisplay::update()
{
	if(_fmt == WEATHER_DISPLAY_FORMAT_TIME)
	{
		writeDec(_val, 64, true);
	}
	else if(_fmt == WEATHER_DISPLAY_FORMAT_NUMBER)
	{
		writeDec(_val, 0, false);
	}
	else
	{
		writeTemperature(_val);
	}
}

void WeatherDisplay::writeDec(int num, uint8_t dots, bool leading_zero)
{
	uint8_t digits[4] = {0};
	const static int divisors[] = {1, 10, 100, 1000};
	bool leading = true;

	for(int8_t k = 0; k < 4; ++k)
	{
		int divisor = divisors[3 - k];
		int d = num / divisor;
		uint8_t digit = 0;

		if(d == 0)
		{
			if(leading_zero || !leading || (k == 3))
			{
				digit = encodeDigit(d);
			}
		}
		else
		{
			digit = encodeDigit(d);
			num -= d * divisor;
			leading = false;
		}
    
		digit |= (dots & 0x80); 
		dots <<= 1;
    
		digits[k] = digit;
	}

	setSegments(digits, 4);
}

void WeatherDisplay::writeTemperature(int c)
{
	uint8_t digits[4] = {0};

	int num = abs(c);

	int digit = num / 10;

	if(digit)
	{
		digits[1] = encodeDigit(digit);

		if(c < 0)
		{
			digits[0] = SEGMENT_MINUS;
		}
	}
	else if(c < 0)
	{
		digits[1] = SEGMENT_MINUS;
	}

	digits[2] = encodeDigit(num - (digit * 10));
	digits[3] = SEGMENT_C;

	setSegments(digits, 4);
}

uint8_t WeatherDisplay::encodeDigit(uint8_t digit)
{
	return digitToSegment[digit & 0x0f];
}

void WeatherDisplay::setSegments(const uint8_t segments[], uint8_t length)
{
	start();
	writeByte(TM1637_I2C_COMM1);
	stop();

	start();
	writeByte(TM1637_I2C_COMM2);

	for(uint8_t k = 0; k < length; k++)
	{
		writeByte(segments[k]);
	}

	stop();

	int brightness = WEATHER_DISPLAY_DARK;

	if(_bright)
	{
		brightness = WEATHER_DISPLAY_BRIGHT;
	}

	brightness = (brightness & 0x7) | (_on ? 0x08 : 0x00);

	start();
	writeByte(TM1637_I2C_COMM3 + (brightness & 0x0f));
	stop();
}

void WeatherDisplay::start()
{
	pinMode(_dioPin, OUTPUT);
	BIT_DELAY;
}

void WeatherDisplay::stop()
{
	pinMode(_dioPin, OUTPUT);
	BIT_DELAY;

	pinMode(_clkPin, INPUT);
	BIT_DELAY;

	pinMode(_dioPin, INPUT);
	BIT_DELAY;
}

bool WeatherDisplay::writeByte(uint8_t byte)
{
	uint8_t data = byte;

	// send byte:
	for(uint8_t i = 0; i < 8; i++)
	{
		pinMode(_clkPin, OUTPUT);
		BIT_DELAY;

		if(data & 0x01)
		{
			pinMode(_dioPin, INPUT);
		}
		else
		{
			pinMode(_dioPin, OUTPUT);
		}

		BIT_DELAY;

		pinMode(_clkPin, INPUT);
		BIT_DELAY;
		data = data >> 1;
	}

	// wait for acknowledge:
	pinMode(_clkPin, OUTPUT);
	pinMode(_dioPin, INPUT);
	BIT_DELAY;

	pinMode(_clkPin, INPUT);
	BIT_DELAY;

	uint8_t ack = digitalRead(_dioPin);

	if(ack == 0)
	{
		pinMode(_dioPin, OUTPUT);
	}

	BIT_DELAY;
	pinMode(_clkPin, OUTPUT);
	BIT_DELAY;

	return ack;
}

