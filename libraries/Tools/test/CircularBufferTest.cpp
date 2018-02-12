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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <CircularBuffer.h>

class CircularBufferTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CircularBufferTest);
	CPPUNIT_TEST(testCount);
	CPPUNIT_TEST(testBuffer);
	CPPUNIT_TEST_SUITE_END();

	public:
		void testCount()
		{
			CircularBuffer<int32_t, 10> buffer;

			for(int32_t i = 1; i <= 20; ++i)
			{
				buffer.append(0);

				if(i <= 10)
				{
					CPPUNIT_ASSERT(buffer.count() == (size_t)i);
				}
				else
				{
					CPPUNIT_ASSERT(buffer.count() == 10);
				}
			}
		}

		void testBuffer()
		{
			CircularBuffer<int32_t, 100> buffer;

			int32_t sum = 0;

			for(int32_t i = 1; i <= 200; ++i)
			{
				buffer.append(i);

				if(i > 100)
				{
					sum += i;
				}
			}

			Sum f;

			buffer.forEach(f);

			CPPUNIT_ASSERT(sum == f.sum());
		}

	private:
		class Sum : public ProcessBufferValue<int32_t>
		{
			public:
				Sum() : _sum(0) {}

				void operator()(int32_t value)
				{
					_sum += value;
				}

				int32_t sum() const
				{
					return _sum;
				}

			private:
				int32_t _sum;
		};
};

CPPUNIT_TEST_SUITE_REGISTRATION(CircularBufferTest);

int
main(int argc, char *argv[])
{
	CppUnit::TextUi::TestRunner runner;

	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
	runner.run();

	return 0;
}

