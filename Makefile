FQBN := arduino:avr:uno
PORT := /dev/ttyUSB0

.PHONY: build up

build:
	arduino-cli compile --fqbn $(FQBN) .

up:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) .
