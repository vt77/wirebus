/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <daniel@vt77.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Marchasin
 * ----------------------------------------------------------------------------
 */

#include "wirebus.h"
#include "uart.h"
#include "proto.h"

uint8_t itoh( uint8_t b, uint8_t i )
{
	uint8_t t = ( i == 0 ? b : b >> 4  ) & 0x0F; 
	t += 0x30;
	if( t > 9 ) t += 0x27;
	return t;
}

void setup()
{
	proto_init();
	uart_init();
	uart_send('>');
	uart_start_receiver();
}

void loop()
{
	  unsigned char b;
	  wirebus_packet  *p;

	  if( ( b = uart_recieve(0) )  != 0 )
	  {
 			 //Echo char back
			 uart_send(b);

	  		 if( b == 0x0D )
             {
                        //Reset state machine regardless the state
						reset_sm();
						uart_send('\n');
						uart_send('>');

             }else if( (p = proto_check(b) ) != 0 )
			 {
				( p->size > 1) ? 
						wirebusSendMessage(p->hdr.priority, p->hdr.cmd , p->dst, p->size, p->data ):
						wirebusSendCommand(p->hdr.priority, p->hdr.cmd , p->dst, p->size == 1 ? p->data[0] : 0 );

				reset_sm();
				uart_send('>');
			 }
	  }

	  if( ( p = wirebus_check_new_data() ) != 0 )
	  {
			uart_send('M');	
			uart_send(':');
			uart_send( itoh( p->hdr.priority,0 ) );
			uart_send( itoh( p->hdr.priority,1 ) );
			uart_send( itoh( p->hdr.cmd, 0) );
			uart_send( itoh( p->hdr.cmd, 1 ) );
			uart_send( itoh( p->src, 0 ) );
			uart_send( itoh( p->src, 1 ) );
			uart_send( itoh( p->dst, 0) );
			uart_send( itoh( p->dst, 1) );
			uart_send( itoh( p->size,0 ) );
			uart_send( itoh( p->size,1 ) );

			for(b=0;b<p->size;b++){
				uart_send( itoh( p->data[b], 0) );
				uart_send( itoh( p->data[b], 1) );
			}
	 }
}
