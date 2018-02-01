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

#include <cstring>
#include <cmath>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <Delta.h>

class WeatherDeltaTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(WeatherDeltaTest);
	CPPUNIT_TEST(testTimestamp);
	CPPUNIT_TEST(testTemperature);
	CPPUNIT_TEST(testTemperatureFailure);
	CPPUNIT_TEST(testTemperatureOutOfBounds);
	CPPUNIT_TEST(testPressure);
	CPPUNIT_TEST(testPressureFailure);
	CPPUNIT_TEST(testPressureOutOfBounds);
	CPPUNIT_TEST(testHumidity);
	CPPUNIT_TEST(testHumidityFailure);
	CPPUNIT_TEST(testHumidityOutOfBounds);
	CPPUNIT_TEST(testUV);
	CPPUNIT_TEST(testUVFailure);
	CPPUNIT_TEST(testUVOutOfBounds);
	CPPUNIT_TEST(testAll);
	CPPUNIT_TEST(testApply);
	CPPUNIT_TEST_SUITE_END();

	#define INCREMENT_TIMESTAMP(v) _valueBuilder.setTimestamp(LOG_VALUE_DECODE_TIMESTAMP(v) + 120);

	public:
		WeatherDeltaTest() : _deltaBuilder(120) {}

		void setUp()
		{
			_valueBuilder.reset();

			_valueBuilder.setTemperature(0.0);
			_valueBuilder.setPressure(LOG_VALUE_PRESSURE_MIN);
			_valueBuilder.setHumidity(0);
			_valueBuilder.setUV(0.0);

			_value = _valueBuilder.build();
		}

		void testTimestamp()
		{
			for(uint8_t i = 0; i < 63; ++i)
			{
				_valueBuilder.setTimestamp(LOG_VALUE_DECODE_TIMESTAMP(_value) + 120 + i);

				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				CPPUNIT_ASSERT(_reader.seconds() == i);
			}

			_valueBuilder.setTimestamp(LOG_VALUE_DECODE_TIMESTAMP(_value) + 120 + 64);
			testInvalidBuild();
		}

		void testTemperature()
		{
			for(double f : {0.1, 1.9, 5.0, -2.5, 12.7, -12.8})
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setTemperature(LOG_VALUE_DECODE_TEMPERATURE(_value) + f);
	
				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				assertOnlyTemperatureSet();
				CPPUNIT_ASSERT(_reader.temperature() == f * 10);

				_value = to;
			}
		}

		void testTemperatureFailure()
		{
			_valueBuilder.setTemperatureFailure(0);
			_value = _valueBuilder.build();

			for(int i = 1; i <= LOG_VALUE_MAX_ERROR; ++i)
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setTemperatureFailure(i);

				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				assertOnlyTemperatureSet();
				CPPUNIT_ASSERT(_reader.temperature() == 1);
				CPPUNIT_ASSERT(_reader.temperatureFailed() == true);

				_value = to;
			}
		}

		void testTemperatureOutOfBounds()
		{
			for(double f : {20.0, -20.0})
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setTemperature(f);
				testInvalidBuild();
			}
		}

		void testPressure()
		{
			for(int8_t p : {2, -1, 3, 8, -1, -8, 4, -4})
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setPressure(LOG_VALUE_DECODE_PRESSURE(_value) + p);
	
				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				assertOnlyPressureSet();
				CPPUNIT_ASSERT(_reader.pressure() == p);

				_value = to;
			}
		}

		void testPressureFailure()
		{
			_valueBuilder.setPressureFailure(0);
			_value = _valueBuilder.build();

			for(int i = 1; i <= LOG_VALUE_MAX_ERROR; ++i)
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setPressureFailure(i);

				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				assertOnlyPressureSet();
				CPPUNIT_ASSERT(_reader.pressure() == 1);
				CPPUNIT_ASSERT(_reader.pressureFailed() == true);

				_value = to;
			}
		}

		void testPressureOutOfBounds()
		{
			for(int8_t p : {9, -9})
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setPressure(LOG_VALUE_DECODE_PRESSURE(_value) + p);
				testInvalidBuild();
			}
		}

		void testHumidity()
		{
			for(int8_t h : {16, -14, 63, -1, -63}) // , -10, 64, -64})
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setHumidity(LOG_VALUE_DECODE_HUMIDITY(_value) + h);
	
				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				assertOnlyHumidtySet();
				CPPUNIT_ASSERT(_reader.humidity() == h);

				_value = to;
			}
		}

		void testHumidityFailure()
		{
			_valueBuilder.setHumidityFailure(0);
			_value = _valueBuilder.build();

			for(int i = 1; i <= LOG_VALUE_MAX_ERROR; ++i)
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setHumidityFailure(i);

				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				assertOnlyHumidtySet();
				CPPUNIT_ASSERT(_reader.humidity() == 1);
				CPPUNIT_ASSERT(_reader.humidityFailed() == true);

				_value = to;
			}
		}

		void testHumidityOutOfBounds()
		{
			for(int8_t p : {64, -64})
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setHumidity(LOG_VALUE_DECODE_HUMIDITY(_value) + p);
				testInvalidBuild();
			}
		}

		void testUV()
		{
			for(double f : {0.1, 1.9, 5.0, -2.5, 6.3, -6.3})
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setUV(LOG_VALUE_DECODE_UV(_value) + f);
	
				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				assertOnlyUVSet();
				CPPUNIT_ASSERT(_reader.uv() == f * 10);

				_value = to;
			}
		}

		void testUVFailure()
		{
			_valueBuilder.setUVFailure(0);
			_value = _valueBuilder.build();

			for(int i = 1; i <= LOG_VALUE_MAX_ERROR; ++i)
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setUVFailure(i);

				LogValue to = _valueBuilder.build();

				buildDelta(to);
				readDelta();

				assertOnlyUVSet();
				CPPUNIT_ASSERT(_reader.uv() == 1);
				CPPUNIT_ASSERT(_reader.uvFailed() == true);

				_value = to;
			}
		}

		void testUVOutOfBounds()
		{
			for(double f : {6.4, -6.4})
			{
				INCREMENT_TIMESTAMP(_value);
				_valueBuilder.setUV(LOG_VALUE_DECODE_UV(_value) + f);
				testInvalidBuild();
			}
		}

		void testAll()
		{
			INCREMENT_TIMESTAMP(_value);
			_valueBuilder.setTemperature(5.3);
			_valueBuilder.setPressure(LOG_VALUE_PRESSURE_MIN + 3);

			LogValue to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			CPPUNIT_ASSERT(_reader.hasTemperature() == true);
			CPPUNIT_ASSERT(_reader.hasPressure() == true);
			CPPUNIT_ASSERT(_reader.hasHumidity() == false);
			CPPUNIT_ASSERT(_reader.hasUV() == false);

			CPPUNIT_ASSERT(_reader.temperature() == 53);
			CPPUNIT_ASSERT(_reader.pressure() == 3);

			_valueBuilder.setHumidity(44);
			_valueBuilder.setUV(1.7);

			to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			CPPUNIT_ASSERT(_reader.hasTemperature() == true);
			CPPUNIT_ASSERT(_reader.hasPressure() == true);
			CPPUNIT_ASSERT(_reader.hasHumidity() == true);
			CPPUNIT_ASSERT(_reader.hasUV() == true);

			CPPUNIT_ASSERT(_reader.temperature() == 53);
			CPPUNIT_ASSERT(_reader.pressure() == 3);
			CPPUNIT_ASSERT(_reader.humidity() == 44);
			CPPUNIT_ASSERT(_reader.uv() == 17);

			_value = to;

			INCREMENT_TIMESTAMP(_value);
			_valueBuilder.setHumidity(47);
			_valueBuilder.setUV(1.5);

			to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			CPPUNIT_ASSERT(_reader.hasTemperature() == false);
			CPPUNIT_ASSERT(_reader.hasPressure() == false);
			CPPUNIT_ASSERT(_reader.hasHumidity() == true);
			CPPUNIT_ASSERT(_reader.hasUV() == true);

			CPPUNIT_ASSERT(_reader.humidity() == 3);
			CPPUNIT_ASSERT(_reader.uv() == -2);

			CPPUNIT_ASSERT(_reader.temperatureFailed() == false);
			CPPUNIT_ASSERT(_reader.pressureFailed() == false);
			CPPUNIT_ASSERT(_reader.uvFailed() == false);
			CPPUNIT_ASSERT(_reader.humidityFailed() == false);

			_value = to;

			INCREMENT_TIMESTAMP(_value);
			_valueBuilder.setHumidityFailure(50);
			_valueBuilder.setPressureFailure(5);
			_valueBuilder.setUVFailure(20);

			to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			CPPUNIT_ASSERT(_reader.temperatureFailed() == false);
			CPPUNIT_ASSERT(_reader.pressureFailed() == true);
			CPPUNIT_ASSERT(_reader.uvFailed() == true);
			CPPUNIT_ASSERT(_reader.humidityFailed() == true);

			CPPUNIT_ASSERT(_reader.humidity() == 3);
			CPPUNIT_ASSERT(_reader.pressure() == 2);
			CPPUNIT_ASSERT(_reader.uv() == 5);

			_valueBuilder.setTemperature(-30.0);

			_value = _valueBuilder.build();

			INCREMENT_TIMESTAMP(_value);
			_valueBuilder.setTemperatureFailure(10);

			to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			CPPUNIT_ASSERT(_reader.temperatureFailed() == true);
			CPPUNIT_ASSERT(_reader.pressureFailed() == false);
			CPPUNIT_ASSERT(_reader.uvFailed() == false);
			CPPUNIT_ASSERT(_reader.humidityFailed() == false);

			CPPUNIT_ASSERT(_reader.temperature() == 10);
		}

		void testApply()
		{
			_valueBuilder.setTimestamp(120 + 7);
			_valueBuilder.setTemperature(11.2);
			_valueBuilder.setPressure(LOG_VALUE_PRESSURE_MIN + 2);
			_valueBuilder.setHumidity(44);
			_valueBuilder.setUV(1.7);

			LogValue to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			size_t size = ApplyDelta(_value, _bytes, 120);

			CPPUNIT_ASSERT(size == _reader.size());
			assertLogValue(127, 11.2, LOG_VALUE_PRESSURE_MIN + 2, 44, 1.7);

			_value = to;

			_valueBuilder.setTimestamp(240 + 60);
			_valueBuilder.setTemperature(11.2);
			_valueBuilder.setPressure(LOG_VALUE_PRESSURE_MIN + 1);
			_valueBuilder.setHumidity(43);
			_valueBuilder.setUV(2.1);

			to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			size = ApplyDelta(_value, _bytes, 120);

			CPPUNIT_ASSERT(size == _reader.size());
			assertLogValue(300, 11.2, LOG_VALUE_PRESSURE_MIN + 1, 43, 2.1);

			_valueBuilder.setTimestamp(0);
			_valueBuilder.setTemperature(-30.0);
			_valueBuilder.setPressure(LOG_VALUE_PRESSURE_MIN);
			_valueBuilder.setHumidity(0);
			_valueBuilder.setUV(0);

			_value = _valueBuilder.build();

			_valueBuilder.setTimestamp(130);
			_valueBuilder.setTemperatureFailure(10);
			_valueBuilder.setPressure(LOG_VALUE_PRESSURE_MIN + 1);
			_valueBuilder.setHumidityFailure(50);
			_valueBuilder.setUVFailure(1);

			to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			size = ApplyDelta(_value, _bytes, 120);

			CPPUNIT_ASSERT(size == _reader.size());
			CPPUNIT_ASSERT(LOG_VALUE_DECODE_TIMESTAMP(_value) == 130);
			CPPUNIT_ASSERT(LOG_VALUE_TEMPERATURE_ERROR(_value));
			CPPUNIT_ASSERT(LOG_VALUE_TEMPERATURE_ERROR_CODE(_value) == 10);
			CPPUNIT_ASSERT(LOG_VALUE_HUMIDITY_ERROR(_value));
			CPPUNIT_ASSERT(LOG_VALUE_HUMIDITY_ERROR_CODE(_value) == 50);
			CPPUNIT_ASSERT(LOG_VALUE_UV_ERROR_CODE(_value) == 1);

			_valueBuilder.setTimestamp(0);
			_valueBuilder.setTemperature(-30.0);
			_valueBuilder.setPressureFailure(1);
			_valueBuilder.setHumidity(0);
			_valueBuilder.setUV(0);

			_value = _valueBuilder.build();

			_valueBuilder.setTimestamp(150);
			_valueBuilder.setPressureFailure(7);

			to = _valueBuilder.build();

			buildDelta(to);
			readDelta();

			size = ApplyDelta(_value, _bytes, 120);

			CPPUNIT_ASSERT(size == _reader.size());
			CPPUNIT_ASSERT(LOG_VALUE_PRESSURE_ERROR(_value));
			CPPUNIT_ASSERT(LOG_VALUE_PRESSURE_ERROR_CODE(_value) == 7);
		}

	private:
		LogValueBuilder _valueBuilder;
		LogValue _value;
		DeltaBuilder _deltaBuilder;
		uint8_t _bytes[DELTA_MAX_SIZE];
		DeltaReader _reader;

		void buildDelta(const LogValue& to)
		{
			_deltaBuilder.build(_value, to);

			CPPUNIT_ASSERT(_deltaBuilder.hasDelta() == true);

			size_t size = _deltaBuilder.copy(_bytes, DELTA_MAX_SIZE);

			CPPUNIT_ASSERT(size == _deltaBuilder.size());
		}

		void testInvalidBuild()
		{
			LogValue to = _valueBuilder.build();

			_deltaBuilder.build(_value, to);

			CPPUNIT_ASSERT(_deltaBuilder.hasDelta() == false);
		}

		void readDelta()
		{
			_reader.setSource(_bytes);

			size_t calculatedSize = _reader.calcSize();

			_reader.read();

			CPPUNIT_ASSERT(_deltaBuilder.size() == _reader.size());
			CPPUNIT_ASSERT(calculatedSize == _reader.size());
		}

		void assertOnlyTemperatureSet()
		{
			CPPUNIT_ASSERT(_reader.hasTemperature() == true);
			CPPUNIT_ASSERT(_reader.hasPressure() == false);
			CPPUNIT_ASSERT(_reader.hasHumidity() == false);
			CPPUNIT_ASSERT(_reader.hasUV() == false);
		}

		void assertOnlyPressureSet()
		{
			CPPUNIT_ASSERT(_reader.hasTemperature() == false);
			CPPUNIT_ASSERT(_reader.hasPressure() == true);
			CPPUNIT_ASSERT(_reader.hasHumidity() == false);
			CPPUNIT_ASSERT(_reader.hasUV() == false);
		}

		void assertOnlyHumidtySet()
		{
			CPPUNIT_ASSERT(_reader.hasTemperature() == false);
			CPPUNIT_ASSERT(_reader.hasPressure() == false);
			CPPUNIT_ASSERT(_reader.hasHumidity() == true);
			CPPUNIT_ASSERT(_reader.hasUV() == false);
		}

		void assertOnlyUVSet()
		{
			CPPUNIT_ASSERT(_reader.hasTemperature() == false);
			CPPUNIT_ASSERT(_reader.hasPressure() == false);
			CPPUNIT_ASSERT(_reader.hasHumidity() == false);
			CPPUNIT_ASSERT(_reader.hasUV() == true);
		}

		void assertLogValue(uint32_t timestamp, double temp, int pressure, int humidity, double uv)
		{
			CPPUNIT_ASSERT(LOG_VALUE_DECODE_TIMESTAMP(_value) == timestamp);
			CPPUNIT_ASSERT(LOG_VALUE_DECODE_PRESSURE(_value) == pressure);
			CPPUNIT_ASSERT(LOG_VALUE_DECODE_HUMIDITY(_value) == humidity);

			double result = abs(temp - LOG_VALUE_DECODE_TEMPERATURE(_value));

			CPPUNIT_ASSERT(std::fpclassify(result) == FP_ZERO);

			result = abs(uv - LOG_VALUE_DECODE_UV(_value));

			CPPUNIT_ASSERT(std::fpclassify(result) == FP_ZERO);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(WeatherDeltaTest);

int
main(void)
{
	CppUnit::TextUi::TestRunner runner;

	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
	runner.run();

	return 0;
}

