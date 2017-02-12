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

extern uint8_t sendByte(uint8_t byte);
extern uint8_t sendStart();
extern void releaseLine();
extern void transport_init();

extern wirebus_packet  		packet;
extern wirebus_device 		device;    

volatile uint8_t crc 		 = 0;
volatile uint8_t data_readed = 0;


void wirebus_init()
{
	transport_init();
}


/*

Sends packet without data size byte.
i.e WIREBUS_CMD orWIREBUS_INFO

*/

uint8_t wirebusSendCommand( uint8_t priority, uint8_t cmd , uint8_t dst, uint8_t data )
{

	uint8_t header;

	if( sendStart() == ERROR_TRANSMIT_ABORT )
				return ERROR_TRANSMIT_ABORT;

	header = ( priority << 6 ) | cmd;

	CRC(crc, header);
	ASSERT( sendByte( header ) );
	
	CRC( crc, device.addr );
	ASSERT( sendByte( device.addr ) );

	CRC(crc,dst);
	ASSERT( sendByte( dst ) );

	if( cmd & WIREBUS_INFO_BASE )
	{
		CRC(crc,data);
		ASSERT( sendByte( data ) );
	}
	
	ASSERT( sendByte( crc ));

	releaseLine();
	return ERROR_OK;

error:
	releaseLine();
	return ERROR_TRANSMIT_ABORT;
}

/*

Sends packet with data size byte.
i.e WIREBUS_MESSAGE

*/

uint8_t wirebusSendMessage(  uint8_t priority,  uint8_t cmd ,  uint8_t dst,  uint8_t size, const uint8_t *data )
{
	uint8_t tmp;

	if( sendStart() == ERROR_TRANSMIT_ABORT )
				return ERROR_TRANSMIT_ABORT;

	tmp =  ( priority << 6 ) + cmd;

	CRC(crc, tmp);
	ASSERT( sendByte( tmp ) );

	CRC(crc,device.addr);
	ASSERT( sendByte( device.addr ) );

	CRC(crc,dst);
	ASSERT( sendByte( dst ) );

	CRC(crc,size);
	ASSERT( sendByte(size));	

	for(tmp=0;tmp<size;tmp++)
	{
			CRC(crc,data[tmp]);
			ASSERT(sendByte( data[tmp] ));
	}

	return sendByte( crc );

error:
	return ERROR_TRANSMIT_ABORT;

}

uint8_t  start_receive( uint8_t b )
{
	packet.size = ( b & 0b0110000 ) >> 4;
	//packet.hdr.b = b;
	packet.hdr.priority = PACKET_PRIORITY(b);
	packet.hdr.cmd      = PACKET_COMMAND(b);

	return WIREBUS_DEVICE_STATUS_READ_SRC_ADDR;	
}

uint8_t  set_src_addr( uint8_t b )
{
	packet.src = b;
	return WIREBUS_DEVICE_STATUS_READ_DST_ADDR;	
}

uint8_t  set_dst_addr( uint8_t b )
{
		packet.dst = b;

		if( b != device.addr && b != 0xFF )
				return WIREBUS_DEVICE_STATUS_READY; //Skip packet

		if( packet.size == 1 )
				return WIREBUS_DEVICE_STATUS_READ_DATA;

		if( packet.size > 1 )
				return WIREBUS_DEVICE_STATUS_READ_DATA_SIZE;
		
		return WIREBUS_DEVICE_STATUS_READ_CRC;	
}

uint8_t  set_pkt_size(  uint8_t b )
{
		packet.size = b;	
		return WIREBUS_DEVICE_STATUS_READ_DATA;
}


uint8_t  set_pkt_data( uint8_t b )
{
	//TODO WARNING ! Size of array not checked
	//Buffer overflow possible 
	packet.data[data_readed++] = b;
	if( data_readed == packet.size) 
			return WIREBUS_DEVICE_STATUS_READ_CRC;	

	return WIREBUS_DEVICE_STATUS_READ_DATA;
}

uint8_t check_crc ( uint8_t b )
{
	if(crc == b)
		if(device.func) device.func();
	return WIREBUS_DEVICE_STATUS_READY;
}

#define BEGIN_STATE_MACHINE(a)   switch( a ){
#define END_STATE_MACHINE		 }	
#define BRANCH(a,b)		case a: device.state = b; break;

uint8_t onByteReceived(uint8_t pos, uint8_t b)
{

	if(0 == pos ) 
		device.state =  WIREBUS_DEVICE_STATUS_READ_START;

	if( device.state != WIREBUS_DEVICE_STATUS_READ_CRC)
		CRC(crc,b);

	BEGIN_STATE_MACHINE(device.state)
		BRANCH(	WIREBUS_DEVICE_STATUS_READ_START,  	   start_receive (b) )
		BRANCH(	WIREBUS_DEVICE_STATUS_READ_SRC_ADDR,   set_src_addr  (b) )
		BRANCH(	WIREBUS_DEVICE_STATUS_READ_DST_ADDR,   set_dst_addr  (b) )
		BRANCH(	WIREBUS_DEVICE_STATUS_READ_DATA_SIZE , set_pkt_size  (b) )
		BRANCH(	WIREBUS_DEVICE_STATUS_READ_DATA ,      set_pkt_data  (b) )
		BRANCH(	WIREBUS_DEVICE_STATUS_READ_CRC ,       check_crc     (b) )
	END_STATE_MACHINE

	if( device.state == WIREBUS_DEVICE_STATUS_READY) 
						return 0xFF;  //Complete
	return ERROR_OK;
}

