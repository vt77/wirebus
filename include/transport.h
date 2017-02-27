#ifndef __WIREBUS_TRANSPORT_H
#define __WIREBUS_TRANSPORT_H

#include  <stdint.h>

#ifndef WIREBUS_RX_PIN 
#error Please define WIREBUS_RX_PIN
#endif

#ifndef WIREBUS_TX_PIN 
#error Please define WIREBUS_TX_PIN
#endif

#define WIREBUS_TRANSPORT_STATE_IDLE 			0
#define WIREBUS_TRANSPORT_STATE_READ 			1
#define WIREBUS_TRANSPORT_STATE_TRANSMIT		2

#define MIN_PREAMBULE_LENGHT	6


#ifndef WIREBUS_MCU
#error "Unknown microcontroller. Check please include platform.h BEFORE transport.h"
#endif

#define			DELAY(a)		delayTicks(a)		


/* Flags used by statemachine in wirebus.c */

//#define         WIREBIS_STATE_IDLE      0x0     /* Default state, set by resetting all flags */
//#define         WIREBIS_STATE_BREAK     0x01    /* First possible state offset */
//#define         WIREBIS_STATE_SEND      0x02
//#define         WIREBIS_STATE_RECV      0x04
//#define         WIREBIS_STATE_COMPLETE  0x08

/* Line state flags */
//#define         FLAG_MARK               0x10 
//#define         DATA_TICK               0x20
//#define         BIT_ONE                 0x40
//#define         FLAG_RESET              0x80


///////////////////
// Flags 
//     0x08        0x07         0x06       0x05        0x04        0x03      0x02     0x01      
//[ FLAG_INT | FLAG_DATA | FLAG_RECV | FLAG_SEND | FLAG_BREAK | FLAG_SB |FLAG_ONE|FLAG_DATA]

#define         FLAG_IDLE       0x00 
#define         FLAG_BIT       0x01   //Data tick 
#define         FLAG_ONE        0x02   //Period to send/receive 'one'
#define         FLAG_RESET      0x04   //Period of SEND_START or BREAK  (third data tick)
#define         FLAG_BREAK      0x08   //BREAK received. Reached on  increment when no other action performed
#define         FLAG_SEND       0x10   //Sending something
#define         FLAG_RECV       0x20   //Rceiving something
#define         FLAG_DATA       0x40   //databyte is ready to process
#define         FLAG_COMPLETE   0x80   //External interrupt fired (on receive)



uint8_t sendByte(uint8_t byte);
uint8_t sendStart();
uint8_t readByte(uint8_t* byte);

void    releaseLine();
void    transport_init();

enum pinState{
        PIN_STATE_SPACE                         =       0,
        PIN_STATE_MARK                          =       1
};

#endif
