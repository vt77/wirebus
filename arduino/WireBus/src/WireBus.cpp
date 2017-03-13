#include <stdint.h>
#include <WireBus.h>
#include "Timer.h"
#include <avr/interrupt.h>
#include <Arduino.h>


//Wire state flags
#define FLAG_IDLE		0x00		//Empty flag
#define FLAG_BIT		0x01		//The timer tick is bit value. Otherwise tick is sync
#define FLAG_ONE		0x02		//Marks ONE period
#define FLAG_RESET		0x04		//Marks RESET period
#define FLAG_SKIP		0x08		//Skip line release when sending reset

//Device state flags
#define FLAG_RECV		0x10
#define FLAG_SEND       	0x20
#define FLAG_COMPLETE		0x40		
#define FLAG_BREAK		0x80

//pin manipulation macros
#define SET_MARK(a)			pinMode(a,OUTPUT)
#define SET_SPACE(a)			pinMode(a,INPUT)
#define READ_WIRE_STATE(a)  		digitalRead(a)		
#define PIN_STATE_MARK			0
#define PIN_STATE_SPACE			1


#define  NOP()		__asm__("nop\n\t") 

#define         EXTRACT_PRIORITY(a)     ( a >> 6 )
#define         EXTRACT_COMMAND(a)   ( a & 0x3F)


static volatile uint8_t wirebus_flags		  = 0;
static uint8_t wirebus_pin					  = 0;
static uint8_t recv_byte					  = 0;
static uint8_t send_byte					  = 0;
static uint8_t send_bit						  = 0;
static uint8_t local_addr			  = 0;


typedef struct{
	uint8_t count;
	uint8_t size;
	uint8_t buffer[WIREBUS_MAX_DATA+7];
	uint8_t state;
}transferState; 

transferState ts;

void handlePinChange();

void wirebusInit(uint8_t pin)
{
	wirebus_pin = pin;

    	initWirebusTimer();

    	pinMode(pin,OUTPUT);
	digitalWrite(pin, LOW);  //Tri-state mode    
    	pinMode(pin,INPUT);
	attachInterrupt(digitalPinToInterrupt(pin), handlePinChange, FALLING );
}

void wirebusSetLocalAddr(uint8_t addr)
{
	local_addr = addr;
}

/********************************************
*
*  Calculate crc
*
********************************************/

uint8_t calc_crc(uint8_t *ptr,uint8_t size)
{
        uint8_t crc_calculated = 0;

        for(;size--;)
        {
                crc_calculated = crc_calculated ^ (*ptr++);
        }

        return crc_calculated;
}


uint8_t wirebusSendMessage( uint8_t priority, uint8_t cmd , uint8_t dst, wirebus_packet *p )
{
	detachInterrupt(digitalPinToInterrupt(wirebus_pin));
	SET_MARK(wirebus_pin);
	wirebusResetTimer(WIREBUS_BIT_DELAY);
	wirebus_flags = (FLAG_SEND|FLAG_SKIP);
	send_bit  = 0; ts.state = FLAG_SEND;
	return 0;
}


/*************************************
 *
 *  Checks received buffer for valid packet
 *
 * ***********************************/

uint8_t check_buffer(uint8_t crc,  wirebus_packet *p)
{
        if( calc_crc(ts.buffer,ts.size) == crc)
        {
                p->cmd = EXTRACT_COMMAND(ts.buffer[0]);
                return 1;
        }

        p->cmd = WIREBUS_CMD_NONE;

        return 0;
}



