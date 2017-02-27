CC=avr-gcc
DEFINES=-D__MCU_attiny13__ -D__ARCH_AVR__
LIBS=../../src/transport.c ../../src/wirebus.c

all: wirbus

wirbus: firmware-button-attiny13.c
	@echo 'Building target: $@'
	 $(CC) -Os -mmcu=attiny13 $(DEFINES) -I../../include -I./ firmware-button-attiny13.c $(LIBS)  -o firmware-button-attiny13.elf
	-avr-objdump -h -S firmware-button-attiny13.elf  >firmware-button-attiny13.lss
	avr-size --format=avr --mcu=attiny13a firmware-button-attiny13.elf
	avr-objcopy -j .text -j .data -O ihex firmware-button-attiny13.elf ../../hex/firmware-button-attiny13.hex

