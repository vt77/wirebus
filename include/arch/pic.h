#ifndef _INPUT_PIN_H
#define _INPUT_PIN_H

#ifdef __PIC12__

#define _SET(type,name,bit)          type ## bit = 1
#define _CLEAR(type,name,bit)        type ## bit = 0
#define _TOGGLE(type,name,bit)       type ## bit^= 1
#define _GET(type,name,bit)          type ## bit

//these macros are used by end user
#define OUTPUT(pin)         _SET(TRISIO,pin)    
#define INPUT(pin)          _CLEAR(TRISIO,pin)    
#define HIGH(pin)           _SET(GP,pin)
#define LOW(pin)            _CLEAR(GP,pin)    
#define TOGGLE(pin)         _TOGGLE(GP,pin)    
//#define READ(pin)           _GET(WPU,pin)
#define READ(pin)           _GET(GP,pin)
#define PULLUP_ON(pin)       _SET(WPU,pin)
#define PULLUP_OFF(pin)      _CLEAR(WPU,pin)

#else

#define _SET(type,name,bit)          type ## name ## bit  = 1    
#define _CLEAR(type,name,bit)        type ## name ## bit  = 0        
#define _TOGGLE(type,name,bit)       type ## name ## bit  ^= 1  
#define _GET(type,name,bit)          type ## name ## bit

//these macros are used by end user
#define OUTPUT(pin)         _SET(TRIS,pin)    
#define INPUT(pin)          _CLEAR(TRIS,pin)    
#define HIGH(pin)           _SET(PORT,pin)
#define LOW(pin)            _CLEAR(PORT,pin)    
#define TOGGLE(pin)         _TOGGLE(PORT,pin)    
#define READ(pin)           _GET(PORT,pin)
#define PULLUP_ON(pin)       _SET(WPU,pin)
#define PULLUP_OFF(pin)      _CLEAR(WPU,pin)


#endif


#endif