uint8_t wirebusProcess(wirebus_packet *p)
{

	if( ( wirebus_flags & FLAG_BREAK ) ) 
			ts.state  = FLAG_BREAK;

	if( ( wirebus_flags == FLAG_IDLE ) && ( ts.state  != FLAG_IDLE ) )
	{
			ts.state  = FLAG_IDLE;
			ts.count  = 0;
			ts.size   = 0;
			attachInterrupt(digitalPinToInterrupt(wirebus_pin), handlePinChange, FALLING );
	}

	if( ( wirebus_flags & FLAG_COMPLETE ) == 0 ) 
				return 0;
	
	wirebus_flags &= ~FLAG_COMPLETE;

	if(ts.state == FLAG_IDLE)
	{
		if( wirebus_flags & FLAG_RECV )
		{
			//Start receive byte
			//Because recv_byte shifted left while receiving we init it by '1';
			//If '1' is shifted out from left it means 8 cycles done.
			recv_byte = 0x01;
			ts.size   = WIREBUS_MIN_HEADER_SIZE;
			ts.state  = FLAG_RECV;
			wirebus_flags = FLAG_RECV;  //Reset all wire states
		}

	}else if( ts.state == FLAG_RECV )
	{	
		//We're in receive state	
		
		if(ts.count <= ts.size)
		{
	
			switch( ts.count )
			{
				case WIREBUS_CMD_OFFSET:
					//Calculate size based on command : WIREBUS_CMD_BASE = 0 ,WIREBUS_MESSAGE_BASE=1 or WIREBUS_DATA_BASE=3
					ts.size = ( ( recv_byte & 0x30 ) >> 4) + WIREBUS_MIN_HEADER_SIZE;
					break;
				case WIREBUS_DST_ADDR_OFFSET:
					if( ( recv_byte != local_addr ) && (recv_byte != 0xFF) ) //We don't want to receive alien's packets
						ts.state = FLAG_SKIP;
					break;

				case WIREBUS_DATA_SIZE_OFFSET:
					if( ts.buffer[0] & 0x20 ) //This 'magic' number is just a check for WIREBUS_DATA_BASE command type 
			   			ts.size += recv_byte;
					break;	
			}

			if(ts.count == ts.size)
			{

				ts.state = FLAG_SKIP; //Skip all junk after packet received

				//Last byte in sequience crc
				if( check_buffer(recv_byte , p) )
						return p->cmd;    //Return received command

				return 0;
			}

			ts.buffer[ts.count++] = recv_byte;
                	recv_byte = 0x01;
		}
	}
	else if( ts.state == FLAG_SEND )
	{
			send_byte = ts.buffer[ts.count];
			send_bit = 0x80;
			wirebus_flags = FLAG_SEND;
	}

	return 0;
};

void handlePinChange()
{
	//The condition check bellow is useless because we defined interrupt as FALLING
	//Uncomment if you're using another interrupt setup
	//if( READ_WIRE_STATE(wirebus_pin) == PIN_STATE_MARK )
	//{
		wirebus_flags = FLAG_RECV;		//Clear flags
		wirebusResetTimer(WIREBUS_BIT_DELAY/2);	//Fire timer on center of period
	//}
}

/*

Flags progress example while Sending(S) and Receiving(R) 

NOTE:
When receiving timer fired in center of period.
When sending timer fired in end of period

-|_ _ _ _ _|-|_|-|_ _ _|- - - - - -
R 0 1 2 3 4 5 0 1 0 1 2 3 4 5 6 7 8
S 0 1 2 3 4 2 3 0 1 2 3 0 1 2 3 4  

*/

SIGNAL (TIMER_INTERRUPT)
{	
	uint8_t local_flags  = wirebus_flags & 0x0F;

	if( wirebus_flags & FLAG_RECV )
	{
		//When receiving timer fired in center of period
		//On MARK  just increase timer and  wait till line released
		if( READ_WIRE_STATE(wirebus_pin) == PIN_STATE_MARK )
		{
				wirebus_flags++;
				return; 
		}
		
		//We're in SPACE line state

		if(local_flags & FLAG_RESET )
		{
			//Initial reset . Nothing to do.
			wirebus_flags |= FLAG_COMPLETE;
			return;
		}

		if( recv_byte & 0x80 ){				
			//End of byte transfer 
			wirebus_flags |= FLAG_COMPLETE;
		}

		recv_byte <<= 1;

		if(wirebus_flags & FLAG_ONE )
					recv_byte |= 0x01;

		wirebus_flags &= ~FLAG_RECV;  //Flag is set by wire low level in interrupt 

	}

    	if( wirebus_flags & FLAG_SEND )
	{
		
		switch( local_flags )
		{
			case 0:									//FLAG_IDLE
			case 2:									//FLAG_ONE
				SET_MARK(wirebus_pin);
				break;
			case 3:									//FLAG_BIT | FLAG_ONE
				if(wirebus_flags & FLAG_SKIP)
				break;								//Skip while sending reset  
			case 4:									//FLAG_RESET
				wirebus_flags |= 0xF0;						//Clear wire state flags
				SET_SPACE(wirebus_pin);						//Release line

                		//Wait pin change
				NOP();
                		NOP();

                		//Arbitrate
                		if( READ_WIRE_STATE(wirebus_pin) == PIN_STATE_MARK )
				{		
						wirebus_flags &= ~FLAG_SEND;    //Clear send state
                        			wirebus_flags |= FLAG_BREAK;
						return;	
				}
				if(send_bit == 0)
				{
					//Send byte complete
					wirebus_flags |= FLAG_COMPLETE; //Next state is complete
					wirebus_flags &= ~FLAG_SEND;    //Clear send state
					return;
				}

				//Take next bit to send
				if( ( send_byte & (1 << send_bit ) ) == 0 )
					wirebus_flags |= FLAG_ONE; //Skip two leading marks when sending ZERO

				send_bit >>= 1;
				
				break;

		}
	}

	wirebus_flags++;

	if( wirebus_flags >= 6 && READ_WIRE_STATE(wirebus_pin) == PIN_STATE_SPACE )
				wirebus_flags = FLAG_IDLE; //RESET ALL STATES

}

