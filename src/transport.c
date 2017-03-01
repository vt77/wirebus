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

volatile uint8_t flags_byte;
 

#define         FLAG_IDLE       0x00
#define         FLAG_BIT        0x01
#define         FLAG_ONE        0x02
#define         FLAG_RESET      0x04

#define         FLAG_SEND       0x10
#define         FLAG_RECV       0x20
#define         FLAG_BREAK      0x30 // (FLAG_SEND|FLAG_RECV)

#define         FLAG_COMPLETE   0x40
#define         FLAG_SKIP       0x80


#define SET_FLAG(a)    local_flags |= (a)
#define CLEAR_FLAG(a)  local_flags &= ~(a)
#define CHECK_FLAG(a)  (local_flags & (a))
#define TOGGLE_FLAG(a) (local_flags ^= a)

#define GET_STATE()    (( local_flags >> 4 ) & 0x03)

#ifdef USE_SOFTWARE_TIMER
volatile uint8_t timer_now;
#endif


void INLINE transportInit()
{
        SETUP_WIREPIN;
	START_TIMER;
        START_RECEIVER();
}


void INLINE releaseLine()
{
	START_RECEIVER();
	flags_byte = 0;
}


uint8_t processDataTransfer(wirebus_device *device)
{
	//
	// Please ensure we're in send/receive state before calling this function
	//

	uint8_t local_flags = flags_byte;
	device->state = GET_STATE();
	
	//Process take place on line release only
	if( CHECK_FLAG( FLAG_COMPLETE ) == 0 )
                                return ERROR_BUSY;
	
	//Clear all dynamic flags
	CLEAR_FLAG(FLAG_COMPLETE|FLAG_ONE|FLAG_BIT|FLAG_SKIP);
	
	//Process flags
	if(CHECK_FLAG( FLAG_RESET ))
	{

		//After reset
		device->bytes_cnt = 0;
		device->bits_to_process = 8;
		CLEAR_FLAG(FLAG_RESET);

	}else{ 
	
	  uint8_t bitmask = (1<<device->bits_to_process);
	
	  if( device->data_byte & bitmask ) //This will no effect when receiving
			SET_FLAG(FLAG_ONE);
	  
	  if(CHECK_FLAG( FLAG_ONE ))	//This line will no effect when sending
			device->data_byte |= bitmask;
	}

	flags_byte = local_flags;
	
	if( device->bits_to_process-- != 0)
                        	return ERROR_BUSY;

	device->bits_to_process = 8; 
	device->bytes_cnt++;

	return ERROR_OK;
}

/*******************************************
*
*  Sends 5 marks + 1 space to occupy line 
*
*******************************************/
uint8_t INLINE sendStart()
{
	uint8_t local_flags = flags_byte;

        if(  flags_byte & (FLAG_SEND|FLAG_RECV) )
                         return ERROR_TRANSMIT_ABORT;

	STOP_RECEIVER();		//Take over the line
	RESET_TIMER(WIREBUS_BIT_DELAY);	//Timer on BIT_DELAY
	SET_FLAG(FLAG_SEND|FLAG_SKIP);		//Mark, we're sending and skip till reset 

	//Now wait for line released by timer
	//When line released FLAG_COMPLETE will be set

	flags_byte = local_flags;

	return ERROR_OK;
}


/*************************************************************************
 * 
 *  Pin change vector . Fired on falling edge. Disabled while sending
 *
 * **********************************************************************/

ISR( PINCHANGE_VECTOR )
{
	//uint8_t local_flags = flags_byte;

	//We're not fast enough to fetch data duting one timer tick
	//if((flags_byte & (FLAG_RECV|FLAG_COMPLETE)) == (FLAG_RECV|FLAG_COMPLETE) )
	//		flags_byte |= FLAG_BREAK;

	//Set timer to fire in center of tick period
	RESET_TIMER(WIREBUS_BIT_DELAY/2);
	flags_byte |= FLAG_RECV;
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
 ********************************************************************/

/*
 *
 *	
 *
 *
 *
 *
 */

ISR( TIMER_VECTOR )
{
	uint8_t local_flags = flags_byte;

        if(CHECK_FLAG(FLAG_SEND|FLAG_BIT|FLAG_RECV) == (FLAG_SEND))
        {
                //Sync period while sending.
                //Hold line down
                 SET_MARK();
        }

	if( ( CHECK_FLAG(FLAG_SEND|FLAG_BIT|FLAG_ONE|FLAG_SKIP) == (FLAG_SEND|FLAG_BIT|FLAG_ONE) ) || /* Release line when FLAG_ON set on DATA tick and not SKIP present */
		 CHECK_FLAG(FLAG_RESET) /* Always release line on reset */)
	{
			SET_SPACE(); 
                        //Wait pin change
			NOP();
                        NOP();
                        //Arbitrate
                        if( READ_WIRE_STATE == PIN_STATE_MARK )
                                SET_FLAG(FLAG_BREAK);
	}
	
	local_flags++; //This will cout 0-4 to toggle flags TICK_BIT(0x01),TICK_ONE(0x02) and TICK_RESET(0x04) continuously
		       //If TICK_ONE(2) was set by send procedure count will be 2-3 and sending zero
		       //3 Is usually last tick and WIRE_STATE should be switched to MARK
		       //Except of sending/receiving RESET signal 

	if( READ_WIRE_STATE != PIN_STATE_MARK )
			 SET_FLAG(FLAG_COMPLETE);

	flags_byte = local_flags;


#ifdef USE_SOFTWARE_TIMER
		timer_now++;
#endif
}


#if __ARCH_PIC__

void interrupt inter(void)
{
   GIE = 0; // CLI
 
   if ((T0IF)&&(T0IE))
   {
		isr_timer_ovf();
		T0IF = 0;
   }
   if (INTCON&0b00000001)
   {
		isr_pin_change();
	    //TCON&=0b11111110;		//Clearing the interrupt flag RBIF
   }
	INTCON |= 0b10001000;	//Re enabling the interrupt
}

#endif


