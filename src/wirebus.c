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
#include <wirebus.h>
#include <platform.h>
#include <transport.h>

extern volatile uint8_t flags_byte;
uint8_t device_addr;

static uint8_t buff[WIREBUS_MAX_DATA+5];

static uint8_t pos=0;
static uint8_t total_bytes=0;

#ifdef USE_SOFTWARE_TIMER
extern void wirebus_timer_handler();
#endif


#define CRC(a,b)  a^=b
#define         EXTRACT_PRIORITY(a)     ( a >> 6 )
#define         EXTRACT_COMMAND(a)   ( a & 0x3F)


/************************************
 *
 *  Exported functions
 *
 * *********************************/

uint8_t calc_crc(uint8_t *ptr,uint8_t size)
{
	uint8_t i;
	uint8_t crc_calculated = 0;
	
	for(;size--;)
	{
		crc_calculated = CRC(crc_calculated,*ptr++);
	}

	return crc_calculated;
}

void INLINE wirebusInit(wirebus_device * dev)
{ 
	transport_init();	
	device_addr = load_device_addr();
}

uint8_t wirebusSendMessage(  uint8_t priority, uint8_t cmd,  uint8_t dst, wirebus_packet *p)
{
	uint8_t *ptr = buff;
	total_bytes = (( cmd >> 4 ) & 0x03);

	uint8_t data_size = p->p.size;
	uint8_t *data_ptr = p->p.data;

	*ptr++ = ( priority << 6 ) | cmd;
	*ptr++ = device_addr;
	*ptr++ = dst;
	
	if(total_bytes == 3)
	{	
		*ptr++ = data_size;
		total_bytes += data_size;
	}

	for(;data_size--;)
		*ptr++ = *data_ptr++;


	*ptr++ = calc_crc(buff,total_bytes);
	
	return sendStart();
}

/*************************************
 *
 *  Checks received buffer for valid packet
 *
 * ***********************************/

uint8_t check_buffer(uint8_t crc,  wirebus_packet *p)
{
	uint8_t crc_calculated = 0;

	if( calc_crc(buff,total_bytes) == crc)
	{
		p->p.cmd = EXTRACT_COMMAND(buff[0]);
		return 1;
	}

	p->p.cmd = WIREBUS_CMD_NONE;
	
	return 0;
}

uint8_t wirebusProcess(wirebus_packet *p)
{
	uint8_t local_flags = flags_byte;
	uint8_t *ptr = buff + pos;

	if(local_flags & FLAG_BREAK )
	{
		//Process break state
		pos = total_bytes = 0;	
	}
	else if( local_flags & FLAG_SEND )
	{
		if( sendByte(*ptr) == ERROR_OK )
		{
			if( pos < total_bytes )
				sendByte(*ptr+1);
		}
	}
	else if( local_flags & FLAG_RECV )
	{		
		uint8_t b;
		if(readByte(&b) == ERROR_OK)
		{	
			//TODO: This pice of code takes 130 bytes. Too big to be good	
			
			switch(pos)
			{
				case 0:
				 	total_bytes =  ( ( b >> 4 ) & 0x3 ) + 3;	
					break;
                        	case 2:
                                	if(b!=0xFF && b!=device_addr ) //If not our address stop receiving 
                                        		flags_byte |= (FLAG_BREAK);
					break;
				case 3:
                                	if(total_bytes > 5) total_bytes += b; //Size of packet data 
                                		break;
			}
			//68 bytes till this line 	
			if( pos == total_bytes )
			{
				check_buffer(b,p);
				return p->p.cmd;
			}

			//Next line 34 bytes
			*ptr = b; pos++;
			//END OF TODO
		}
		
	}

#ifdef USE_SOFTWARE_TIMER
	wirebus_timer_handler();
#endif
	return WIREBUS_CMD_NONE;	
}

