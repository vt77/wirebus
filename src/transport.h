#ifndef __WIREBUS_TRANSPORT_H
#define __WIREBUS_TRANSPORT_H

#include <avr/io.h>

#define WIREBUS_TRANSPORT_STATE_WAITFORSTART 	0
#define WIREBUS_TRANSPORT_STATE_READ 			1
#define WIREBUS_TRANSPORT_STATE_TRANSMIT		2


#define MIN_PREAMBULE_LENGHT	6


enum  wirebusErrorCode {
	ERROR_OK				=   0,
	ERROR_TRANSMIT_ABORT	=	1
};


enum pinStateCode{
	PIN_STATE_SPACE			=	0,
	PIN_STATE_MARK		    =	1
} ;


typedef uint8_t (*rcv_callback)(uint8_t);

typedef struct wirebus 
{
	uint8_t data;
 	uint8_t bits_count;	
	uint8_t state;
	enum pinStateCode pinState;
	rcv_callback onReceive;

}wirebus;


//Define wire pin
#define			WIRE_PORT		PORTB
#define WIRE_PIN                1
#define READ_WIRE_STATE		(PINB & _BV(WIRE_PIN))

#define			START_RECEIVER 	MCUCR = (1<<ISC00); GIMSK=(1<< INT0);
#define			STOP_RECEIVER	GIMSK=0;

//Timer definitions
#define         NOW             TCNT0
#define         RESET_TIMER     TCNT0=0
#define         PERIOD          OCR0A


#define PRESCALER_64 	(1 << CS00)|(1 << CS01)|(0 << CS02)
#define PRESCALER_256 	(0 << CS00)|(0 << CS01)|(1 << CS02)
#define PRESCALER_1024 	(1 << CS00)|(0 << CS01)|(1 << CS02)

#define PRESCALER PRESCALER_256


#define START_TIMER	TCCR0A =(1<<WGM01);TCCR0B = PRESCALER; TIMSK0 |= (1<<OCIE0A);
#define STOP_TIMER	TCCR0B = 0;


/*
#define         NOW             TCNT0
#define         RESET_TIMER     TCNT0=0

#define 		START_RECEIVER		GIMSK |= (1 << PCIE);PCMSK |= (1<<PCINT4);
#define			STOP_RECEIVER		PCMSK &= ~_BV(PCINT4);

// prescale timer to 1/1024th the clock rate
#define START_TIMER	TCCR0B |= (1 << CS02)|(1 << CS00)|(1<<WGM01)|(0<<WGM00);TIMSK0 |= 1<<OCIE0A;
#define STOP_TIMER	TCCR0B = 0;

#define         NOW             TCNT0
#define         RESET_TIMER     TCNT0=0 
#define         PERIOD          OCR0B

#define			WIRE_PORT		PORTB

#define 		SET_SPACE		WIRE_PORT |= _BV(WIRE_PIN)
#define 		SET_MARK		WIRE_PORT &= ~_BV(WIRE_PIN)

#define			DELAY(a)		_delay_ms(a)		
*/

#define 		SET_SPACE		WIRE_PORT |= _BV(WIRE_PIN)
#define 		SET_MARK		WIRE_PORT &= ~_BV(WIRE_PIN)

#define			DELAY(a)		delayTicks(a)		
enum wirebusErrorCode sendByte(uint8_t byte);


#endif
