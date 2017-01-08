#ifndef __SOFT_UART_H
#define __SOFT_UART_H

#include <stdint.h>

//#define HALF_DUPLEX

#ifdef HALF_DUPLEX

#define UART_RX_PIN             1

#else

#define UART_RX_PIN             0
#define UART_TX_PIN             1

#endif


void uart_init();
int  uart_recieve(uint8_t *b);
void uart_send(uint8_t b);
void uart_start_receiver();
void uart_stop_receiver();

#endif
