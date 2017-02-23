

#define _SET(type,name,bit)          type ## name  |= (1 << bit )
#define _CLEAR(type,name,bit)        type ## name  &= ~(1 << bit )
#define _TOGGLE(type,name,bit)       type ## name  ^= _BV(bit)    
#define _GET(type,name,bit)          ((type ## name >> bit) &  1)
#define _PUT(type,name,bit,value)    type ## name = ( type ## name & ( ~ _BV(bit)) ) | ( ( 1 & (unsigned char)value ) << bit )

//these macros are used by end user
#define OUTPUT(pin)         _SET(DDR,pin)    
#define INPUT(pin)          _CLEAR(DDR,pin)    
#define HIGH(pin)           _SET(PORT,pin)
#define LOW(pin)            _CLEAR(PORT,pin)    
#define TOGGLE(pin)         _TOGGLE(PORT,pin)    
#define READ(pin)           _GET(PIN,pin)


#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>


#define     save_device_addr(a)   eeprom_write_byte(0x0,a)
#define     load_device_addr()    eeprom_read_byte(0x0)

#define ENABLE_INTERRUPTS()             sei()  
#define DISABLE_INTERRUPTS()            cli() 

#define RESET_WATCHDOG()		wdt();

#define NOP()				__asm("nop");
