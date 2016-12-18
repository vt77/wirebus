#ifndef _WIREBUS_DEVICES_85
#define _WIREBUS_DEVICES_85

#define WIREBUS_MCU  attiny85


#define	WIRE_PORT		PORTB
#define WIRE_RX_PIN     2
#define WIRE_TX_PIN     2


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


#define START_TIMER     TCCR0A =(1<<WGM01);TCCR0B = PRESCALER; TIMSK |= (1<<OCIE0A);
#define STOP_TIMER   	TCCR0B=0;


#define SET_MARK     PORTB |= _BV(WIRE_TX_PIN) //Hold line down
#define SET_SPACE	 PORTB ^= _BV(WIRE_TX_PIN) //Release line


#define  PINCH_INTERRUPTNAME(a) INT #a _vect


#define TIMER_CTC_VECTOR	TIM0_COMPA_vect
#define PINCHANGE_VECTOR	INT0_vect

#define  READ_WIRE_STATE  PINB0==1


#endif
