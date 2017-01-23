

function create_makefile
{

echo "Creating Makefile for $PROJECTNAME $MCU $ARCH in $WORKDIR"
MAKEFILENAME="$WORKDIR/Makefile";
PROJECTLIBS='../../src/transport.c ../../src/firmware.c ../../src/wirebus.c';
DEFMCU="__MCU_${MCU}__"

echo "CC=sdcc" > $MAKEFILENAME
echo "DEFINES=-D${DEFMCU} -D__ARCH_STM__" >> $MAKEFILENAME
echo "LIBS=$PROJECTLIBS" >> $MAKEFILENAME
echo ""
echo "WIREBUS_UUID_MAJOR=DEFINE_YOUR_DEVICE_MAJOR_TAG_HERE" >> $MAKEFILENAME
echo "WIREBUS_UUID_MINOR=DEFINE_YOUR_DEVICE_MINOR_TAG_HERE" >> $MAKEFILENAME
echo ""
echo "DEFINES+=-DWIREBUS_UUID_MAJOR=\$(WIREBUS_UUID_MAJOR) -DWIREBUS_UUID_MINOR=\$(WIREBUS_UUID_MINOR)"  >> $MAKEFILENAME
echo 'all: wirebus' >> $MAKEFILENAME
echo -e "\nwirebus: $PROJECTNAME.c" >> $MAKEFILENAME
echo -e "\t@echo 'Building target: \$@'" >> $MAKEFILENAME
echo -e "\t \$(CC) -c \$(DEFINES) -I../../include $PROJECTNAME.c" >> $MAKEFILENAME
echo -e "\t \$(CC) -c \$(DEFINES) -I../../include  ../../src/transport.c" >> $MAKEFILENAME
echo -e "\t \$(CC) -c \$(DEFINES) -I../../include  ../../src/wirebus.c" >> $MAKEFILENAME
echo -e "\t \$(CC) -c \$(DEFINES) -I../../include  ../../src/firmware.c" >> $MAKEFILENAME
echo -e "\t \$(CC) \$(DEFINES)   -o ../../hex/$PROJECTNAME.ihx" >> $MAKEFILENAME
echo -e "\t \$(CC) -Os \$(DEFINES) -I../../include \$(LIBS) $TRANSPORT -o $PROJECTNAME.hex" >> $MAKEFILENAME
echo "Creating Makefile OK"



if true; then
	echo "Creating firmware skeleton in $PROJECTNAME.c"
	cat <<- EOS > $WORKDIR/$PROJECTNAME.c
/*
Copyright (c) 2016 [YOUR NAME MUST BE HERE]
All rights reserved.

Licensed under MIT License (the "License");
you may not use this file except in compliance with the License.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

NOTE: This file created automaticly by create project script

*/

#include <stdint.h>
#include <wirebus.h>

/*

This  function called once on initialization 

*/

void setup()
{

}

/*

This  function called on each program cycle 
WARNING ! Don't use time greedy procedures here, otherwise program may lock
*/

void loop()
{

}

/*

Called when new command received 

*/

void on_command(wirebus_packet *p)
{

}


	
	EOS
fi

}

