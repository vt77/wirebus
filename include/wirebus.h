#ifndef __WIREBUS_DEF_H
#define __WIREBUS_DEF_H


#include "transport.h"


#define		WIREBUS_PRIORITY_ALERT		0b11
#define		WIREBUS_PRIORITY_COMMAND	0b10
#define		WIREBUS_PRIORITY_MESSAGE	0b01
#define		WIREBUS_PRIORITY_INFO		0b00


#ifndef WIREBUS_UUID_MAJOR
#error "Please define WIREBUS_UUID_MAJOR and WIREBUS_UUID_MINOR codes of your device"
#endif
#ifndef WIREBUS_UUID_MINOR
#error "Please define WIREBUS_UUID_MAJOR and WIREBUS_UUID_MINOR codes of your device"
#endif

/*************************
	Commands
	Param : None
	Packet format : [priority][command][src][dst]
**************************/

#define		WIREBUS_CMD_BASE				0x00
#define		WIREBUS_CMD_NONE				WIREBUS_CMD_BASE + 0			//Empty command for internal use
#define		WIREBUS_CMD_REBOOT				WIREBUS_CMD_BASE + 1			//Sent by device on init
#define		WIREBUS_CMD_GETSTATE			WIREBUS_CMD_BASE + 2			//Sent to device to get its state
#define		WIREBUS_CMD_GETDEVICEINFO		WIREBUS_CMD_BASE + 3			//Sent to device to get its state
#define		WIREBUS_CMD_PING				WIREBUS_CMD_BASE + 4			//Sent to device
#define		WIREBUS_ACK_INIT				WIREBUS_CMD_BASE + 5			//Sent by device on init
#define		WIREBUS_CMD_TOGGLE				WIREBUS_CMD_BASE + 6			//Sent to device toggle switch

/*
	Info packets
	Params : One byte
	Packet format : [priority][command][src][dst][param]
*/

#define		WIREBUS_INFO_BASE			0x010 
#define		WIREBUS_INFO_PONG			WIREBUS_INFO_BASE + 1			//Sent by device onpong response. Param = counter
#define		WIREBUS_INFO_STATE			WIREBUS_INFO_BASE + 2			//Sent by device on its status
#define		WIREBUS_DATA_SWITCH			WIREBUS_INFO_BASE + 3			//Sent to switch device . Device respond with STATE 
#define		WIREBUS_DATA_SETADDR		WIREBUS_INFO_BASE + 4			//Sent to device to update its network address

/*
	Message
	Params : variable size
	Packet format : [priority][command][src][dst][size][params]
*/

#define		WIREBUS_MESSAGE_BASE			0x30
#define		WIREBUS_MESSAGE_DEVICEINFO		WIREBUS_MESSAGE_BASE + 1


#define 	WIREBUS_ADDRESS_BROADCAST	0xFF

#define			WIREBUS_DEVICE_STATUS_READY	  	  		0
#define         WIREBUS_DEVICE_STATUS_BUSY     	  		1
#define         WIREBUS_DEVICE_STATUS_NEWDATA     		2
#define 		WIREBUS_DEVICE_STATUS_SKIP		  		3	
#define 		WIREBUS_DEVICE_STATUS_READ_START		4
#define 		WIREBUS_DEVICE_STATUS_READ_SRC_ADDR		5
#define 		WIREBUS_DEVICE_STATUS_READ_DST_ADDR		6
#define 		WIREBUS_DEVICE_STATUS_READ_DATA_SIZE	7
#define 		WIREBUS_DEVICE_STATUS_READ_DATA			8
#define 		WIREBUS_DEVICE_STATUS_READ_CRC			9


#define 		ERROR_OK		  	  0
#define			ERROR_CMD_UNKNOWN	  1
#define         ERROR_TRANSMIT_ABORT           2


#define 	ASSERT(a)  if(a != ERROR_OK) goto error


#define 	WIREBUS_MAX_DATA	8


typedef void (*wirebus_callback)();

/*
typedef struct
{
	unsigned  priority:2;
 	unsigned  cmd:6;	
}wirebus_header ;

typedef union {
	wirebus_header 	h;
	uint8_t		b;
}pkt_hdr;
*/

typedef struct
{
	uint8_t  priority;
 	uint8_t  cmd;	
}pkt_hdr ;


#define		PACKET_PRIORITY(a)	( a >> 6 )
#define		PACKET_COMMAND(a)   ( a & 0x3F)


typedef struct sWirebusPacket{
	pkt_hdr		hdr;
	uint8_t		src;
	uint8_t		dst;
	uint8_t		size;
	uint8_t		*data;
}wirebus_packet;

typedef uint8_t	device_status;

typedef struct sWirebusDevice
{
        uint8_t         addr;
        device_status   state;
		wirebus_callback func;
}wirebus_device;


void wirebus_init();
wirebus_packet *wirebus_check_new_data();

uint8_t wirebusSendCommand( uint8_t priority, uint8_t cmd , uint8_t dst, uint8_t data );
uint8_t wirebusSendMessage(  uint8_t priority,  uint8_t cmd ,  uint8_t dst,  uint8_t size, const uint8_t *data );
#define CRC(a,b)  a^=b


#endif
