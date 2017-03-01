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


#include "config.h"
#include "wirebus.h"
#include "platform.h"
#include <avr/eeprom.h>


//Define device address and save it in EEPROM
uint8_t myAddress EEMEM = 0x13;

wirebus_packet  packet;
wirebus_device  device;


//Device settings.
//Stored in EEPROM from address 0x01
//0x01 - First target switch command 
//0x02 - First target switch address
//0x03 - Second target switch command
//0x03 - Second target switch address
//
// First data byte format : 
// NXXXXXXX  
// 	      N - number of target (0 or 1)
//            XXXXXXX - Command to send when button pressed
void save_target()
{
   uint8_t pos = (packet.p.data[0] >> 7) << 1;
   eeprom_write_byte((uint8_t *)pos+1,packet.p.data[0] & 0x7F);	
   eeprom_write_byte((uint8_t *)pos+2,packet.p.data[1]);
}


uint8_t check_buttons_state()
{
	return packet.p.dst;	
}

int main()
{
	//uint8_t	   pong_count  =   0;

	wirebusInit( &device );	

	ENABLE_INTERRUPTS();

//	wirebusSendMessage(&device, WIREBUS_PRIORITY_INFO,WIREBUS_CMD_INIT,0xFF,&packet);

	while(1){

		/* 
 		*  Due to really small memory size 
 		*  only limited commands set supported by this device
 		*
 		*  WIREBUS_MESSAGE_SETADDR is disabled.
 		*  You can change device address manualy writing it to EEPROM at offset 0
 		*
 		*  Ping command is disabled too.
 		*  You can use WIREBUS_CMD_GETDEVICEINFO to check device alive 	
 		*
 		*/

		uint8_t cmd = wirebusProcess(&device, &packet);
		uint8_t dst 	    =  packet.p.src;

		switch( cmd )
		{
			//case 	WIREBUS_CMD_PING:
			//					packet.p.size = 1;	
			//					packet.p.data[0] = pong_count++; 
			//					wirebusSendMessage(&device,WIREBUS_PRIORITY_INFO,WIREBUS_MESSAGE_PONG,dst,&packet);
			//					break;
			case 	WIREBUS_CMD_REBOOT:
								while(1); //Just halt and let wachdog do the job;
			//case 	WIREBUS_MESSAGE_SETADDR:
			//					save_device_addr(packet.p.data[0]);
			//					break;
			case	WIREBUS_CMD_GETDEVICEINFO:
								packet.p.size = 2;
								packet.p.data[0] = WIREBUS_UUID_MAJOR;
								packet.p.data[1] = WIREBUS_UUID_MINOR;
								wirebusSendMessage(&device,WIREBUS_PRIORITY_MESSAGE, WIREBUS_DATA_DEVICEINFO,dst, &packet );
								break;
			case WIREBUS_DATA_SET:
								save_target();
								break;
		}

		
		if( dst = check_buttons_state() )
		{
			wirebusSendMessage(&device,WIREBUS_PRIORITY_MESSAGE, packet.p.cmd ,dst, &packet );
		}
	}
}

