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
#include <iostream>
#include <fstream>
#include <cmath>
#include <queue>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <WeatherLog.h>

static LogValue
ParseCSVLine(char *line)
{
	LogValueBuilder builder;
	int i = 0;
	bool error = false;

	char *ptr = strtok(line, ",");

	while(ptr)
	{
		switch(i)
		{
			case 0:
				builder.setTimestamp(atoi(ptr));
				break;

			case 1:
			case 3:
			case 5:
			case 7:
				error = strcmp(ptr, "0");
				break;

			case 2:
				if(error)
				{
					builder.setTemperatureFailure(atoi(ptr));
				}
				else
				{
					builder.setTemperature(atof(ptr));
				}

				break;

			case 4:
				if(error)
				{
					builder.setPressureFailure(atoi(ptr));
				}
				else
				{
					builder.setPressure(atoi(ptr));
				}

				break;

			case 6:
				if(error)
				{
					builder.setHumidityFailure(atoi(ptr));
				}
				else
				{
					builder.setHumidity(atoi(ptr));
				}

				break;

			case 8:
				if(error)
				{
					builder.setUVFailure(atoi(ptr));
				}
				else
				{
					builder.setUV(atof(ptr));
				}

				break;

			default:
				std::cerr << "Warning, ignoring colum from CSV line: \"" << ptr << "\"" << std::endl;
		}

		ptr = strtok(nullptr, ",");
		i++;
	}

	return builder.build();
}

class WeatherLogTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(WeatherLogTest);
	CPPUNIT_TEST(testFill);
	CPPUNIT_TEST(testOverlapLogValue);
	CPPUNIT_TEST(testOverlapDelta);
	CPPUNIT_TEST_SUITE_END();

	public:
		WeatherLogTest() : _log(120) {}

		void setUp()
		{
			_log.clear();
		}

		void testFill()
		{
			loadCSVFile("test-data/log.32.csv");

			assertLogValues();
		}

		void testOverlapLogValue()
		{
			loadCSVFile("test-data/log.39.csv");

			_values.pop();

			assertLogValues();
		}

		void testOverlapDelta()
		{
			loadCSVFile("test-data/log.34.csv");

			_values.pop();

			assertLogValues();
		}

	private:
		WeatherLog<32> _log;
		std::queue<LogValue> _values;

		void loadCSVFile(const char *path)
		{
			std::fstream in(path);
			char line[64];

			while(in >> line)
			{
				LogValue value = ParseCSVLine(line);

				_log.append(value);
				_values.push(value);
			}

			in.close();
		}

		class AssertNextLogValue : public ProcessLogValue
		{
			public:
				AssertNextLogValue(std::queue<LogValue>& queue)
					: _queue(queue) {}

				void operator()(const LogValue& a)
				{
					LogValue b = _queue.front();

					CPPUNIT_ASSERT(a.timestamp == b.timestamp);
					CPPUNIT_ASSERT(a.uv == b.uv);
					CPPUNIT_ASSERT(a.errors == b.errors);
					CPPUNIT_ASSERT(a.hum == b.hum);
					CPPUNIT_ASSERT(a.temp == b.temp);
					CPPUNIT_ASSERT(a.hPa == b.hPa);

					_queue.pop();
				}

			private:
				std::queue<LogValue>& _queue;
		};

		void assertLogValues()
		{
			AssertNextLogValue f(_values);

			_log.forEach(f);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(WeatherLogTest);

int
main(void)
{
	CppUnit::TextUi::TestRunner runner;

	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
	runner.run();

	return 0;
}

