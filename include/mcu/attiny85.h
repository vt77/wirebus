#ifndef _WIREBUS_DEVICES_85
#define _WIREBUS_DEVICES_85

#define WIREBUS_MCU  attiny85

#include "arch/avr.h"

#define                 START_RECEIVER  MCUCR = (1<<ISC00); GIMSK=(1<< INT0);
#define                 STOP_RECEIVER   GIMSK=0;


//Timer definitions
#define         NOW             TCNT0
#define         RESET_TIMER     TCNT0=0
#define         PERIOD          OCR0A


#define PRESCALER_64    (1 << CS00)|(1 << CS01)|(0 << CS02)
#define PRESCALER_256   (0 << CS00)|(0 << CS01)|(1 << CS02)
#define PRESCALER_1024  (1 << CS00)|(0 << CS01)|(1 << CS02)

#define PRESCALER PRESCALER_256

//One tick period 0.5 mSec . 
//BIT_DELAY = (CPU_CLOCK / UART_PRESCALER ) / BAUD_RATE
#define WIREBUS_BIT_DELAY    ( (8000000 / 256) / 1000 )


#define START_TIMER     TCCR0A =(1<<WGM01);TCCR0B = PRESCALER; TIMSK |= (1<<OCIE0A);OCR0A = WIREBUS_BIT_DELAY;
#define STOP_TIMER   	TCCR0B=0;

#define SETUP_WIREPIN   INPUT(WIREBUS_RX_PIN);\
		    	OUTPUT(WIREBUS_TX_PIN);

#define SET_MARK     	 HIGH(WIREBUS_TX_PIN)
#define SET_SPACE	 LOW(WIREBUS_TX_PIN) 

#define TIMER_VECTOR	TIM1_OVF_vect
#define PINCHANGE_VECTOR	INT0_vect

#define READ_WIRE_STATE READ(WIREBUS_RX_PIN)

#endif
