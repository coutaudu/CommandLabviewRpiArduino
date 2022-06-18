PROGS = Arduino.ino

all: $(PROGS)
	arduino --board arduino:avr:uno --upload Arduino.ino --port /dev/ttyACM0

clean:
	rm *~
