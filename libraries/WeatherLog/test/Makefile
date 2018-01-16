all:
	g++ -g -O0 ../LogValue.cpp ./LogValueTest.cpp -std=c++11 -o ./TestLogValue -Wall -lcppunit -I../
	g++ -g -O0 ../LogValue.cpp ../Delta.cpp ./DeltaTest.cpp -std=c++11 -o ./TestDelta -Wall -lcppunit -I../

clean:
	rm -f ./TestLogValue
	rm -f ./TestDelta
