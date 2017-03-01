/*
Copyright (c) 2016 Daniel Marchasin
All rights reserved.

Licensed under MIT License (the "License");
you may not use this file except in compliance with the License.

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/


#ifndef __WIREBUS_DEF_H
#define __WIREBUS_DEF_H

#include <stdint.h>
#include <platform.h>
#include <transport.h>

#define		WIREBUS_PRIORITY_ALERT		0b11
#define		WIREBUS_PRIORITY_COMMAND	0b10
#define		WIREBUS_PRIORITY_MESSAGE	0b01
#define		WIREBUS_PRIORITY_INFO		0b00


/*************************
	Commands
	Param : None
	Packet format : [priority][command][src][dst]
**************************/

#define		WIREBUS_CMD_BASE				0x00
#define		WIREBUS_CMD_NONE				WIREBUS_CMD_BASE + 0			//Empty command for internal use
#define		WIREBUS_CMD_REBOOT				WIREBUS_CMD_BASE + 1			//Asks device to reboot
#define		WIREBUS_CMD_GETSTATE				WIREBUS_CMD_BASE + 2			//Sent to device to get its state . WIREBUS_INFO_STATE replaied
#define		WIREBUS_CMD_GETDEVICEINFO			WIREBUS_CMD_BASE + 3			//Sent to device to get manufacturer info . Device replays its UUID_MAJOR and UUID_MINOIR. WIREBUS_MESSAGE_DEVICEINFO
#define		WIREBUS_CMD_PING				WIREBUS_CMD_BASE + 4			//Sent to device
#define		WIREBUS_CMD_INIT				WIREBUS_CMD_BASE + 5			//Sent by device on init
#define		WIREBUS_CMD_TOGGLE				WIREBUS_CMD_BASE + 6			//Sent to device toggle switch

/*
	Info packets
	Params : One byte
	Packet format : [priority][command][src][dst][param]
*/

#define		WIREBUS_MESSSAGE_BASE			0x10 
#define		WIREBUS_MESSAGE_PONG			WIREBUS_MESSSAGE_BASE + 1			//Sent by device pong response. Param = counter
#define		WIREBUS_MESSAGE_STATE			WIREBUS_MESSSAGE_BASE + 2			//Sent by device on state command (WIREBUS_CMD_GETSTATE)
#define		WIREBUS_MESSAGE_SWITCH			WIREBUS_MESSSAGE_BASE + 3			//Sent to switch device . Device respond with WIREBUS_INFO_STATE 
#define		WIREBUS_MESSAGE_SETADDR			WIREBUS_MESSSAGE_BASE + 4			//Sent to device to update its network address
#define		WIREBUS_MESSAGE_TIMER			WIREBUS_MESSSAGE_BASE + 5			//Sent to devices with timer functionality

/*
	Message
	Params : variable size
	Packet format : [priority][command][src][dst][size][params]
*/

#define		WIREBUS_DATA_BASE			0x30
#define		WIREBUS_DATA_DEVICEINFO			WIREBUS_DATA_BASE + 1		//Two byte device info UUID_MAJOR and UUID_MINOIR
#define         WIREBUS_DATA_SET                        WIREBUS_DATA_BASE + 2                   //Set Data. Device specific. 

#define 	WIREBUS_ADDRESS_BROADCAST	0xFF



/*
 * WireBus Interface
 *
 */


#define			WIREBUS_DEVICE_STATUS_READY	  	  	0
#define         	WIREBUS_DEVICE_STATUS_BUSY     	  		1
#define         	WIREBUS_DEVICE_STATUS_NEWDATA     		2
#define 		WIREBUS_DEVICE_STATUS_SKIP		  	3	
#define 		WIREBUS_DEVICE_STATUS_READ_START		4
#define 		WIREBUS_DEVICE_STATUS_READ_SRC_ADDR		5
#define 		WIREBUS_DEVICE_STATUS_READ_DST_ADDR		6
#define 		WIREBUS_DEVICE_STATUS_READ_DATA_SIZE		7
#define 		WIREBUS_DEVICE_STATUS_READ_DATA			8
#define 		WIREBUS_DEVICE_STATUS_READ_CRC			9


#ifndef WIREBUS_MAX_DATA
#define 	WIREBUS_MAX_DATA	8
#endif

typedef struct{
        uint8_t         cmd;
        uint8_t         src;
        uint8_t         dst;
        uint8_t         size;
        uint8_t         data[WIREBUS_MAX_DATA];
        uint8_t         crc;
}packet_t;

typedef union{
        packet_t        	p;
        uint8_t                 b[WIREBUS_MAX_DATA+5];
}wirebus_packet;


#define		EXTRACT_PRIORITY(a)	( a >> 6 )
#define		EXTRACT_COMMAND(a)   ( a & 0x3F)


#define         ASSERT(a)  if(a != ERROR_OK) goto error

/*
typedef struct sWirebusDevice
{
        uint8_t         addr;
        uint8_t   	state;
	uint8_t		bytes_cnt;
	uint8_t  	data_byte;
	uint8_t  	bits_to_process;
}wirebus_device;
*/

void    wirebusInit(wirebus_device *dev);
uint8_t wirebusSendPacket( wirebus_device *dev, wirebus_packet *p );
uint8_t wirebusSendMessage( wirebus_device *dev, uint8_t priority, uint8_t cmd , uint8_t dst, wirebus_packet *p );
uint8_t wirebusProcess(wirebus_device *dev, wirebus_packet *p);

#endif
