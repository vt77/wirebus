#ifndef _WIREBUS_DEVICES_STM8
#define _WIREBUS_DEVICES_STM8
#define WIREBUS_MCU	STM8

//Timer definitions

#define         NOW
#define         RESET_TIMER     
#define         PERIOD          

//
//  
//  Procedures to setup wirebus pins
//  Fire interrupt on pinchange
//

#define WIRE_PORT  GPIO0 
#define PINB       GPIO0

#define			SETUP_WIREPIN   
#define                 START_RECEIVER


//
//  Wire PIN manipulations. 
//  For SPACE and MARK states defenitions please refer to circuit diagram
//


#define SET_MARK        TRISIO0 = 0
#define SET_SPACE       TRISIO0 = 1

#define READ_WIRE_STATE GPIO0 == 1


//
//
//  Setup timer to fire interrupt per 0.5 ms
//  Procedures to start and stop timer
//

#define		 WIREBUS_BIT_DELAY    ( (9600000 / 256) / 1250 )
#define		 SETUP_TIMER     OCR0A = WIREBUS_BIT_DELAY;TCCR0A =(1<<WGM01);TCCR0B = 0; TIMSK0 |= (1<<OCIE0A);

#define START_TIMER     
#define STOP_TIMER      


//
//  Wire PIN manipulations. 
//  For SPACE and MARK states defenitions please refer to circuit diagram
//


#endif
