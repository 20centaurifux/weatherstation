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
#include <unistd.h>

#include <Event.h>

class EventTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(EventTest);
	CPPUNIT_TEST(testLoop);
	CPPUNIT_TEST(testOverflow);
	CPPUNIT_TEST(testClear);
	CPPUNIT_TEST_SUITE_END();

	template<unsigned long INTERVAL>
	class Increment: public EventCallback
	{
		public:
			Increment(unsigned long& n) : _n(n) {}

			unsigned long operator()()
			{
				++_n;

				return INTERVAL;
			}

		private:
			unsigned long& _n;
	};

	template<unsigned long INTERVAL>
	class Decrement: public EventCallback
	{
		public:
			Decrement(unsigned long& n) : _n(n) {}

			unsigned long operator()()
			{
				--_n;

				return INTERVAL;
			}

		private:
			unsigned long& _n;
	};

	public:
		void testLoop()
		{
			EventLoop<5> loop;

			unsigned long n = 0;

			Increment<0> a(n);
			Increment<500> b(n);
			Decrement<1500> c(n);

			EventId id = loop.timeout(&a, 0);

			CPPUNIT_ASSERT(id > 0);
			CPPUNIT_ASSERT(a.completed() == false);

			id = loop.timeout(&b, 1500);

			CPPUNIT_ASSERT(id > 0);
			CPPUNIT_ASSERT(b.completed() == false);

			id = loop.timeout(&c, 1000);

			CPPUNIT_ASSERT(id > 0);
			CPPUNIT_ASSERT(c.completed() == false);

			for(int i = 0; i < 10; ++i)
			{
				loop.iteration();
				usleep(500000);
			}

			CPPUNIT_ASSERT(n == 5);

			CPPUNIT_ASSERT(a.completed() == true);
			CPPUNIT_ASSERT(b.completed() == false);
			CPPUNIT_ASSERT(c.completed() == false);
		}

		void testOverflow()
		{
			EventLoop<2> loop;

			unsigned long n = 0;

			Increment<100> a(n);
			Decrement<100> b(n);
			Increment<100> c(n);

			EventId id = loop.timeout(&a, 0);

			CPPUNIT_ASSERT(id > 0);
			CPPUNIT_ASSERT(a.completed() == false);

			id = loop.timeout(&b, 0);

			CPPUNIT_ASSERT(id > 0);
			CPPUNIT_ASSERT(b.completed() == false);

			id = loop.timeout(&c, 0);

			CPPUNIT_ASSERT(id == 0);

			for(int i = 0; i < 10; ++i)
			{
				loop.iteration();
				usleep(500000);
			}

			CPPUNIT_ASSERT(n == 0);
		}

		void testClear()
		{
			EventLoop<2> loop;

			unsigned long n = 0;

			Increment<100> a(n);
			Decrement<100> b(n);

			EventId id = loop.timeout(&a, 0);

			CPPUNIT_ASSERT(id > 0);
			CPPUNIT_ASSERT(a.completed() == false);

			id = loop.timeout(&b, 0);

			CPPUNIT_ASSERT(id > 0);
			CPPUNIT_ASSERT(b.completed() == false);

			for(int i = 0; i < 10; ++i)
			{
				if(i == 5)
				{
					loop.clear(id);
				}

				loop.iteration();

				usleep(500000);
			}

			CPPUNIT_ASSERT(n == 5);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(EventTest);

int
main(int argc, char *argv[])
{
	CppUnit::TextUi::TestRunner runner;

	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
	runner.run();

	return 0;
}

