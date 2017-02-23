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

Disclaimer: 
	I know, the code looks ugly, almost asm, but it's good for smaller code-size optimization

*/

#include "config.h"
#include "platform.h"
#include "transport.h"
#include "wirebus.h"


//typedef void (*fsm_dispatch)();
uint8_t onByteReceived(uint8_t pos, uint8_t b);

#ifdef __AVR_ARCH__
register volatile uint8_t data_byte asm("r13");
register uint8_t flags_byte asm("r17");
register uint8_t bits_to_send asm("r15");
register uint8_t counter asm("r2");
#else
static volatile uint8_t data_byte;
static volatile uint8_t bits_to_send;
volatile uint8_t flags_byte;
#endif



void inline transport_init()
{
        SETUP_WIREPIN;
	START_TIMER;
        START_RECEIVER;
}


/****************************************************
*
* This two functions should be called pereodically 
* We have queue size of one byte only. So program should empty it fast enough  
*
*****************************************************/

uint8_t inline isTransportReady()
{
        return flags_byte & WIREBIS_STATE_COMPLETE;
}

uint8_t inline readByte()
{
        flags_byte &= ~(WIREBIS_STATE_COMPLETE);
        uint8_t ret = data_byte;
        data_byte = 0x01;
        return ret;
}


/*******************************************
*
*  Sends 5 marks + 1 space to occupy line 
*
*******************************************/
uint8_t inline sendStart()
{
        if( flags_byte != WIREBIS_STATE_IDLE )
                                        return ERROR_TRANSMIT_ABORT;

	//Take over the line
 	flags_byte |= WIREBIS_STATE_SEND;
	RESET_TIMER(0);	
	flags_byte &= ~DATA_TICK; //First tick always sync	

	SET_MARK(WIREBUS_TX_PIN);
	while( (flags_byte & WIREBIS_STATE_BREAK|FLAG_RESET) == 0 ){};	
	flags_byte &= ~(WIREBIS_STATE_BREAK|FLAG_RESET);
	SET_SPACE(WIREBUS_TX_PIN);
	return ERROR_OK;
}

/******************************************
*
*  Sets data to send by timer 
*
******************************************/
uint8_t sendByte(uint8_t byte)
{	
	data_byte 	= byte;
	bits_to_send 	= 8;
        return ERROR_OK;
}


/*************************************************************************
 * 
 *  Pin change vector . Size 36 bytes
 *
 * **********************************************************************/

ISR( PINCHANGE_VECTOR )
{

	uint8_t local_flags = flags_byte;	

	//Update line state flag	
	if( READ_WIRE_STATE == PIN_STATE_MARK )
			local_flags |= FLAG_MARK;
	else
			local_flags &= ~FLAG_MARK;
		
	//Don't care line level changes while sending
	if( local_flags & ( WIREBIS_STATE_SEND )  ) 
		goto exit_pinchange_isr;

	//Receiver function: 
	//On falling edge set timer to half of bit period
	//to fire timer interrupt in center of each bit
		
	if( local_flags & FLAG_MARK )
	{
		RESET_TIMER(WIREBUS_BIT_DELAY/2); //Fire timer on center of bit delay
        	flags_byte &= ~DATA_TICK; //First tick always sync
		goto exit_pinchange_isr;
	}

	if( local_flags & WIREBIS_STATE_RECV  )
	{			

		//Check last bit in sending sequence;  		
		if( local_flags & 0x80 )
			local_flags |= WIREBIS_STATE_COMPLETE;

		data_byte << 1;
		if( local_flags & BIT_ONE ) 
			data_byte &= 0x01;

	}

exit_pinchange_isr:
	local_flags |= ~BIT_ONE;   //Reset counter flag on every level change
	flags_byte = local_flags; 
}


/*******************************************************************
 *
 *
 *     Each bit has 2 or 4 timer periods.
 *    
 *     Receive : Timer fired on center of each bit.
 *     Each even bit is SYNC_TICK. Each odd bit is DATA_TICK
 *     - 1 2_
 *     0|_|     - 1 - Sync, 2 is data bit.  2 is SPACE - zero received
 *     _ 123 _  
 *     0|___|   - 1,3,Sync, 2 is data bit.  2 is MARK - one received 
 *
 *	Receive : Timer fired on center of each bit.
 *		  The sequence is 
 *
 *
 *     Size : 78 bytes (From 680bytes) 
 *
 ********************************************************************/
 
ISR( TIMER_VECTOR )
{

	uint8_t local_flags = flags_byte;

	if( local_flags & DATA_TICK )
	{

	     if( local_flags & BIT_ONE )
	     {
                //5-th or more data tick in series of the same level
		local_flags |= WIREBIS_STATE_BREAK;
		if(local_flags & FLAG_MARK)
		{
			//If line is in MARK state RESET
			local_flags|=FLAG_RESET;
		}else{
			//Reset state machine on external break pulse
			local_flags &= ~(WIREBIS_STATE_RECV|WIREBIS_STATE_SEND);
		}

		SET_SPACE(WIREBUS_TX_PIN); //Always(!) release line  on break

		goto exit_isr;
	     }

	     local_flags &= BIT_ONE;
	}

	if(bits_to_send)
	{

		if( (local_flags & DATA_TICK ) == 0 )
		{
			//When sending sync tick is always mark
			SET_MARK();
			goto exit_isr;
		}

		if(data_byte & 0x80)
		{
			//If sending bit is one, continue holdin line in space state
			//And reset sending bit to zero, so next data tick will release the line
			data_byte &= ~(0x80);
			goto exit_isr;
		}
		
		bits_to_send--;
                
		if( !bits_to_send )
                          local_flags |= WIREBIS_STATE_COMPLETE;	
	
		//Release line and arbitrate 
		SET_SPACE(WIREBUS_TX_PIN);
        	NOP();
        	if( READ_WIRE_STATE == PIN_STATE_MARK ) //Some one else hold line down, Collision detected
                        local_flags |= WIREBIS_STATE_BREAK;

	}

exit_isr:
	local_flags ^= DATA_TICK;	
	flags_byte = local_flags;
}

