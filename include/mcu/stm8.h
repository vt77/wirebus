#ifndef _WIREBUS_DEVICES_STM8
#define _WIREBUS_DEVICES_STM8
#define WIREBUS_MCU	STM8


//Define pins

#define 		WIRE_RX_PIN  		7 
#define 		WIRE_TX_PIN       	6

//
//  Setup timer to fire interrupt per 0.5 ms
//  Procedures to start and stop timer
//

#define  PRESCALER  159 //160
#define  WIREBUS_BIT_DELAY	(10000) >> 8; // 16М / PRESCALER / 10000 = 10 Гц

#define		 SETUP_TIMER     	   \
							CLK_PCKENR2_bit.PCKEN21 = 1;\
							TIM1_PSCRH = 0;\
							TIM1_PSCRL = PRESCALER; \
							TIM1_ARRH = WIREBUS_BIT_DELAY; \
							TIM1_ARRL = (10000)& 0xFF;\
							TIM1_CR1_bit.URS = 1;\
							TIM1_EGR_bit.UG = 1;\
							TIM1_IER_bit.UIE = 1\


#define 	START_TIMER     	    TIM1_CR1_bit.CEN = 1;								
#define 	STOP_TIMER  			TIM1_CR1_bit.CEN = 0;
#define 	RESET_TIMER  			\
							TIM1_ARRH = WIREBUS_BIT_DELAY; \
							TIM1_ARRL = (10000)& 0xFF;\

//  
//  Procedures to setup wirebus pins
//  Fire interrupt on pinchange
//

#define			SETUP_WIREPIN	\
									PC_DDR_bit.DDR ## WIRE_TX_PIN = 1;\
									PC_DDR_bit.DDR ## WIRE_RX_PIN = 0;\

//External interrupt  rasing edge
#define         START_RECEIVER		PD_CR2_bit.C26     = 1;              					
#define         STOP_RECEIVER		PD_CR2_bit.C26     = 0;              					

//
//  Wire PIN manipulations. 
//  For SPACE and MARK states defenitions please refer to circuit diagram
//

#define SET_MARK        PC_ODR_bit.ODR ## WIRE_TX_PIN = 1
#define SET_SPACE       PC_ODR_bit.ODR ## WIRE_TX_PIN = 0

#define READ_WIRE_STATE PC_IDR_bit.IDR ## WIRE_RX_PIN

#endif
