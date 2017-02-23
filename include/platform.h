#ifndef __WIREBUS_PLATFORM_H
#define __WIREBUS_PLATFORM_H

#include  <stdint.h>

#ifdef __ARCH_AVR__
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <arch/avr.h>
#endif

#ifdef __ARCH_PIC__
#include <htc.h>
#include <arch/pic.h>
#endif

#ifdef __ARCH_STM__
#include <arch/stm.h>
#endif

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
#error "Unknown microcontroller. Please define microcontroleer by __MCU_XXXX"
#endif

#ifdef __GNUC__
#define NOINLINE __attribute__((noinline))
#define INLINE   __attribute__((always_inline))
#endif

#ifdef __XC8__
#define NOINLINE noinline
#define INLINE   inline
#endif


#endif
