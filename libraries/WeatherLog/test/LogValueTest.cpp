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
#include <random>

#include <LogValue.h>

class LogValueBuilderTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(LogValueBuilderTest);
	CPPUNIT_TEST(testTimestamp);
	CPPUNIT_TEST(testTemperature);
	CPPUNIT_TEST(testPressure);
	CPPUNIT_TEST(testHumidty);
	CPPUNIT_TEST(testUV);
	CPPUNIT_TEST(testTemperatureOutOfBounds);
	CPPUNIT_TEST(testPressureOutOfBounds);
	CPPUNIT_TEST(testHumidyOutOfBounds);
	CPPUNIT_TEST(testUVOutOfBounds);
	CPPUNIT_TEST(testError);
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp()
		{
			_builder.reset();
		}

		void testTimestamp()
		{
			for(uint32_t i = 0; i <= 4200; ++i)
			{
				uint32_t timestamp = i * 1000000;

				_builder.setTimestamp(timestamp);

				auto v = _builder.build();

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(timestamp == LOG_VALUE_DECODE_TIMESTAMP(v));
			}

			_builder.setTimestamp(UINT32_MAX);

			auto v = _builder.build();

			CPPUNIT_ASSERT(UINT32_MAX == LOG_VALUE_DECODE_TIMESTAMP(v));
		}

		void testTemperature()
		{
			for(double f = LOG_VALUE_TEMPERATURE_MIN; f <= LOG_VALUE_TEMPERATURE_MAX; f += 0.1)
			{
				bool success = _builder.setTemperature(f);

				CPPUNIT_ASSERT(success == true);

				auto v = _builder.build();

				double result =  abs(f - LOG_VALUE_DECODE_TEMPERATURE(v));

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(std::fpclassify(result) == FP_ZERO);
			}
		}

		void testPressure()
		{
			for(uint16_t p = LOG_VALUE_PRESSURE_MIN; p <= LOG_VALUE_PRESSURE_MAX; ++p)
			{
				bool success = _builder.setPressure(p);

				CPPUNIT_ASSERT(success == true);

				auto v = _builder.build();

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(p == LOG_VALUE_DECODE_PRESSURE(v));;
			}
		}

		void testHumidty()
		{
			for(uint8_t h = LOG_VALUE_HUMIDITY_MIN; h <= LOG_VALUE_HUMIDITY_MAX; ++h)
			{
				bool success = _builder.setHumidity(h);

				CPPUNIT_ASSERT(success == true);

				auto v = _builder.build();

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(h == LOG_VALUE_DECODE_HUMIDITY(v));;
			}
		}

		void testUV()
		{
			for(double f = LOG_VALUE_UV_MIN; f <= LOG_VALUE_UV_MAX; f += 0.1)
			{
				bool success = _builder.setUV(f);

				CPPUNIT_ASSERT(success == true);

				auto v = _builder.build();

				double result =  abs(f - LOG_VALUE_DECODE_UV(v));

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(std::fpclassify(result) == FP_ZERO);
			}
		}

		void testTemperatureOutOfBounds()
		{
			_builder.setTemperature(10.0);

			for(double f : {LOG_VALUE_TEMPERATURE_MIN - 0.1, LOG_VALUE_TEMPERATURE_MAX + 0.1})
			{
				bool success = _builder.setTemperature(f);

				CPPUNIT_ASSERT(success == false);

				success = _builder.setTemperature(f);

				CPPUNIT_ASSERT(success == false);

				auto v = _builder.build();

				double result =  abs(10.0 - LOG_VALUE_DECODE_TEMPERATURE(v));

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(std::fpclassify(result) == FP_ZERO);
			}
		}

		void testPressureOutOfBounds()
		{
			_builder.setPressure(1000);

			for(uint16_t p : {LOG_VALUE_PRESSURE_MIN - 1, LOG_VALUE_PRESSURE_MAX + 1})
			{
				bool success = _builder.setPressure(p);

				CPPUNIT_ASSERT(success == false);

				success = _builder.setPressure(p);

				CPPUNIT_ASSERT(success == false);

				auto v = _builder.build();

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(LOG_VALUE_DECODE_PRESSURE(v) == 1000);
			}
		}

		void testHumidyOutOfBounds()
		{
			_builder.setHumidity(60);

			for(int8_t h : {LOG_VALUE_HUMIDITY_MIN - 1, LOG_VALUE_HUMIDITY_MAX + 1})
			{
				bool success = _builder.setHumidity(h);

				CPPUNIT_ASSERT(success == false);

				success = _builder.setHumidity(h);

				CPPUNIT_ASSERT(success == false);

				auto v = _builder.build();

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(LOG_VALUE_DECODE_HUMIDITY(v) == 60);
			}
		}

		void testUVOutOfBounds()
		{
			_builder.setUV(7.3);

			for(double f : {LOG_VALUE_UV_MIN - 0.1, LOG_VALUE_UV_MAX + 0.1})
			{
				bool success = _builder.setUV(f);

				CPPUNIT_ASSERT(success == false);

				success = _builder.setUV(f);

				CPPUNIT_ASSERT(success == false);

				auto v = _builder.build();

				double result =  abs(7.3 - LOG_VALUE_DECODE_UV(v));

				CPPUNIT_ASSERT(v.errors == 0);
				CPPUNIT_ASSERT(std::fpclassify(result) == FP_ZERO);
			}
		}

		void testError()
		{
			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution<uint8_t> errDist(0, LOG_VALUE_MAX_ERROR);

			int errCode = errDist(rng);

			auto v = _builder.build();

			CPPUNIT_ASSERT(v.errors == 0);

			bool success = _builder.setTemperatureFailure(errCode);

			v = _builder.build();

			CPPUNIT_ASSERT(success == true);
			CPPUNIT_ASSERT(LOG_VALUE_TEMPERATURE_ERROR(v));
			CPPUNIT_ASSERT(LOG_VALUE_TEMPERATURE_ERROR_CODE(v) == errCode);

			success = _builder.setTemperatureFailure(LOG_VALUE_MAX_ERROR + 1);

			CPPUNIT_ASSERT(success == false);

			_builder.setPressureFailure(errCode);

			v = _builder.build();

			CPPUNIT_ASSERT(LOG_VALUE_PRESSURE_ERROR(v));
			CPPUNIT_ASSERT(LOG_VALUE_PRESSURE_ERROR_CODE(v) == errCode);

			success = _builder.setPressureFailure(LOG_VALUE_MAX_ERROR + 1);

			CPPUNIT_ASSERT(success == false);

			success = _builder.setUVFailure(errCode);

			v = _builder.build();

			CPPUNIT_ASSERT(LOG_VALUE_UV_ERROR(v));
			CPPUNIT_ASSERT(LOG_VALUE_UV_ERROR_CODE(v) == errCode);

			success = _builder.setUVFailure(LOG_VALUE_MAX_ERROR + 1);

			CPPUNIT_ASSERT(success == false);

			success = _builder.setHumidityFailure(errCode);

			v = _builder.build();

			CPPUNIT_ASSERT(LOG_VALUE_HUMIDITY_ERROR(v));
			CPPUNIT_ASSERT(LOG_VALUE_HUMIDITY_ERROR_CODE(v) == errCode);

			success = _builder.setHumidityFailure(LOG_VALUE_MAX_ERROR + 1);

			CPPUNIT_ASSERT(success == false);
		}

	private:
		LogValueBuilder _builder;
};

