#ifndef _WIREBUS_DEVICES_PIC12F683
#define _WIREBUS_DEVICES_PIC12F683
#define WIREBUS_MCU	pic12F683

//Define pins

#define 		WIRE_RX_PIN  		0 
#define 		WIRE_TX_PIN       	1

//
//  Setup timer to fire interrupt per 0.5 ms
//  Procedures to start and stop timer
//

#define		 WIREBUS_BIT_DELAY    ( 255 - (0.0005/0.000004) )
#define		 SETUP_TIMER     	   OPTION_REG = PS1

#define 	START_TIMER     	   	T0IE = 1
#define 	STOP_TIMER  			T0IE = 0
#define     RESET_TIMER 			TMR0 = WIREBUS_BIT_DELAY    

//
//  
//  Procedures to setup wirebus pins
//  Fire interrupt on pinchange
//

#define			SETUP_WIREPIN	\
									TRISIO ## WIRE_TX_PIN = 0;\
									GPIO ## WIRE_RX_PIN = 1;\

//External interrupt  rasing edge
#define         START_RECEIVER		INTEDG     = 1;              					
#define         STOP_RECEIVER		INTEDG     = 0;              					

//
//  Wire PIN manipulations. 
//  For SPACE and MARK states defenitions please refer to circuit diagram
//

#define SET_MARK        GPIO ## WIRE_TX_PIN = 1
#define SET_SPACE       GPIO ## WIRE_TX_PIN = 0

#define READ_WIRE_STATE GPIO ## WIRE_RX_PIN

#endif
