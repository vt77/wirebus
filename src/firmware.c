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

#ifndef WIREBUS_UUID_MAJOR
#error "Please define WIREBUS_UUID_MAJOR and WIREBUS_UUID_MINOR codes of your device"
#endif
#ifndef WIREBUS_UUID_MINOR
#error "Please define WIREBUS_UUID_MAJOR and WIREBUS_UUID_MINOR codes of your device"
#endif

wirebus_packet  packet;
wirebus_device  device;

extern void setup();
extern void loop(uint8_t cmd);

int main()
{
	uint8_t	   pong_count  =   0;

	wirebusInit( &device );
	setup();

	ENABLE_INTERRUPTS();

	wirebusSendMessage(WIREBUS_PRIORITY_INFO,WIREBUS_CMD_INIT,0xFF,&packet);

	while(1){

		uint8_t cmd = wirebusProcess(&packet);
		uint8_t * data_ptr  = &packet.p.size;
		uint8_t src 	    =  packet.p.src;

		switch( cmd )
		{
			case 	WIREBUS_CMD_PING:
								*data_ptr++ = 1;	
								*data_ptr++ = pong_count++; 
								wirebusSendMessage(WIREBUS_PRIORITY_INFO,WIREBUS_MESSAGE_PONG,src,&packet);
								break;
			case 	WIREBUS_CMD_REBOOT:
								while(1); //Just halt and let wachdog do the job;
			case 	WIREBUS_MESSAGE_SETADDR:
								save_device_addr(packet.p.data[0]);
								break;
			case	WIREBUS_CMD_GETDEVICEINFO:
								*data_ptr++ = 2;
								*data_ptr++ = WIREBUS_UUID_MAJOR;
								*data_ptr++ = WIREBUS_UUID_MINOR;
								wirebusSendMessage(  WIREBUS_PRIORITY_MESSAGE, WIREBUS_DATA_DEVICEINFO,src, &packet );
								break;
			default:
								loop( cmd );
								break;
		}

	}

}

