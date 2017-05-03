#include <stdint.h>
#include <WireBus.h>
#include "Timer.h"
#include <avr/interrupt.h>
//#include <Arduino.h>

//Wire state flags
#define FLAG_IDLE		0x00		//Empty flag
#define FLAG_BIT		0x01		//The timer tick is bit value. Otherwise tick is sync
#define FLAG_SYNC		0x02		//Marks ONE period
#define FLAG_RESET		0x04		//Marks RESET period
#define FLAG_COMPLETE		0x08		//Skip line release when sending reset

//Device state flags
#define FLAG_RECV		0x10
#define FLAG_SEND       	0x20
#define FLAG_BREAK		0x40
#define FLAG_ONE		0x80


#define	digitalWrite(a,b)   digitalWrite ## b (a)

#define pinModeINPUT(a)	    DDRD  &= ~(1<<a)
#define pinModeOUTPUT(a)    DDRD  |= (1<<a)

#define pinMode(a,b)	    pinMode ## b (a)


#define SET_MARK() PORTD &= ~(1<<WIREBUS_PIN);
#define SET_SPACE() PORTD |= (1<<WIREBUS_PIN);

#define READ_WIRE_STATE(a)  		( (PIND & (1 << a ) ) ? 1 : 0 )
 	
#define PIN_STATE_MARK			0
#define PIN_STATE_SPACE			1


#define  NOP()		__asm__("nop\n\t") 

#define         EXTRACT_PRIORITY(a)     ( a >> 6 )
#define         EXTRACT_COMMAND(a)   ( a & 0x3F)


#define 	WIREBUS_PIN		2


static volatile uint8_t wirebus_flags		  		  = 0;
static uint8_t local_addr			  		  = 0;

typedef struct{
        uint8_t transfer_byte;
        uint8_t transfer_bits;
}transferData_t;
transferData_t transferData;


void handlePinChange();

void wirebusInit(uint8_t pin)
{
    	initWirebusTimer();
	DDRD  |= (1<<WIREBUS_PIN); 	//Make pin output
	//digitalWrite(pin, LOW);  //Tri-state mode    
    	//pinMode(pin,INPUT);
	//attachInterrupt(digitalPinToInterrupt(pin), handlePinChange, FALLING );
	SET_SPACE();
}

void wirebusSetLocalAddr(uint8_t addr)
{
	local_addr = addr;
}


//Note: this is blocking function
uint8_t wirebusSendMessage( uint8_t priority, uint8_t cmd , uint8_t dst, wirebus_packet *p )
{
	//detachInterrupt(digitalPinToInterrupt(wirebus_pin));

	uint8_t bytes_to_send  = 3;
	uint8_t send_byte = 0x33;

        wirebusResetTimer(WIREBUS_BIT_DELAY);
        wirebus_flags = 3;	//Count periods from FLAG_RESET to FLAG_COMPLETE	
	SET_MARK();

//	PORTD |= (1<<4);

	do{
//		PORTD ^= (1<<3);	

		while( (wirebus_flags & FLAG_COMPLETE )  == 0 )
		{

			PORTB = wirebus_flags;

			if(wirebus_flags & FLAG_BREAK)
					return FLAG_BREAK;
		}

		SET_SPACE();

		if( bytes_to_send  == 0 ) break ;

		transferData.transfer_byte = send_byte;
		transferData.transfer_bits = 8;	

		bytes_to_send--;

		wirebus_flags = FLAG_SEND;	

	}while(1);

	wirebus_flags = 0;

//	PORTD &= ~(1<<4);

	return 0;
}

/*******************************************
 *
 *  Internal functions below this line 
 *
 *****************************************/


/********************************************
*
*  Calculate crc
*
********************************************/

uint8_t calc_crc(uint8_t *ptr,uint8_t size)
{
        uint8_t crc_calculated = 0;

//        for(;size--;)
//        {
//                crc_calculated = crc_calculated ^ (*ptr++);
//        }

        return crc_calculated;
}



/*************************************
 *
 *  Checks received buffer for valid packet
 *
 * ***********************************/

