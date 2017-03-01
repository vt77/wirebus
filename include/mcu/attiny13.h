#ifndef _WIREBUS_DEVICES_13
#define _WIREBUS_DEVICES_13

#define WIREBUS_MCU  attiny13

#include "arch/avr.h"

#define                 START_RECEIVER()  MCUCR = (1<<ISC01); GIMSK=(1<< INT0);
#define                 STOP_RECEIVER()   GIMSK=0;

//Timer definitions
#define         NOW             TCNT0

#define PRESCALER_64    (1 << CS00)|(1 << CS01)|(0 << CS02)
#define PRESCALER_256   (0 << CS00)|(0 << CS01)|(1 << CS02)
#define PRESCALER_1024  (1 << CS00)|(0 << CS01)|(1 << CS02)

#define PRESCALER PRESCALER_256

//One tick period 800 uSec . BitRate 1250 
//BIT_DELAY = (CPU_CLOCK / UART_PRESCALER ) / BAUD_RATE
#define WIREBUS_BIT_DELAY    ( (9600000 / 256) / 1250 )


#define START_TIMER     OCR0A = WIREBUS_BIT_DELAY;TCCR0A =(1<<WGM01);TCCR0B = PRESCALER; TIMSK0 |= (1<<OCIE0A);
#define STOP_TIMER      TCCR0B=0;
#define RESET_TIMER(a)  TCNT0=WIREBUS_BIT_DELAY-a


#define SETUP_WIREPIN   INPUT(WIREBUS_RX_PIN);\
                        OUTPUT(WIREBUS_TX_PIN);

//#define SET_MARK         HIGH(WIREBUS_TX_PIN)
//#define SET_SPACE        LOW(WIREBUS_TX_PIN) 

#define SET_MARK(a)         ( DDRB |= (1 << 1) )
#define SET_SPACE(a)        ( DDRB &= ~(1 << 1) )

#define  _GET_TX_PIN(a,b)   PINB && (1 << b)
//BUG: _GET_TX_PIN(B,1) should be  _GET_TX_PIN(WIREBUS_TX_PIN)
#define  SET_WIRE_STATE(c) if(c) \
				 DDRB |= (1 << 0);\
			   else  DDRB &= 0b11111110\


#define  READ_WIRE_STATE  ( PINB & _BV(PB1))

#define TIMER_VECTOR		TIM0_COMPA_vect
#define PINCHANGE_VECTOR	INT0_vect

#endif
