PORT?=/dev/ttyUSB0
BOARD?=arduino:avr:nano

all:
	arduino --board $(BOARD) --verify ./weatherstation.cpp

upload:
	arduino --board $(BOARD) --port $(PORT) --upload ./weatherstation.cpp
