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

#ifdef __AVR_ARCH__
register uint8_t flags_byte asm("r17");
register uint8_t device_addr asm("r5");
#else
extern volatile uint8_t flags_byte;
uint8_t device_addr;
#endif

static uint8_t pos=0;
static uint8_t total_bytes=0;
static uint8_t crc;
static uint8_t new_command = WIREBUS_CMD_NONE;


#ifdef USE_SOFTWARE_TIMER
extern wirebus_timer_handler();
#endif



#define CRC(a,b)  a^=b
#define         EXTRACT_PRIORITY(a)     ( a >> 6 )
#define         EXTRACT_COMMAND(a)   ( a & 0x3F)

typedef void (*fsm_tran)(wirebus_packet *p);


/************************************
 *
 *  Exported functions
 *
 * *********************************/


void inline wirebusInit(wirebus_device * dev)
{ 
	transport_init();	
	device_addr = load_device_addr();
}

uint8_t wirebusSendMessage(  uint8_t priority, uint8_t cmd,  uint8_t dst, wirebus_packet *p)
{
	p->p.cmd = ( priority << 6 ) | cmd;
	//p->p.src = device->addr;
	p->p.src = device_addr;
	return sendStart();
}

//void wirebusProcess(wirebus_device *dev, wirebus_packet *p);


/************************************
 *
 *  Wirebus statemachine functions
 *
 * *********************************/


/********************************************
 *
 *  Called when no operation shceduled 
 *
 * ******************************************/
void sm_on_idle(wirebus_packet *p)
{
	//Nothing to do	
#ifdef USE_SOFTWARE_TIMER
	wirebus_timer_handler();
#endif
}

/*********************************************
 *
 *  1. Called pereodiaclly each data cicle when line is in idle state
 *  2. Called when reset detected from remote device. FLAG_RESET is set in this case
 *
 ********************************************/

void sm_on_reset(wirebus_packet *p)
{
	if(flags_byte & FLAG_RESET )	
		flags_byte != WIREBIS_STATE_RECV;
	
	flags_byte &= ~(WIREBIS_STATE_BREAK);
}

/**************************************************
 *
 *  Called when we're in receiving state
 *  Bytes sequence
 *  0 - Cmd + Proirity
 *  1 - Source address
 *  2 - Destination address
 *  3 - CRC on COMMAND, data on INFO packet or size of data on MESSAGE
 *  4 - CRC on INFO, or data for MESSAGE
 *  .....  DATA 
 *  n+1 CRC on DATA
 *
 *  Note:
 *	This function not indicates errors while receiving. Bad packets just dropped.
 *	May be not too good behaviour for debugging reason
 *
 * ***********************************************/

void sm_on_receive(wirebus_packet *p)
{
	if( isTransportReady() )
	{
		uint8_t b = readByte();	
		switch(pos)
		{
			case 0:
		                crc = 0;
				total_bytes =  ( ( b >> 4 ) & 0x3 ) + 3;  // 3 for command, 4-for info 6 for message
				break;
			case 2:
				if(b!=0xFF && b!=device_addr ) //If not our address stop receiving 
					flags_byte |= (WIREBIS_STATE_BREAK); 
				break;
			case 3:
				if(total_bytes > 5) total_bytes += b; //Size of data 
				break;
		}

		if(pos != total_bytes)
		{
			CRC(crc,b);
			p->b[pos++] = b;
			flags_byte &= ~(WIREBIS_STATE_COMPLETE);
			return;
		}	
		else{
			//check crc;
			new_command = p->b[0];
			flags_byte |= (WIREBIS_STATE_BREAK);	
		}
	}
}


void sm_on_send(wirebus_packet *p)
{
        if( isTransportReady() )
        {
                //Send next byte        
                if(pos == 0)
                        crc = 0;
                
		if( pos < total_bytes )
                {
                        uint8_t b = p->b[pos++];
                        CRC(crc,b);
                        sendByte(b);
                }else
                   flags_byte &= ~(WIREBIS_STATE_SEND);
        }
}

/***********************************
 *
 *  Break occures while operation.
 *  It may be number of reasons
 *  1. Arbitration while sending packet
 *  2. Receive packet not addressed to us.
 *  3. We're not fast enouph to receive packet. (should never happen) 
 *
 * *******************************/

void sm_on_break(wirebus_packet *p)
{
	//Error occures while processing data;
	//Reset all received states
	pos = 0;
	total_bytes = 0;
}

fsm_tran fsm[] = {
	sm_on_idle,		/* WIREBIS_STATE_IDLE   0x0  */
	sm_on_reset,		/* WIREBIS_STATE_BREAK  0x01 Fired each timer data tick when line idle (timer_period * 2) */
	sm_on_send,		/* WIREBIS_STATE_SEND   0x02 */
	sm_on_break,		/* WIREBIS_STATE_SEND | WIREBIS_STATE_BREAK 0x03 */
	sm_on_receive,          /* WIREBIS_STATE_RECV | 0x04 Fired each time we're receiving data */
        sm_on_break,    	/* WIREBIS_STATE_RECV | WIREBIS_STATE_BREAK 0x05 Break while receiving*/
};

uint8_t wirebusProcess(wirebus_packet *p)
{
	uint8_t ptr = flags_byte & 0x07;
	if(ptr < 6)
		fsm[ptr](p);

	ptr = new_command;
	new_command = WIREBUS_CMD_NONE;
	return ptr;	
}

