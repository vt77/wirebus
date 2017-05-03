#include <WireBus.h>
#include <EEPROM.h>
#include <avr/wdt.h>

#ifdef __SIMUL__

#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega328p");

const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
    { AVR_MCU_VCD_SYMBOL("DDRB"), .what = (void*)&DDRB, },
    { AVR_MCU_VCD_SYMBOL("TCCR0B"), .what = (void*)&TCCR0B, },
    { AVR_MCU_VCD_SYMBOL("TCCR0A"), .what = (void*)&TCCR0A, },
    { AVR_MCU_VCD_SYMBOL("TCNT0"), .what = (void*)&TCNT0, },
    { AVR_MCU_VCD_SYMBOL("OCR0A"), .what = (void*)&OCR0A, },
};

#endif

#define WIRE_PIN	1

//WARNING!!!!!
//Please define YOUR OWN WIREBUS_UUIDs. It should be unequal per device type
//For more info please visit https://gitlab.com/vt77/wirebus
#define WIREBUS_UUID_MAJOR	0x06
#define WIREBUS_UUID_MINOR      0x02


static uint8_t localAddress = 0; 
static uint8_t pong_count = 0;

//This is global on purpose to prevent 
//big variable allocation in stack at runtime
wirebus_packet p;

void processCommandByte(uint8_t byte);
int commandComplete();
uint8_t hex2bin(unsigned char *);


//Input processing states
#define STATE_START 		0
#define STATE_FIRST_CHAR 	1
#define STATE_BYTE_READY	2


uint8_t state = STATE_START;

void setup()
{
	//It's always good idea to enable watchdog
	wdt_enable(WDTO_1S);

	wirebusInit( WIRE_PIN );	
	wirebusSetLocalAddr(EEPROM.read(localAddress));
	wirebusSendMessage(WIREBUS_PRIORITY_INFO,WIREBUS_CMD_INIT,0xFF,0);

	Serial.begin(19200);
}


void loop()
{
	uint8_t cmd = wirebusProcess(&p);

	if( cmd != WIREBUS_CMD_NONE)
	{

		uint8_t dst = p.src;

		switch(cmd)
		{
			case    WIREBUS_CMD_PING:
                                   p.size = 1;
                                   p.data[0] = pong_count++;
                                   wirebusSendMessage(WIREBUS_PRIORITY_INFO,WIREBUS_MESSAGE_PONG,dst,&p);
                                   break;


			case 	WIREBUS_MESSAGE_SETADDR:
					EEPROM.write(localAddress, p.data[0] );
					wirebusSetLocalAddr(p.data[0]);
					break;

                        case    WIREBUS_CMD_REBOOT:
                                           while(1); //Just halt and let wachdog do the job;

                        case    WIREBUS_CMD_GETDEVICEINFO:
                                     p.size = 2;
                                     p.data[0] = WIREBUS_UUID_MAJOR;
                                     p.data[1] = WIREBUS_UUID_MINOR;
                                     wirebusSendMessage(WIREBUS_PRIORITY_MESSAGE, WIREBUS_DATA_DEVICEINFO,dst, &p);
				     break;	
			default:
				    //In any other case send command to host
				    Serial.print(p.cmd, HEX);
				    Serial.print(p.src, HEX);
				    Serial.print(p.dst, HEX);
				    Serial.print(p.size, HEX);
				    Serial.println(";");
				    break;
		}
	}

         //Read command from serial port
         //Command sent as series of HEX encoded bytes terminated by ; character. EOL is just skipped         

         unsigned char chars_buffer[2];
	 if (Serial.available() > 0) {
               int inByte = Serial.read();
	       //Process bytes by very primitive state machine
	       if( (inByte >= '0' && inByte <= '9' ) || (inByte >= 'A' && inByte <= 'F' ) )
	       {
			if(state==STATE_FIRST_CHAR)
			{	chars_buffer[1] = inByte;	
				processCommandByte(hex2bin(chars_buffer));
				state=STATE_BYTE_READY;
			}else{
				chars_buffer[0] = inByte;
				state=STATE_FIRST_CHAR;
			}

	       }else if(inByte == ';')
	       {	
			int result = commandComplete();
			Serial.println( result == 0 ? "OK" : "ERROR");
	       }
	 }
         
	 wdt_reset();	
}

uint8_t hex2bin(unsigned char *b)
{
   uint8_t result = 0;
   //                            A -  F           0 - 9
   result = ( b[0] > 0x39 ? (b[0] - 0x37) : (b[0] - 0x30) ) << 4;
   result |= ( b[1] > 0x39 ? (b[1] - 0x37) : (b[1] - 0x30) ); 
   return result;
}


void processCommandByte(uint8_t byte)
{

}

int commandComplete()
{
	return 0;
}

