#ifndef _INPUT_PIN_H
#define _INPUT_PIN_H


//#define _SET(type,name,bit)          type ## name  |= _BV(bit)    
#define _SET(type,name,bit)          P ## name ## _ ## type ## _bit.type ## bit = 1
#define _CLEAR(type,name,bit)        P ## name ## _ ## type ## _bit.type ## bit = 0     
#define _TOGGLE(type,name,bit)       P ## name ## _ ## type ## _bit.type ## bit ^= 1
#define _GET(type,name,bit)          P ## name ## _ ## type ## _bit.type ## bit
#define _PUT(type,name,bit,value)    type ## name = ( type ## name & ( ~ _BV(bit)) ) | ( ( 1 & (unsigned char)value ) << bit )

//these macros are used by end user
#define OUTPUT(pin)         _SET(DDR,pin)    
#define INPUT(pin)          _CLEAR(DDR,pin)    
#define HIGH(pin)           _SET(ODR,pin)
#define LOW(pin)            _CLEAR(ODR,pin)    
#define TOGGLE(pin)         _TOGGLE(PORT,pin)    
#define READ(pin)           _GET(IDR,pin)


#endif
