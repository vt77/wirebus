#ifndef __WIREBUS_TRANSPORT_H
#define __WIREBUS_TRANSPORT_H

#include  <stdint.h>

#ifdef __ARCH_AVR__
#include <avr/io.h> 
#include <avr/interrupt.h>
#endif

#ifdef __ARCH_PIC__
#include <htc.h>
#endif


#ifndef WIREBUS_RX_PIN 
#error Please define WIREBUS_RX_PIN
#endif

#ifndef WIREBUS_TX_PIN 
#error Please define WIREBUS_TX_PIN
#endif

#define WIREBUS_TRANSPORT_STATE_IDLE 			0
#define WIREBUS_TRANSPORT_STATE_READ 			1
#define WIREBUS_TRANSPORT_STATE_TRANSMIT		2

#define MIN_PREAMBULE_LENGHT	6

#ifdef __MCU_attiny2313__
#include "mcu/attiny2313.h"
#endif

#ifdef __MCU_attiny13__
#include "mcu/attiny13.h"
#endif

#ifdef  __MCU_attiny85__
#include "mcu/attiny85.h"
#endif


#ifdef __MCU_12f629__ 
#include "mcu/pic12f629.h"
#endif

#ifdef __MCU_12f683__ 
#include "mcu/pic12f683.h"
#endif

#ifdef __MCU_stm8s__
#include "mcu/stm8.h"
#endif

#ifdef __MCU_stm8l__
#include "mcu/stm8.h"
#endif


#ifndef WIREBUS_MCU
#error "Unknown microcontroller. Check devices.txt  for list of known devices and platforms"
#endif

#define			DELAY(a)		delayTicks(a)		

#ifdef __GNUC__
#define NOINLINE __attribute__((noinline))
#define INLINE   __attribute__((always_inline))
#endif

#ifdef __XC8__
#define NOINLINE noinline
#define INLINE   inline
#endif


uint8_t sendByte(uint8_t byte);
uint8_t sendStart();
void releaseLine();
void transport_init();

#endif
