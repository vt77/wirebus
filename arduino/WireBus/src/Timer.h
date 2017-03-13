#ifndef WIREBUS_TIMER_H
#define WIREBUS_TIMER_H

#include <avr/io.h>

#define TIMER_INTERRUPT		TIMER1_COMPA_vect

#define WIREBUS_BIT_DELAY    256 - ( (F_CPU / 256) / 1250 )

void inline initWirebusTimer()
{

    TCCR1A = (1<<WGM01);    				// CTC counting mode
    TCCR1B = (0 << CS00)|(0 << CS01)|(1 << CS02);     	// set prescaler of 256
    TCNT1 = 0;              				// clear the timer count
    OCR1A = WIREBUS_BIT_DELAY;


#if defined(__AVR_ATmega8__)|| defined(__AVR_ATmega128__)

    TIFR |= _BV(OCF1A);      // clear any pending interrupts;
    TIMSK |=  _BV(OCIE1A) ;  // enable the output compare interrupt

#else

    // here if not ATmega8 or ATmega128
    TIFR1 |= _BV(OCF1A);     // clear any pending interrupts;
    TIMSK1 |=  _BV(OCIE1A) ; // enable the output compare interrupt

#endif
	
};


void inline wirebusResetTimer(uint8_t delay)
{
	TCNT0=WIREBUS_BIT_DELAY-delay;
#if defined(__AVR_ATmega8__)|| defined(__AVR_ATmega128__)
	TIFR |= _BV(OCF1A);      // clear any pending interrupts;
#else
	TIFR1 |= _BV(OCF1A);     // clear any pending interrupts;
#endif
}


#endif
