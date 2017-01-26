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

#include "wirebus.h"
#include "config.h"

#ifdef __ARCH_AVR__
#include <avr/wdt.h>
#include <avr/eeprom.h>

#define     save_device_addr(a)   eeprom_write_byte(0x0,a)
#define     load_device_addr()    eeprom_read_byte(0x0)

#define ENABLE_INTERRUPTS() 		sei()  

#endif

#ifdef __ARCH_PIC__
#include <htc.h>

#define     save_device_addr(a)   eeprom_write(0x0,a)
#define     load_device_addr()    eeprom_read(0x0)

#define ENABLE_INTERRUPTS() 		GIE=1  

#endif


#ifndef WIREBUS_UUID_MAJOR
#error "Please define WIREBUS_UUID_MAJOR and WIREBUS_UUID_MINOR codes of your device"
#endif
#ifndef WIREBUS_UUID_MINOR
#error "Please define WIREBUS_UUID_MAJOR and WIREBUS_UUID_MINOR codes of your device"
#endif


wirebus_packet  packet;
wirebus_device 	device;

uint8_t  volatile cmd;
extern void setup();
extern void loop();

uint8_t    data_buffer[WIREBUS_MAX_DATA];

wirebus_packet *wirebus_check_new_data()
{
	if( cmd != WIREBUS_CMD_NONE )
	{
		cmd = WIREBUS_CMD_NONE;
		return &packet;
	}
	return 0;
}

void on_new_data()
{
	cmd = packet.hdr.cmd;
}


int main()
{

	uint8_t	   pong_count  =   0;
	device.addr = load_device_addr();
	device.func = on_new_data;
	packet.data = data_buffer;

	wirebus_init();

	setup();

	ENABLE_INTERRUPTS();

	wirebusSendCommand(WIREBUS_PRIORITY_INFO,WIREBUS_CMD_REBOOT,packet.src,0);


	while(1){
	  
	  if( cmd != WIREBUS_CMD_NONE )
	  {
			switch( cmd )
			{
				case 	WIREBUS_CMD_PING:
									cmd = WIREBUS_CMD_NONE;
									wirebusSendCommand(WIREBUS_PRIORITY_INFO,WIREBUS_INFO_PONG,packet.src,pong_count++);
									break;
				case 	WIREBUS_CMD_REBOOT:
									cmd = WIREBUS_CMD_NONE;
									while(1); //Just halt and let wachdog do the job;
				case 	WIREBUS_DATA_SETADDR:
									cmd = WIREBUS_CMD_NONE;
									save_device_addr(data_buffer[0]);
									break;
				case	WIREBUS_CMD_GETDEVICEINFO:
									cmd = WIREBUS_CMD_NONE;	
									data_buffer[0] = WIREBUS_UUID_MAJOR;
									data_buffer[1] = WIREBUS_UUID_MINOR;
									wirebusSendMessage(  WIREBUS_PRIORITY_MESSAGE,  WIREBUS_MESSAGE_DEVICEINFO , packet.src,  2, data_buffer );
									break;
			}
	  }

	  loop();
	}
}
