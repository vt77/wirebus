
#include "wirebus.h"

uint8_t  		pos;
wirebus_packet  packet;

uint8_t onByteReceived(uint8_t b)
{
	((uint8_t*)&packet)[pos++] = b;

	if(pos==3)
	{
		//header received
		//Check it

		if( packet.dst == device.addr || packet.dst == 0xFF )
		{
			//Its our or broadcast
			packet.hdr.h.cmd , packet.src ;
		}
	}

	return 0;
}
