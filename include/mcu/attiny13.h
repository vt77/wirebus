#ifndef _WIREBUS_DEVICES_13
#define _WIREBUS_DEVICES_13

#define WIREBUS_DEVICE  attiny13

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


#define START_TIMER     TCCR0A =(1<<WGM01);TCCR0B = PRESCALER; TIMSK0 |= (1<<OCIE0A);
#define STOP_TIMER      TCCR0B = 0;


#define                 WIRE_PORT               PORTB


#define TIMER_CTC_VECTOR	TIM0_COMPA_vect
#define PINCHANGE_VECTOR	INT0_vect

#endif
