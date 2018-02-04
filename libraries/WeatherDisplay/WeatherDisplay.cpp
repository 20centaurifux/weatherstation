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
#define SEGMENT_COMMA 0b00010000

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

void WeatherDisplay::showFloat(float f)
{
	_val = f * 10;

	if(_val < 100)
	{
		_val = f * 10;
	}

	_fmt = WEATHER_DISPLAY_FORMAT_FLOAT;

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

void WeatherDisplay::showError(int err)
{
	_val = err;
	_fmt = WEATHER_DISPLAY_FORMAT_ERROR;

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
	else if(_fmt == WEATHER_DISPLAY_FORMAT_FLOAT)
	{
		writeFloat(_val);
	}
	else if(_fmt == WEATHER_DISPLAY_FORMAT_TEMPERATURE)
	{
		write2DigitsWithSuffix(_val, SEGMENT_C);
	}
	else
	{
		write2UDigitsWithPrefix(_val, SEGMENT_E);
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

void WeatherDisplay::writeFloat(int num)
{
	uint8_t digits[4] = {0};
	int divisor = 100;

	for(int8_t k = 0; k < 3; ++k)
	{
		int d = num / divisor;
		uint8_t digit = 0;

		if(d == 0)
		{
			if(k)
			{
				digit = encodeDigit(d);
			}
		}
		else
		{
			digit = encodeDigit(d);
			num -= d * divisor;
		}
  
		digits[k == 2 ? 3 : k] = digit;
		divisor /= 10;
	}

	digits[2] = SEGMENT_COMMA;

	setSegments(digits, 4);
}

void WeatherDisplay::write2DigitsWithSuffix(int n, uint8_t suffix)
{
	uint8_t digits[4] = {0};

	int num = abs(n);

	int digit = num / 10;

	if(digit)
	{
		digits[1] = encodeDigit(digit);

		if(n < 0)
		{
			digits[0] = SEGMENT_MINUS;
		}
	}
	else if(n < 0)
	{
		digits[1] = SEGMENT_MINUS;
	}

	digits[2] = encodeDigit(num - (digit * 10));
	digits[3] = suffix;

	setSegments(digits, 4);
}

void WeatherDisplay::write2UDigitsWithPrefix(int n, uint8_t prefix)
{
	uint8_t digits[4] = {0};

	int digit = n / 10;

	if(digit)
	{
		digits[1] = prefix;
		digits[2] = encodeDigit(digit);
		digits[3] = encodeDigit(n - (digit * 10));

	}
	else
	{
		digits[2] = prefix;
		digits[3] = encodeDigit(n);
	}

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

