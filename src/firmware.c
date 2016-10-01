


#define F_CPU 4800000U

#include "transport.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

extern wirebus wire;
static volatile uint8_t count;


uint8_t onByteReceived(uint8_t b)
{
	eeprom_write_byte((uint8_t *)0,b);
	return 0;
}

int main()
{
	wire.data = 10;
	wire.state = WIREBUS_TRANSPORT_STATE_WAITFORSTART;
	wire.onReceive = onByteReceived;

	DDRB |= _BV(WIRE_PIN) | _BV(2) | _BV(3)|  _BV(4);

	//Setup INT0 
	START_RECEIVER
	//setup timer
	START_TIMER
	PERIOD  = 10; //Timer ticks. Please adjust this value to match 250 uSek tick 

	sei();

	
	while(1){
	//	sendByte( 0b01010011 );
	//	_delay_ms(0xFF);			
	};
	return 0;
}



