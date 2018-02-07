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

#include <fstream>
#include <cstring>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <StationModel.h>

class PressureTendencyTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(PressureTendencyTest);
	CPPUNIT_TEST(testTendency0);
	CPPUNIT_TEST(testTendency1);
	CPPUNIT_TEST(testTendency2);
	CPPUNIT_TEST(testTendency3);
	CPPUNIT_TEST(testTendency4);
	CPPUNIT_TEST(testTendency5);
	CPPUNIT_TEST(testTendency6);
	CPPUNIT_TEST(testTendency7);
	CPPUNIT_TEST(testTendency8);
	CPPUNIT_TEST(testOutOfBounds);
	CPPUNIT_TEST(testOverflow);
	CPPUNIT_TEST_SUITE_END();

	public:
		void testTendency0()
		{
			loadCSVFile("test-data/pressure-tendency-0.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 0);
		}

		void testTendency1()
		{
			loadCSVFile("test-data/pressure-tendency-1.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 1);
		}

		void testTendency2()
		{
			loadCSVFile("test-data/pressure-tendency-2.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 2);
		}

		void testTendency3()
		{
			loadCSVFile("test-data/pressure-tendency-3.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 3);
		}

		void testTendency4()
		{
			loadCSVFile("test-data/pressure-tendency-4.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 4);
		}

		void testTendency5()
		{
			loadCSVFile("test-data/pressure-tendency-5.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 5);
		}

		void testTendency6()
		{
			loadCSVFile("test-data/pressure-tendency-6.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 6);
		}

		void testTendency7()
		{
			loadCSVFile("test-data/pressure-tendency-7.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 7);
		}

		void testTendency8()
		{
			loadCSVFile("test-data/pressure-tendency-8.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 8);
		}

		void testOutOfBounds()
		{
			loadCSVFile("test-data/pressure-tendency-out-of-bounds.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 1);
		}

		void testOverflow()
		{
			loadCSVFile("test-data/pressure-tendency-overflow.csv");

			CPPUNIT_ASSERT(_tendency.tendency() == 4);
		}

	private:
		PressureTendency _tendency;

		void loadCSVFile(const char *path)
		{
			std::fstream in(path);
			char line[64];
			bool first = true;

			while(in >> line)
			{
				char *ptr = strtok(line, ",");

				int timestamp = 0;
				int pressure = 0;

				if(ptr)
				{
					timestamp = atoi(ptr);

					ptr = strtok(nullptr, ",");

					if(ptr)
					{
						pressure = atoi(ptr);
					}
				}

				if(timestamp && pressure)
				{
					if(first)
					{
						_tendency.start(timestamp, pressure);
						first = false;
					}
					else
					{
						_tendency.update(timestamp, pressure);
					}
				}
			}

			in.close();
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PressureTendencyTest);

int
main(int argc, char *argv[])
{
	CppUnit::TextUi::TestRunner runner;

	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
	runner.run();

	return 0;
}

