
#define F_CPU 9600000U

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "transport.h"

typedef void (*fsm_dispatch)();

static volatile uint8_t counter;
wirebus wire;

void delayTicks(uint8_t cnt)
{
	counter=0;
	RESET_TIMER;
	START_TIMER
	while(counter<cnt);
	STOP_TIMER;
}


enum wirebusErrorCode sendByte(uint8_t byte)
{
	wire.bits_count = 8;
	wire.data		= byte;

	wire.state =  WIREBUS_TRANSPORT_STATE_TRANSMIT;

	SET_MARK;
	DELAY(8);
	SET_SPACE;
	DELAY(1);

	while(wire.bits_count--)
	{
		SET_MARK;
		if( wire.data & 0x80 )
			DELAY(2);
		DELAY(1);
		SET_SPACE;
		wire.data <<= 1;
		DELAY(1);
		
		//if( READ_WIRE_STATE  != 1  )
		//{
			//Some one else holds line down.
			//Stop transmitting and return error
		//	return ERROR_TRANSMIT_ABORT;	
		//}
	}

	wire.state =  WIREBUS_TRANSPORT_STATE_WAITFORSTART;
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


/* level chaged to space in WIREBUS_TRANSPORT_STATE_READ . Bit received  */
void nextByte()
{
//Just for debug
//	PORTB |= _BV(4);

    if( counter > 1 )
		wire.data |= 1;
	
	if( ++wire.bits_count == 8 )
	{
		
		if( wire.onReceive != 0 ){
			if( !wire.onReceive( wire.data ) )
			{
					wire.state =  WIREBUS_TRANSPORT_STATE_WAITFORSTART;
					STOP_TIMER;
					return;
			}
		}
		
		wire.data = 0;
        wire.bits_count = 0;
		
		return;	
	}

	wire.data <<= 1;

}


fsm_dispatch fsm [2][2] = {
	/*WIREBUS_TRANSPORT_STATE_WAITFORSTART*/
	/*PIN_STATE_SPACE , PIN_STATE_MARK */
	{  startReadByte , startTimer       },
	/* WIREBUS_TRANSPORT_STATE_READ */
	{  nextByte , countPeriods     }
};



ISR(INT0_vect)
{
	if( wire.state != WIREBUS_TRANSPORT_STATE_WAITFORSTART && wire.state != WIREBUS_TRANSPORT_STATE_READ )
			return;

	wire.pinState = READ_WIRE_STATE  ? PIN_STATE_SPACE : PIN_STATE_MARK;
	fsm[wire.state][wire.pinState]();	
/*
	if( wire.pinState == PIN_STATE_MARK )
	{
		WIRE_PORT &= ~_BV(3);

	}else{

		WIRE_PORT  |= _BV(3);
	}
*/
}


ISR(TIM0_COMPA_vect)
{
        counter++;


		//Just for debug
		PORTB ^= _BV(2);

}

