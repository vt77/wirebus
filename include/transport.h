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

enum  wirebusErrorCode {
        ERROR_OK                                =   0,
        ERROR_TRANSMIT_ABORT                    =   1,
        ERROR_BUSY                              =   2
};

enum wirebusDeviceStates{
	WIREBUS_STATE_IDLE			=  0,
	WIREBUS_STATE_SEND			=  1,
	WIREBUS_STATE_RECV			=  2,
	WIREBUS_STATE_BREAK			=  3,
};

enum pinState{
        PIN_STATE_SPACE                         =       0,
        PIN_STATE_MARK                          =       1
};

typedef struct sWirebusDevice
{
        uint8_t         addr;
        uint8_t         state;
        uint8_t         bytes_cnt;
        uint8_t         data_byte;
        uint8_t         bits_to_process;
}wirebus_device;


uint8_t sendStart();
uint8_t readByte(wirebus_device *device);
uint8_t sendByte(wirebus_device *device);
uint8_t processDataTransfer(wirebus_device *device);

void    releaseLine();
void    transportInit();


#endif
