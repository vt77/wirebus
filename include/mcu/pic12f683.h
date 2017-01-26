//#ifndef _WIREBUS_DEVICES_PIC12F683
//#define _WIREBUS_DEVICES_PIC12F683
#define WIREBUS_MCU	pic12F683

#define __PIC12__

#include "arch/pic.h"

//
//  Setup timer to fire interrupt per 0.5 ms
//  Procedures to start and stop timer
//

#define		 WIREBUS_BIT_DELAY    (uint8_t)( 255 - (0.0005/0.000004) )
#define		 SETUP_TIMER     	   OPTION_REG = PS1

#define 	START_TIMER     	   	T0IE = 1
#define 	STOP_TIMER  			T0IE = 0
#define     	RESET_TIMER 			TMR0 = WIREBUS_BIT_DELAY    

//
//  
//  Procedures to setup wirebus pins
//  Fire interrupt on pinchange
//

#define			SETUP_WIREPIN    INPUT(WIREBUS_RX_PIN);

//External interrupt  rasing edge
#define	START_RECEIVER		INTEDG     = 1;              					
#define	STOP_RECEIVER		INTEDG     = 0;              					

//
//  Wire PIN manipulations. 
//  For SPACE and MARK states defenitions please refer to circuit diagram
//


#define SET_MARK 	HIGH(WIREBUS_TX_PIN)
#define SET_SPACE 	LOW(WIREBUS_TX_PIN)

#define READ_WIRE_STATE	READ(WIREBUS_RX_PIN)

//Arch depended defines

#define _PIC12F683_PCONCAT(a,b)		a ## b	


#define ISR(vec)   inline void _PIC12F683_PCONCAT(isr_,vec) ()

#define	PINCHANGE_VECTOR	pin_change
#define	TIMER_VECTOR		timer_ovf

//#endif
