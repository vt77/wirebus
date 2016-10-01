#ifndef __WIREBUS_DEF_H
#define __WIREBUS_DEF_H


#include "transport.h"


#define		WIREBUS_PRIORITY_CMD		0b11
#define		WIREBUS_PRIORITY_ALERT		0b10
#define		WIREBUS_PRIORITY_MESSAGE	0b01
#define		WIREBUS_PRIORITY_INFO		0b00


#define		WIREBUS_CMD_BASE			0x00
#define		WIREBUS_CMD_PING			WIREBUS_CMD_BASE + 1
#define		WIREBUS_CMD_GETSTATUS		WIREBUS_CMD_BASE + 2
#define		WIREBUS_CMD_REPORT			WIREBUS_CMD_BASE + 3

#define		WIREBUS_CMD_MESSAGE_BASE	0x10
#define		WIREBUS_CMD_MESSAGE			WIREBUS_CMD_MESSAGE_BASE
#define		WIREBUS_CMD_MESSAGE_IR		WIREBUS_CMD_MESSAGE_BASE + 1

#define		WIREBUS_CMD_SYSTEM_BASE		0x30
#define		WIREBUS_CMD_SETEEPROM		WIREBUS_CMD_SYSTEM_BASE + 0x0E
#define		WIREBUS_CMD_REBOOT			WIREBUS_CMD_SYSTEM_BASE + 0x0F


typedef struct wirebusHeader 
{
	uint8_t priority:2;
 	uint8_t cmd:6;	
}wirebus_header;

typedef union {
	wirebus_header 	h;
	uint8_t			b;
}pkt_hdr;


typedef struct wirebusPacket{
	pkt_hdr		hdr;
	uint8_t		src;
	uint8_t		dst;
}wirebus_packet;

#endif
