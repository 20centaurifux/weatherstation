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

#include <WeatherSensors.h>
#include <Tools.h>

void WeatherSensors::begin()
{
	_rtcFound = _rtc.begin();
	_bmp180Found = _bmp180.begin();

	pinMode(_uvPin, INPUT);
	pinMode(_ref3VPin, INPUT);
}

bool WeatherSensors::now(DateTime& now)
{
	if(_rtcFound)
	{
		now = _rtc.now();
	}

	return _rtcFound;
}

int WeatherSensors::light()
{
	return averageAnalogRead(_ldrPin);
}

bool WeatherSensors::measure(LogValue& value)
{
	bool success = false;

	_builder.reset();

	DateTime dt;

	if(now(dt))
	{
		uint32_t timestamp = dt.unixtime();

		_builder.setTimestamp(timestamp);

		measureBMP180();
		measureDHT11();
		measureUV();

		value = _builder.build();

		success = true;
	}

	return success;
}

void WeatherSensors::measureBMP180()
{
	if(_bmp180Found)
	{
		double temperature = 0.0;

		if(measureBMP180_Temperature(temperature))
		{
			measureBMP180_Pressure(temperature);
		}
	}
	else
	{
		_builder.setTemperatureFailure(ERROR_NO_SENSOR);
		_builder.setPressureFailure(ERROR_NO_SENSOR);
	}
}

bool WeatherSensors::measureBMP180_Temperature(double& temperature)
{
	int errorCode = 0;
	int status = _bmp180.startTemperature();

	if(status)
	{
		delay(status);

		status = _bmp180.getTemperature(temperature);

		if(status)
		{
			_builder.setTemperature(temperature);
		}
		else
		{
			errorCode = ERROR_BMP180_GET_TEMPERATURE;
		}
	}
	else
	{
		errorCode = ERROR_BMP180_START_TEMPERATURE;
	}

	if(errorCode)
	{
		_builder.setTemperatureFailure(errorCode);
		_builder.setPressureFailure(errorCode);
	}

	return errorCode == 0;
}

void WeatherSensors::measureBMP180_Pressure(double temperature)
{
	int status = _bmp180.startPressure(BMP180_OVERSAMPLING);

	if(status)
	{
		delay(status);

		double pressure;

		status = _bmp180.getPressure(pressure, temperature);

		if(status)
		{
			_builder.setPressure(pressure);
		}
		else
		{
			_builder.setPressureFailure(ERROR_BMP180_GET_PRESSURE);
		}
	}
	else
	{
		_builder.setPressureFailure(ERROR_BMP180_START_PRESSURE);
	}
}

void WeatherSensors::measureDHT11()
{
	byte temperature;
	byte humidity;
	int err;

	err = _dht11.read(_dht11Pin, &temperature, &humidity, NULL);
	
	if(err == SimpleDHTErrSuccess)
	{
		_builder.setHumidity(humidity);
	}
	else
	{
		_builder.setHumidityFailure(err);
	}
}

void WeatherSensors::measureUV()
{
	int uvLevel = averageAnalogRead(_uvPin);
	int refLevel = averageAnalogRead(_ref3VPin);

	float outputVoltage = 3.3 / refLevel * uvLevel;
	float uvIntensity = mapFloat(outputVoltage, 0.99, 2.8, 0.0, 15.0);

	_builder.setUV(uvIntensity);
}