CPPUNIT_TEST_SUITE_REGISTRATION(LogValueBuilderTest);

class LogValueSerializationTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(LogValueSerializationTest);
	CPPUNIT_TEST(testSerialization);
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp()
		{
			_builder.reset();
		}

		LogValueSerializationTest() :
			_rng(std::mt19937(_dev())),
			_timeDist(std::uniform_int_distribution<uint32_t>(0, UINT32_MAX)),
			_tempDist(std::uniform_real_distribution<double>(LOG_VALUE_TEMPERATURE_MIN, LOG_VALUE_TEMPERATURE_MAX)),
			_pressDist(std::uniform_int_distribution<uint16_t>(LOG_VALUE_PRESSURE_MIN, LOG_VALUE_PRESSURE_MAX)),
			_humDist(std::uniform_int_distribution<uint8_t>(LOG_VALUE_HUMIDITY_MIN, LOG_VALUE_HUMIDITY_MAX)),
			_uvDist(std::uniform_real_distribution<double>(LOG_VALUE_UV_MIN, LOG_VALUE_UV_MAX)),
			_errDist(std::uniform_int_distribution<uint8_t>(0, LOG_VALUE_MAX_ERROR)) {}

		void testSerialization()
		{
			for(int i = 0; i < 100000; ++i)
			{
				setRandomTimestamp();
				setRandomTemperature();
				setRandomPressure();
				setRandomHumidty();
				setRandomUV();

				auto v = _builder.build();

				uint8_t bytes[SERIALIZED_LOG_VALUE_SIZE];
				LogValue copy;

				SerializeLogValue(v, bytes);
				DeserializeLogValue(bytes, copy);

				CPPUNIT_ASSERT(v.timestamp == copy.timestamp);
				CPPUNIT_ASSERT(v.temp == copy.temp);
				CPPUNIT_ASSERT(v.hPa == copy.hPa);
				CPPUNIT_ASSERT(v.hum == copy.hum);
				CPPUNIT_ASSERT(v.uv == copy.uv);
				CPPUNIT_ASSERT(v.errors == copy.errors);
			}
		}

	private:
		std::random_device _dev;
		std::mt19937 _rng;
		std::uniform_int_distribution<uint32_t> _timeDist;
		std::uniform_real_distribution<double> _tempDist;
		std::uniform_int_distribution<uint16_t> _pressDist;
		std::uniform_int_distribution<uint8_t> _humDist;
		std::uniform_real_distribution<double> _uvDist;
		std::uniform_int_distribution<uint8_t> _errDist;
		LogValueBuilder _builder;

		void setRandomTimestamp()
		{
			_builder.setTimestamp(_timeDist(_rng));
		}

		void setRandomTemperature()
		{
			uint8_t errCode = _errDist(_rng);

			if(errCode % 3 == 0)
			{
				_builder.setTemperatureFailure(errCode);
			}
			else
			{
				_builder.setTemperature(_tempDist(_rng));
			}
		}

		void setRandomPressure()
		{
			uint8_t errCode = _errDist(_rng);

			if(errCode % 5 == 0)
			{
				_builder.setPressureFailure(errCode);
			}
			else
			{
				_builder.setPressure(_pressDist(_rng));
			}
		}

		void setRandomHumidty()
		{
			uint8_t errCode = _errDist(_rng);

			if(errCode % 3 == 0)
			{
				_builder.setHumidityFailure(errCode);
			}
			else
			{
				_builder.setHumidity(_humDist(_rng));
			}
		}

		void setRandomUV()
		{
			uint8_t errCode = _errDist(_rng);

			if(errCode % 7 == 0)
			{
				_builder.setUVFailure(errCode);
			}
			else
			{
				_builder.setUV(_uvDist(_rng));
			}
		}
};

int
main(int argc, char *argv[])
{
	CppUnit::TextUi::TestRunner runner;

	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
	runner.run();

	return 0;
}

