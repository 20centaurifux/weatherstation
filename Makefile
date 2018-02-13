PORT?=/dev/ttyUSB0
BOARD?=arduino:avr:nano
PREFS?=--pref sketchbook.path=$(PWD)

all:
	arduino $(PREFS) --board $(BOARD) --verify ./WeatherStation.cpp

upload:
	arduino $(PREFS) --board $(BOARD) --port $(PORT) --upload ./WeatherStation.cpp