uint8_t check_buffer(uint8_t crc,  wirebus_packet *p)
{
//        if( calc_crc(ts.buffer,ts.size) == crc)
//        {
//                p->cmd = EXTRACT_COMMAND(ts.buffer[0]);
//                return 1;
//        }

        p->cmd = WIREBUS_CMD_NONE;

        return 0;
}
/*
uint8_t inline reset_engine()
{
	//This function called each timer tick
	//When we in idle mode

	if(t->state == FLAG_SEND)
	{
		//Sending complete
		//Restore receiver interrupt
	
	}else if( t->state == FLAG_RECV )
	{
		//Receiving packet complete.
		//Remove break if we have any
	}

	t->state      = FLAG_IDLE;
	wirebus_flags = FLAG_IDLE;

	return 0;	
}

uint8_t inline start_receiving(transferState* t)
{
     transfer_byte = 0x00;
     t->size   = WIREBUS_MIN_HEADER_SIZE;
     t->state  = FLAG_RECV;
     wirebus_flags = FLAG_RECV;  //Reset all wire states	

     return 0;
}

uint8_t inline receive_byte(transferState* t)
{
      switch( ts.count )
      {
        case WIREBUS_CMD_OFFSET:
        	//Calculate size based on command : WIREBUS_CMD_BASE = 0 ,WIREBUS_MESSAGE_BASE=1 or WIREBUS_DATA_BASE=3
        	ts.size = ( ( transfer_byte & 0x30 ) >> 4) + WIREBUS_MIN_HEADER_SIZE;
        	break;
        case WIREBUS_DST_ADDR_OFFSET:
                if( ( transfer_byte != local_addr ) && (transfer_byte != 0xFF) ) //We don't want to receive alien's packets
                                       wirebus_flags = FLAG_BREAK;
                break;

       case WIREBUS_DATA_SIZE_OFFSET:
               if( ts.buffer[0] & 0x20 ) //This 'magic' number is just a check for WIREBUS_DATA_BASE command type 
                     ts.size += transfer_byte;
               break;
     }

      if(ts.count == ts.size)
      {

         wirebus_flags = FLAG_BREAK; //Skip all junk after packet received

         //Last byte in sequience crc
         if( check_buffer(transfer_byte , &p) )
                return p.cmd;    //Return received command

          return 0;
      }
      
      ts.buffer[ts.count++] = transfer_byte;
      transfer_byte = 0x0;

      return 0;
}



uint8_t inline send_byte(transferState* t)
{

	PORTD ^= (1<<4);

	transfer_byte = t->buffer[ t->count ];
	
	if(t->count++ < t->bytes_to_send)
	{
		transfer_bits = 8;
        	wirebus_flags = FLAG_SEND;	
	}else
	{
	 	 wirebus_flags = FLAG_BREAK; //Mark send complete
	}
	return 0;
}


uint8_t inline start_sending(transferState* t)
{

        PORTD ^= (1<<5);

        t->state = FLAG_SEND;
        t->count = 0;
        transfer_byte = t->buffer[0];
        wirebus_flags = FLAG_SEND;

        return send_byte(t);
}
*/

uint8_t wirebusProcess(wirebus_packet *p)
{

	/*
 	*	Only FLAG_COMPLETE triggers processing
 	*	Please note: when idle FLAG_COMPLETE will be set 
 	*	periodically. Good to implement software timer	
	*/

	if( ( wirebus_flags & FLAG_COMPLETE ) == 0 )
                                return 0;

        wirebus_flags &= ~FLAG_COMPLETE;

#ifdef __SIMUL__
	//gtkwave_trace
	PORTD ^= (1<<3);
	PORTB = wirebus_flags;
#endif

	uint8_t res = 0;

//	if(wirebus_flags == (FLAG_BREAK|FLAG_RESET)) res = reset_engine();
//	else if(wirebus_flags == (FLAG_RESET|FLAG_SEND)) res = start_sending(p);
//	else if(wirebus_flags == (FLAG_RESET|FLAG_RECV)) res = start_receiving(p;
//	else if(wirebus_flags == (FLAG_RECV)) res = receive_byte(p);
//	else if(wirebus_flags == (FLAG_SEND)) res = send_byte(p);

//	PORTB = wirebus_flags;

	return res;
};

