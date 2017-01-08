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

//One tick period 800 uSec . BitRate 1250 
//BIT_DELAY = (CPU_CLOCK / UART_PRESCALER ) / BAUD_RATE
#define WIREBUS_BIT_DELAY    ( (8000000 / 256) / 1250 )


#define START_TIMER     TCCR0A =(1<<WGM01);TCCR0B = PRESCALER; TIMSK |= (1<<OCIE0A);OCR0A = WIREBUS_BIT_DELAY;
#define STOP_TIMER   	TCCR0B=0;

#define SETUP_WIREPIN   PORTB &= ~_BV(WIRE_TX_PIN);DDRB &= ~_BV(WIRE_TX_PIN);  


#define SET_MARK     DDRB |= _BV(WIRE_TX_PIN) //Make port output. Hold line down
#define SET_SPACE	 DDRB  &= ~_BV(WIRE_TX_PIN) //Make port input . Release line


#define  PINCH_INTERRUPTNAME(a) INT #a _vect


#define TIMER_CTC_VECTOR	TIM0_COMPA_vect
#define PINCHANGE_VECTOR	INT0_vect

#define  READ_WIRE_STATE  ( PINB & (1 << PINB2))


#endif
