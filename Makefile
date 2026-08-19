FQBN := arduino:avr:uno
PORT := /dev/ttyUSB0

.PHONY: init build up

init:
	arduino-cli compile --fqbn $(FQBN) --only-compilation-database --build-path ./build
	python3 -c "import json;args=json.load(open('build/compile_commands.json'))[0]['arguments'];[print(a) for a in args if a.startswith('-I') or a.startswith('-D')]" > compile_flags.txt
	echo "--target=avr" >> compile_flags.txt

build:
	arduino-cli compile --fqbn $(FQBN) .

up: build
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) .