void handlePinChange()
{
	//The condition  below is useless because we defined interrupt as FALLING
	//Uncomment if you're using another interrupt setup
	//if( READ_WIRE_STATE(wirebus_pin) == PIN_STATE_MARK )
	//{
		wirebus_flags = FLAG_RECV;		//Clear flags
		wirebusResetTimer(WIREBUS_BIT_DELAY/2);	//Fire timer on center of period
	//}
}

uint8_t  inline process_bit( transferData_t *t)
{

	if( t->transfer_bits == 0 ) //Nothing to send or receive. Just return
			return 0;	

	//Check collision
	if((( t->transfer_byte & 0x80 ) == 0 ) && (wirebus_flags & FLAG_ONE))
	{
		//We're sending '0' but got '1' , collision detected
		wirebus_flags |= FLAG_BREAK;
		return 0;
	}
	
	//This has no effect while sending
	if(wirebus_flags & FLAG_ONE)
			t->transfer_byte |= 1;

	wirebus_flags &= 0x70;  //Clear line status flags and FLAG_ONE

	if(--t->transfer_bits == 0)
		return 1;
	
	//Prepare for next bit
	t->transfer_byte <<=1; 

	//Define we want to send '1';
	//This have no sense  when receiving because high bit is always zero
	if(( t->transfer_byte & 0x80 ) )
			wirebus_flags |= FLAG_ONE;

	return 0;		
}


SIGNAL (TIMER_INTERRUPT)
{	

        uint8_t local_flags  = wirebus_flags & 0x0F;

#ifdef __SIMUL__
	//gtkwave_trace
        PORTD ^= 1;
	PORTC = wirebus_flags;
#endif

        switch(local_flags)
        {
                case 0:
                        if( wirebus_flags & FLAG_SEND )
                                        SET_MARK();
                        break;

                case 1:
			if( ( wirebus_flags & FLAG_ONE ) == 0 ) //Release line on bit tick when sending '0'
					SET_SPACE();
			NOP(); //Wait for line change
			NOP(); 
			NOP();

			if( ( PIND & (1<<WIREBUS_PIN) ) == 0)   //Set flag we received '1'
						wirebus_flags |= FLAG_ONE;

                        break;

                case 2:
			SET_SPACE();
			if( process_bit(&transferData) )
					wirebus_flags |= FLAG_COMPLETE; 
			return;

		case (FLAG_RECV|FLAG_RESET): //Receiving reset
				wirebus_flags |= FLAG_COMPLETE;
				return;
	}

        wirebus_flags++;
}

#ifdef __SIMUL__

#warning Compiling simulator version

#include <util/delay.h>
#include "avr/avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega328p");


const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
    { AVR_MCU_VCD_SYMBOL("DDRD"), .what = (void*)&DDRD, },
    { AVR_MCU_VCD_SYMBOL("PORTD"), .what = (void*)&PORTD, },
    { AVR_MCU_VCD_SYMBOL("PORTB"), .what = (void*)&PORTB, },
    { AVR_MCU_VCD_SYMBOL("PORTC"), .what = (void*)&PORTC, },
    { AVR_MCU_VCD_SYMBOL("TCNT1"), .what = (void*)&TCNT1, },
};


FUSES = 
{
    .low = 0xE2,
    .high = 0xD9,
    .extended = EFUSE_DEFAULT,
};

#define WIRE_PIN        2


//WARNING!!!!!
//Please define YOUR OWN WIREBUS_UUIDs. It should be unequal per device type
//For more info please visit https://gitlab.com/vt77/wirebus
#define WIREBUS_UUID_MAJOR      0x06
#define WIREBUS_UUID_MINOR      0x02


wirebus_packet p;

int main()
{
        //It's always good idea to enable watchdog
        //wdt_enable(WDTO_1S);

        wirebusInit( WIRE_PIN );
        wirebusSetLocalAddr(0x33);
	
	sei();

	_delay_ms(100);

        wirebusSendMessage(WIREBUS_PRIORITY_INFO,WIREBUS_CMD_INIT,0xFF,0);

        //_delay_ms(1000);
        //sleep_cpu();

	while(1)
	{
		wirebusProcess(&p);
		if(p.cmd != WIREBUS_CMD_NONE)
		{
			//Process Packet
			
		}

	}

        return 0;
}

#endif
