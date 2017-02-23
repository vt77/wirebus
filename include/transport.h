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

#define         WIREBIS_STATE_IDLE      0x0     /* Default state, set by resetting all flags */
#define         WIREBIS_STATE_BREAK     0x01    /* First possible state offset */
#define         WIREBIS_STATE_SEND      0x02
#define         WIREBIS_STATE_RECV      0x04
#define         WIREBIS_STATE_COMPLETE  0x08

/* Line state flags */
#define         FLAG_MARK               0x10 
#define         DATA_TICK               0x20
#define         BIT_ONE                 0x40
#define         FLAG_RESET              0x80


uint8_t sendByte(uint8_t byte);
uint8_t sendStart();
uint8_t isTransportReady();

void    releaseLine();
void    transport_init();

enum pinState{
        PIN_STATE_SPACE                         =       0,
        PIN_STATE_MARK                          =       1
};

#endif
