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

static volatile uint8_t data_byte;
static volatile uint8_t bits_to_process;
volatile uint8_t flags_byte;

#ifdef USE_SOFTWARE_TIMER
volatile uint8_t timer_now;
#endif


void INLINE transport_init()
{
        SETUP_WIREPIN;
	START_TIMER;
        START_RECEIVER();
}


void INLINE transport_reset()
{
	START_RECEIVER();
}

/****************************************************
*
* This function should be called pereodically in loop at least once per timer tick, when FLAG_RECV set 
* We have queue size of one byte only. So program should empty it fast enough 
*
* NOTE: We should not clear FLAG_RECV 
* 
* Exeaple to call:
*   uint8_t res = readByte(&b);
*   if( res == ERROR_TRANSMIT_ABORT )
*   {
*   	//Abort receiving 
*   }
*
*   if(readByte(&b) == ERROR_OK)
*   {
*   	//Process byte in b
*   }
*
*
*****************************************************/

uint8_t readByte(uint8_t* byte)
{

	uint8_t local_flags = flags_byte;

	if( (local_flags & FLAG_RECV ) == 0 )
				return ERROR_TRANSMIT_ABORT;

	if( ( local_flags & FLAG_DATA ) == 0 )
				return ERROR_BUSY;

	if( ( local_flags & FLAG_COMPLETE ) == 0 )
                               	return ERROR_BUSY;

	if(bits_to_process != 0)
				return ERROR_BUSY;

	*byte = data_byte;
	local_flags &= ~FLAG_COMPLETE;
	bits_to_process = 8;

	flags_byte = local_flags;
      
	return ERROR_OK;
}


/*******************************************
*
*  Sends 5 marks + 1 space to occupy line 
*
*******************************************/
uint8_t sendStart()
{

	uint8_t local_flags = flags_byte;

        if( local_flags & (FLAG_SEND|FLAG_RECV) )
                         return ERROR_TRANSMIT_ABORT;


	//Take over the line
	STOP_RECEIVER();
	RESET_TIMER(0);
	local_flags |= FLAG_SEND;	
	local_flags &= ~FLAG_BIT; //First tick is sync
	SET_MARK();

	flags_byte = local_flags;

	//Now wait for line released by timer
	//When line released FLAG_COMPLETE will be set

	return ERROR_OK;
}

/******************************************
*
*  Sets data to send by timer.Should be called pereodically from loop at least once per timer tick
*
*  Example to call : 
*  uint8_t res = sendByte(buffer[ptr]);
*  if( res == ERROR_TRANSMIT_ABORT) //Arbitation fails. Stop transmitting
*  if( res == ERROR_OK )
*  {
*  	//Send next byte in sequence if any
*  	sendByte(buffer[ptr]); 
*  }
*
******************************************/
uint8_t sendByte(uint8_t byte)
{

	uint8_t local_flags = flags_byte;	

	if( local_flags & FLAG_BREAK )
				return ERROR_TRANSMIT_ABORT; 

	//Still in progress
	if( ( local_flags & (FLAG_COMPLETE|FLAG_SEND) ) == (FLAG_SEND) )
					return ERROR_BUSY;

	//First bit in sequence
	if( ( local_flags & FLAG_DATA ) == 0 )
	{
		data_byte       = byte;
		bits_to_process    = 8;		
		local_flags = (FLAG_SEND|FLAG_DATA);
	}

	//Till all bits processed
	else if( --bits_to_process == 0 )
	{
		flags_byte  = FLAG_IDLE;
		return ERROR_OK;
	}

	flags_byte = local_flags;	

        return ERROR_BUSY;
}


/*************************************************************************
 * 
 *  Pin change vector . Fired on falling edge. Disabled while sending
 *
 * **********************************************************************/

ISR( PINCHANGE_VECTOR )
{
	uint8_t local_flags = flags_byte;

	//We're not fast enough to fetch data duting one timer tick
	//if((flags_byte & (FLAG_RECV|FLAG_COMPLETE)) == (FLAG_RECV|FLAG_COMPLETE) )
	//		flags_byte |= FLAG_BREAK;


	//Set timer to fire in center of tick period
	RESET_TIMER(WIREBUS_BIT_DELAY/2);
	local_flags &= ~(FLAG_RESET|FLAG_ONE);
	local_flags |= (FLAG_RECV);

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
	
	switch(local_flags)
	{
		//Flags When FLAG_SEND is set, 16 and up + 7 
                //case (FLAG_SEND|FLAG_RESET|FLAG_BIT):                     //0x15          
                     //We sent reset
                //     local_flags |= FLAG_COMPLETE;
                //     break;
 
		case (FLAG_SEND|FLAG_DATA):				//0x50  First sync tick when sending. Hold line down
				SET_MARK();				
				break;

		case (FLAG_SEND|FLAG_DATA|FLAG_BIT):			//0x51
				if(data_byte & 0x80) 			//Check we're sending 'one'		
						break;			//If not, continue to release line process

		case (FLAG_SEND|FLAG_DATA|FLAG_BIT|FLAG_ONE):		//0x53 Release line process
				SET_SPACE();
				NOP();
				NOP();
				//Arbitrate
			        if( READ_WIRE_STATE == PIN_STATE_MARK )
						local_flags |= FLAG_BREAK;
				local_flags |= FLAG_COMPLETE;
				break;	
		
		//Flags when FLAG_RECV is set 32 and up + 7
		case (FLAG_RECV|FLAG_RESET|FLAG_BIT):			//0x25  //We got reset from remote side
				local_flags |= FLAG_DATA;
				bits_to_process = 8;
				break;

		case (FLAG_RECV|FLAG_DATA|FLAG_BIT):	
				bits_to_process--;
				data_byte << 1;				//Zero timing, just shift	
				break;

                case (FLAG_RECV|FLAG_DATA|FLAG_BIT|FLAG_ONE):
                                data_byte |= 0x01;                       //One timing, set 'one'      
                                break;

		//Idle states. 
		case FLAG_BREAK:
			transport_reset(); 					
			local_flags = FLAG_IDLE;
			break;
	}

	if( READ_WIRE_STATE != PIN_STATE_MARK )
			local_flags |= FLAG_COMPLETE;								
	
	local_flags++;   //Transition to next state 
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


