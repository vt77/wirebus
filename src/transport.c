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
#include "transport.h"
#include "wirebus.h"

typedef void (*fsm_dispatch)();
uint8_t onByteReceived(uint8_t pos, uint8_t b);

static volatile uint8_t counter;
static volatile wirebus wire;
static volatile uint8_t pos;


void transport_init()
{
	SETUP_WIREPIN;
	START_RECEIVER;
}

void delayTicks(uint8_t cnt)
{
	counter=0;
	RESET_TIMER;
	START_TIMER;
	while(counter<cnt);
	STOP_TIMER;
}


uint8_t sendStart()
{
	uint8_t i;
	
	START_RECEIVER
	
	//Listen line for 5 periods
	for(i=0;i<5;i++)
	{
		if( wire.pinState == PIN_STATE_MARK )
				  return ERROR_TRANSMIT_ABORT;		
		DELAY(1);
	}

	//Send preambule
	SET_MARK;
	DELAY(8);
	SET_SPACE;
	DELAY(1);
	
	//Someone holds line down. Abort transmit
	if( wire.pinState == PIN_STATE_MARK )
				  return ERROR_TRANSMIT_ABORT;

	wire.state =  WIREBUS_TRANSPORT_STATE_TRANSMIT;

	return ERROR_OK;
}

void releaseLine()
{
	wire.state =  WIREBUS_TRANSPORT_STATE_IDLE;	
	START_RECEIVER;
}


uint8_t sendByte(uint8_t byte)
{
	wire.bits_count = 8;
	wire.data		= byte;

	while(wire.bits_count--)
	{
		SET_MARK;
		if( wire.data & 0x80 )
			DELAY(2);
		DELAY(1);
		SET_SPACE;
		wire.data <<= 1;
		DELAY(1);
		
		if( wire.pinState == PIN_STATE_MARK  )
		{
			//Someone else holds line down.
			//Stop transmitting and return error
			return ERROR_TRANSMIT_ABORT;	
		}
	}
	return ERROR_OK;
}

/* Level changed to mark after silence. Start count delay */
void startTimer()
{
	RESET_TIMER;
	START_TIMER;
	counter = 0;
}

/* level changed to space after mark in WAITFORSTART state. Check preambule if it long enouph start read packet  */
void startReadByte()
{
	if( counter < MIN_PREAMBULE_LENGHT ){
		return;
	}

	wire.state = WIREBUS_TRANSPORT_STATE_READ;
	wire.data = 0;
	wire.bits_count = 0;

	RESET_TIMER;
	counter = 0;
}

/* level chaged to mark in WIREBUS_TRANSPORT_STATE_READ . Start receive bit */
void countPeriods()
{
	//Just for debug
	//PORTB  &= ~_BV(4);
	RESET_TIMER;
	counter = 0;	
}

/* Bit received  */
void nextByte()
{
    if( counter > 1 )
		wire.data |= 1;
	
	if( ++wire.bits_count == 9 )
	{
		
		if( onByteReceived( pos++, wire.data ) != 0 )
		{
					wire.state =  WIREBUS_TRANSPORT_STATE_IDLE;
					STOP_TIMER;
					return;
		}
		
		wire.data = 0;
        wire.bits_count = 0;
		
		return;	
	}

	wire.data <<= 1;
}

fsm_dispatch fsm [2][2] = {
	/*PIN_STATE_SPACE , PIN_STATE_MARK */

	/*WIREBUS_TRANSPORT_STATE_WAITFORSTART*/
	{  startReadByte , startTimer       },

	/* WIREBUS_TRANSPORT_STATE_READ */
	{  nextByte , countPeriods     }
};

#ifndef PINCHANGE_VECTOR 
#error PINCHANGE_VECTOR not defined 
#endif


#ifndef TIMER_VECTOR 
#error TIMER_VECTOR not defined 
#endif


ISR( PINCHANGE_VECTOR )
{
	wire.pinState = READ_WIRE_STATE  ? PIN_STATE_SPACE : PIN_STATE_MARK;
	if( wire.state != WIREBUS_TRANSPORT_STATE_IDLE && wire.state != WIREBUS_TRANSPORT_STATE_READ )
			return;	
	fsm[wire.state][wire.pinState]();
}

#ifdef  ON_TIMER_FUNC
extern ON_TIMER_FUNC (uint8_t cntr);
#endif

ISR( TIMER_VECTOR )
{
        counter++;
#ifdef	ON_TIMER_FUNC
	ON_TIMER_FUNC(counter);
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
