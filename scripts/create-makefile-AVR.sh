

function create_makefile
{
	echo "Creating Makefile for $PROJECTNAME $MCU $ARCH in $WORKDIR"
	MAKEFILENAME="$WORKDIR/Makefile";
	PROJECTLIBS='../../src/transport.c ../../src/transport.c';

	echo "CC=avr-gcc" > $MAKEFILENAME
	echo "DEFINES=-Dmcu=$MCU -D__ARCH_AVR__" >> $MAKEFILENAME
	echo "LIBS=$PROJECTLIBS" >> $MAKEFILENAME
	echo 'all: wirbus' >> $MAKEFILENAME
	echo -e "\nwirbus: $PROJECTNAME.c" >> $MAKEFILENAME
        echo -e "\t@echo 'Building target: \$@'" >> $MAKEFILENAME 
	echo -e "\t \$(CC) -Os \$(DEFINES) -I../../include \$(LIBS) $TRANSPORT -o $PROJECTNAME.elf" >> $MAKEFILENAME
	echo -e "\t-avr-objdump -h -S $PROJECTNAME.elf  >"$PROJECTNAME.lss""	>> $MAKEFILENAME
        echo -e "\tavr-size --format=avr --mcu=attiny13a $PROJECTNAME.elf" >> $MAKEFILENAME
	echo -e "\tavr-objcopy -j .text -j .data -O ihex $PROJECTNAME.elf ../../hex/$PROJECTNAME.hex" >> $MAKEFILENAME
	echo "Creating Makefile OK"


}





