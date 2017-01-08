#ifndef __WIREBUS_TRANSPORT_H
#define __WIREBUS_TRANSPORT_H

#include  <stdint.h>

#ifdef __ARCH_AVR__
#include <avr/io.h> 
#include <avr/interrupt.h>
#endif

#ifdef __ARCH_PIC__
#include <htc.h>
#endif



#define WIREBUS_TRANSPORT_STATE_IDLE 			0
#define WIREBUS_TRANSPORT_STATE_READ 			1
#define WIREBUS_TRANSPORT_STATE_TRANSMIT		2

#define MIN_PREAMBULE_LENGHT	6

enum  wirebusErrorCode {
	ERROR_OK				=   0,
	ERROR_TRANSMIT_ABORT			=   1
};

enum pinStateCode{
	PIN_STATE_SPACE				=	0,
	PIN_STATE_MARK		    		=	1
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


#ifdef __MCU_attiny2313__
#include "mcu/attiny2313.h"
#endif

#ifdef __MCU_attiny13__
#include "mcu/attiny13.h"
#endif

#ifdef  __MCU_attiny85__
#include "mcu/attiny85.h"
#endif


#ifdef __MCU_12f629__ 
#include "mcu/pic12f629.h"
#endif


#ifndef WIREBUS_MCU
#error "Unknown microcontroller. Check ./devices directory for list of known devices"
#endif


//#define 		SET_SPACE		WIRE_PORT |= _BV(WIRE_PIN)
//#define 		SET_MARK		WIRE_PORT &= ~_BV(WIRE_PIN)
//#define 		READ_WIRE_STATE         (PINB & _BV(WIRE_PIN))

#define			DELAY(a)		delayTicks(a)		

#ifdef __GNUC__
#define NOINLINE __attribute__((noinline))
#define INLINE   __attribute__((always_inline))
#endif

#ifdef __XC8__
#define NOINLINE 
#define INLINE   
#endif


enum wirebusErrorCode sendByte(uint8_t byte);
enum wirebusErrorCode sendStart();
void releaseLine();
void transport_init();


#endif
