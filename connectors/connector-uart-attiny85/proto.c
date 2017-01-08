/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <daniel@vt77.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Marchasin
 * ----------------------------------------------------------------------------
 */



#include "proto.h"
#include "wirebus.h"

uint8_t data_bytes_left = 0;

wirebus_packet  data_packet;
wbfsm_t state_machine;
uint8_t data_buffer[WIREBUS_MAX_DATA];

uint8_t tobin(uint8_t n)
{
        n -= 0x30;
        if( n > 9 )  n-= 0x27;
        return n;
}

void  proto_init()
{
	state_machine.state 		= FSM_IDLE;
	state_machine.lastchar     	= 0;
	state_machine.p			   	= &data_packet;	
	data_packet.data			= data_buffer;	
}



wirebus_packet* proto_check( char b)
{

			  wbfsm_t *m = &state_machine;

             if(!m->lastchar || b == 'C' )
             {
                        m->lastchar = b;
                        return 0;
             }

             if( m->lastchar == 'C' && b == ':' )
			 {
                        if( m->lastchar == 'C' && b == ':' )
                                               m->state  = FSM_START;
                        m->lastchar = 0;
                       return 0;
             }

 			 if( fsmprocess(b,m) && m->state  ==  FSM_COMPLETE )
             {
						return &data_packet;
             }

		return 0;
}


void  reset_sm()
{
	  state_machine.lastchar = 0;
      state_machine.state  = FSM_IDLE;
}



uint8_t parse_cmd(uint8_t c,wirebus_packet * p)
{
        //printf("Check command : 0x%X\n", c );

        p->hdr.priority = (c >> 6);
        p->hdr.cmd   = (c & 0x3F);

        return FSM_STATE_SRC;
}


uint8_t parse_src(uint8_t c,wirebus_packet * p)
{
        //printf("Check source : 0x%X\n", c );
        p->src = c;
        return FSM_STATE_DST;
}


uint8_t parse_dst(uint8_t c,wirebus_packet * p)
{
        //printf("Check destination : 0x%X\n", c );
        p->dst = c;
        p->size = ( p->hdr.cmd >> 4 );

        if( p->size == 0 )
                return FSM_COMPLETE;

        data_bytes_left = p->size;

        if( p->size == 1)
                return FSM_STATE_DATA;

        return FSM_STATE_SIZE;

}


uint8_t parse_size(uint8_t c,wirebus_packet * p)
{
        //printf("Check size : 0x%X\n", c );

        p->size = c;
        data_bytes_left = c;

        if(p->size > 0)
                return FSM_STATE_DATA;

        return FSM_COMPLETE;
}

uint8_t parse_data(uint8_t c,wirebus_packet * p)
{
        //printf("Check data : 0x%X\n", c );
        //TODO: Save data to buffer
        if(--data_bytes_left == 0)
                return FSM_COMPLETE;

        return FSM_STATE_DATA;
}

typedef uint8_t (*functype_t)(uint8_t,wirebus_packet *);
functype_t commands[] = {
        parse_cmd,
        parse_src,
        parse_dst,
        parse_size,
        parse_data
};


int fsmprocess(char c, wbfsm_t * m)
{
        if( m->state  > MAX_FSM_STATE )
                        return 0;

        uint8_t b = ( (tobin(m->lastchar) << 4) + tobin(c) );
        m->state = commands[m->state](b,m->p);
        m->lastchar = 0;

        return 1;
}

