#ifndef _WIREBUS_DEVICES_PIC12F629
#define _WIREBUS_DEVICES_PIC12F629
#define WIREBUS_MCU	pic12F629

//Timer definitions
//#define         NOW             
#define         RESET_TIMER     
#define         PERIOD          


#define			SETUP_WIREPIN   
#define			SETUP_TIMER     OCR0A = WIREBUS_BIT_DELAY;TCCR0A =(1<<WGM01);TCCR0B = 0; TIMSK0 |= (1<<OCIE0A);

#define			START_RECEIVER

#define PRESCALER PRESCALER_256


#define START_TIMER     
#define STOP_TIMER      

#define WIREBUS_BIT_DELAY    ( (9600000 / 256) / 1250 )


//#define TIMER_CTC_VECTOR        TIMER0_COMPA_vect
//#define PINCHANGE_VECTOR        INT0_vect


#define WIRE_PORT  GPIO0 
#define PINB       GPIO0


#define SET_MARK	TRISIO0 = 0
#define SET_SPACE	TRISIO0 = 1

#define READ_WIRE_STATE GPIO0 == 1

#endif
