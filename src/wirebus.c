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


uint8_t flags_byte;

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
	transportInit();	
	dev->addr = load_device_addr();
}

uint8_t wirebusSendMessage(  wirebus_device* device, uint8_t priority, uint8_t cmd,  uint8_t dst, wirebus_packet *p)
{
	uint8_t *ptr = buff;
	total_bytes = (( cmd >> 4 ) & 0x03)+3;

	uint8_t data_size = p->p.size;
	uint8_t *data_ptr = p->p.data;

	*ptr++ = ( priority << 6 ) | cmd;
	*ptr++ = device->addr;
	*ptr++ = dst;
	
	if(total_bytes == 3)
	{	
		*ptr++ = data_size;
		total_bytes += data_size;
	}

	for(;data_size--;)
		*ptr++ = *data_ptr++;


	*ptr++ = calc_crc(buff,total_bytes);

	DBG("Buffer created %d bytes",total_bytes);
	
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


void handle_receive(wirebus_device *device, wirebus_packet *p)
{

        uint8_t pos = device->bytes_cnt-1;
	uint8_t b   = device->data_byte;
	device->data_byte = 0;	

        switch(pos)
        {
                case 0:
                        total_bytes =  ( ( b >> 4 ) & 0x3 ) + 3;
                        break;
                case 2:
                       if(b!=0xFF && b!=device->addr ) //If not our address stop receiving 
                       //flags_byte |= (0x30);
		       break;
		case 3:
                     if(total_bytes > 5) total_bytes += b; //Size of packet data 
                      break;
        }

	if( device->bytes_cnt < total_bytes )
	{
		check_buffer(b,p);		
		return;
	}

	buff[pos] = b;
}


void handle_send(wirebus_device *device, wirebus_packet *p)
{
	DBG("Handle send");

	if(device->bytes_cnt < total_bytes)
	{
		DBG("Sending byte %d : 0x%X", device->bytes_cnt, buff[device->bytes_cnt] );
		device->data_byte = buff[device->bytes_cnt];
		device->bits_to_process = 8;
		device->bytes_cnt++;
	}
}


uint8_t wirebusProcess(wirebus_device *device, wirebus_packet *p)
{

	 uint8_t res = processDataTransfer(device);
	 if( res == ERROR_OK)
	 {
		uint8_t state = device->state;	
		switch(state)
		{
			case WIREBUS_STATE_SEND:
				handle_send(device,p);
				break;
			case WIREBUS_STATE_RECV:
				handle_receive(device,p);
                        	break;
                        //case WIREBUS_STATE_BREAK:
                        //	break;
		}	
	 }

#ifdef USE_SOFTWARE_TIMER
	wirebus_timer_handler();
#endif	

	return p->p.cmd;
}

