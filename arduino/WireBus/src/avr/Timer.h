#ifndef WIREBUS_TIMER_H
#define WIREBUS_TIMER_H

#include <avr/io.h>

#define TIMER_INTERRUPT		TIMER1_COMPA_vect

void inline initWirebusTimer()
{

    TCCR1A = 0;             // normal counting mode
    TCCR1B = _BV(CS11);     // set prescaler of 8
    TCNT1 = 0;              // clear the timer count

#if defined(__AVR_ATmega8__)|| defined(__AVR_ATmega128__)

    TIFR |= _BV(OCF1A);      // clear any pending interrupts;
    TIMSK |=  _BV(OCIE1A) ;  // enable the output compare interrupt

#else

    // here if not ATmega8 or ATmega128
    TIFR1 |= _BV(OCF1A);     // clear any pending interrupts;
    TIMSK1 |=  _BV(OCIE1A) ; // enable the output compare interrupt

#endif
	
};


#endif
