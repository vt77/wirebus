/*
Copyright (c) 2016 Daniel Marchasin
All rights reserved.

Licensed under MIT License (the "License");
you may not use this file except in compliance with the License.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include <stdint.h>
#include "config.h"
#include <platform.h>
#include <wirebus.h>


wirebus_packet  packet;
wirebus_device  device;

uint8_t buttons_state;

int main()
{

	wirebusInit( &device );	

	ENABLE_INTERRUPTS();

	wirebusSendMessage(WIREBUS_PRIORITY_INFO,WIREBUS_CMD_INIT,0xFF,&packet);

	while(1){

		uint8_t cmd = wirebusProcess(&packet);
		uint8_t * data_ptr  = &packet.p.size;
		uint8_t src 	    =  packet.p.src;

		/* 
 		*  Due to really small memory size 
 		*  only limited commands set supported 
 		*  by this device
 		*
 		*  WIREBUS_MESSAGE_SETADDR is disabled.
 		*  You can change device address manualy writing it to EEPROM at offset 0 	
 		*
 		*/

		switch( cmd )
		{		
			case 	WIREBUS_CMD_REBOOT:
								while(1); //Just halt and let wachdog do the job;
			//case 	WIREBUS_MESSAGE_SETADDR:
			//					save_device_addr(packet.p.data[0]);
			//					break;
			case	WIREBUS_CMD_GETDEVICEINFO:
								*data_ptr++ = 2;
								*data_ptr++ = WIREBUS_UUID_MAJOR;
								*data_ptr++ = WIREBUS_UUID_MINOR;
								wirebusSendMessage(  WIREBUS_PRIORITY_MESSAGE, WIREBUS_DATA_DEVICEINFO,src, &packet );
								break;
		}

		if( buttons_state )
		{
			wirebusSendMessage(WIREBUS_PRIORITY_INFO,WIREBUS_CMD_INIT,0xFF,&packet);
		}

	}

}

ISR(PCINT0_vect)
{
	buttons_state = PORTB;
}

