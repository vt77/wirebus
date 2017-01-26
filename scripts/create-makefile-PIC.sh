

function create_makefile
{

echo "Creating Makefile for $PROJECTNAME $MCU $ARCH in $WORKDIR"
MAKEFILENAME="$WORKDIR/Makefile";
PROJECTLIBS='../../src/transport.c ../../src/firmware.c ../../src/wirebus.c';
DEFMCU="__MCU_${MCU}__"

echo "CC=picc" > $MAKEFILENAME
echo "DEFINES=-D${DEFMCU} -D__ARCH_PIC__" >> $MAKEFILENAME
echo "LIBS=$PROJECTLIBS" >> $MAKEFILENAME
echo ""
echo ""
echo 'all: wirebus' >> $MAKEFILENAME
echo -e "\nwirebus: $PROJECTNAME.c" >> $MAKEFILENAME
echo -e "\t@echo 'Building target: \$@'" >> $MAKEFILENAME
echo -e "\t \$(CC) --chip=${MCU} \$(DEFINES) -I../../include -I./ $PROJECTNAME.c \$(LIBS)" >> $MAKEFILENAME
echo -e "\t cp $PROJECTNAME.hex ../../hex/$PROJECTNAME.hex" >> $MAKEFILENAME
echo "Creating Makefile OK"

cat <<-EOC > $WORKDIR/config.h
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

#ifndef __WIREBUS_CONFIG_H__
#define __WIREBUS_CONFIG_H__

#define WIREBUS_RX_PIN	B,1 
#define WIREBUS_TX_PIN	B,0

/*
 Define UUIDs of your device 
*/
//#define WIREBUS_UUID_MAJOR YOUR_DEVICE_MAJOR_TAG_HERE 
//#define WIREBUS_UUID_MINOR YOUR_DEVICE_MINOR_TAG_HERE

//Uncomment followed line to activate timer
//#define ON_TIMER_FUNC   on_timer

#endif
EOC


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
	extern uint8_t  volatile cmd;
	extern wirebus_packet  packet;
	//wirebus_packet : 
	//   pkt_hdr         hdr;
	//   uint8_t         src;
	//   uint8_t         dst;
	//   uint8_t         size;
	//   uint8_t         *data;	
	if( cmd != WIREBUS_CMD_NONE )
	{
		//Process new packet



	}
}

/*
   Uncomment timer definition in config.h to receive 0.5 ms timer events
   Params: 
	cntr - count of ticks after timer reset . Totally useless
*/
void on_timer(uint8_t cntr)
{
	
}

	
	EOS
fi

}

