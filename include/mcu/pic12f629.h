#ifndef _WIREBUS_DEVICES_PIC12F629
#define _WIREBUS_DEVICES_PIC12F629
#define WIREBUS_MCU	pic12F629

//Timer definitions
//#define         NOW             
#define         RESET_TIMER     
#define         PERIOD          


//#define PRESCALER_64    (1 << CS00)|(1 << CS01)|(0 << CS02)
//#define PRESCALER_256   (0 << CS00)|(0 << CS01)|(1 << CS02)
//#define PRESCALER_1024  (1 << CS00)|(0 << CS01)|(1 << CS02)

//#define PRESCALER PRESCALER_256


#define START_TIMER     
#define STOP_TIMER      


//#define TIMER_CTC_VECTOR        TIMER0_COMPA_vect
//#define PINCHANGE_VECTOR        INT0_vect


#define WIRE_PORT  GPIO0 
#define PINB       GPIO0


#define SET_MARK	TRISIO0 = 0
#define SET_SPACE	TRISIO0 = 1

#define READ_WIRE_STATE GPIO0 == 1

#endif
