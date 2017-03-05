
#include <stdint.h>
#include <WireBus.h>
#include "Timer.h"
#include <avr/interrupt.h>
#include <Arduino.h>


#define FLAG_RECV	0x10
#define FLAG_SEND       0x20
#define FLAG_COMPLETE	0x40

#define SET_MARK()	
#define SET_SPACE()
#define GET_WIRE_STATE()	1


uint8_t transfer_sequence = 0;
uint8_t flags		  = 0;

void OnPinChange();

void wirebusInit(uint8_t pin)
{

    initWirebusTimer();

    pinMode(pin,INPUT);
    digitalWrite(pin, LOW);  //Tri-state mode
    
    attachInterrupt(digitalPinToInterrupt(pin), OnPinChange, FALLING );
	
}

uint8_t wirebusSendMessage( uint8_t priority, uint8_t cmd , uint8_t dst, wirebus_packet *p )
{
	return 0;
}


uint8_t wirebusProcess(wirebus_packet *p)
{
	return 0;
};

void OnPinChange()
{
	flags |= FLAG_RECV;	
}


SIGNAL (TIMER_INTERRUPT)
{
	if( flags & FLAG_SEND )
	{
		if(transfer_sequence & 1 ){
				SET_SPACE();		//Release line 
		}else{
				SET_MARK();
		}
		transfer_sequence >>= 1;
	}
	
	if( flags & FLAG_RECV )
	{
		transfer_sequence <<= 1;
		if( GET_WIRE_STATE() == WIREBUS_LINESTATE_SPACE)
				transfer_sequence |= 1;
	}

	if( GET_WIRE_STATE() == WIREBUS_LINESTATE_SPACE )
	{
		 flags |= FLAG_COMPLETE;
	}
}

