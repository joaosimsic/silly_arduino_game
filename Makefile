FQBN := arduino:avr:uno
PORT := /dev/ttyUSB0

.PHONY: init build up

init:
	arduino-cli compile --fqbn $(FQBN) --only-compilation-database --build-path ./build

build:
	arduino-cli compile --fqbn $(FQBN) .

up: build
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) .
