#export PATH := "/C/Program Files/HI-TECH Software/PICC/9.50/BIN":$(PATH)
DEFINES=-DWIREBUS_COMMAND_DISPATCHER
all: errorall

pic: checkchip picsetcompiler picompilefirmware  	

#avr: checkchip avrsetcompiler transport avrcompilefirmware

picompilefirmware: pic$(CHIP)

avrcompilefw: firmware.c
	$(CC) -p12F629  firmware.c  $(LIBS) $(DEFINES)

pic12f629:
		$(CC)  $(SRC) $(DEFINES) $(LIBS) $(TRANSPORT) wirebus.c

#transport:
#TRANSPORT=transport.c
#if [ -a mcu/transport-$(CHIP).c ] ; then \
#	TRANSPORT=mcu/transport-$(CHIP).c;\
#fi;\
#if [ -a mcu/transport-$(CHIP).s ]; then \
#	TRANSPORT=mcu/transport-$(CHIP).s;\
#fi;\
#	@echo "--- Selected transport  $(TRANSPORT)"

picsetcompiler:
CC=sdcc --use-non-free
CFLAGS += -p$(CHIP)
DEFINES += -D__FAMILY_PIC__

avrsetcompiler:
CC=avr-gcc
CFLAGS += -p$(CHIP)
DEFINES += -D__MCU_$(CHIP)__
DEFINES += -D__FAMILY_AVR__

	
checkchip:
#	ifndef $(CHIP)
#			$(error CHIP is undefined. Use make pic CHIP=12F629)
#	endif
	@echo "--- Selected platform PIC $(CHIP)..."
	
errorall:
	$(error Please select platform use "make pic" or "make avr") 