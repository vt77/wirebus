/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <daniel@vt77.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Daniel Marchasin
 * ----------------------------------------------------------------------------
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/atomic.h>
#include "uart.h"
#include "portbits.h"

#define         UART_MARK               0
#define         UART_SPACE              1

// Prescaler 8
#define         UART_PRESCALER  (1<<CS12)

//BIT_DELAY = (CPU_CLOCK / UART_PRESCALER ) / BAUD_RATE
#define BIT_DELAY       ( (8000000 / 8) / 9600 )


#define RX_CNTR         OCR1B
#define TX_CNTR         OCR1A


#define ENABLE_ONCHANGE_INTERRUPT()     PCMSK   |= (1 <<  PCINT0)
#define DISABLE_ONCHANGE_INTERRUPT()    PCMSK   &= ~(1 << PCINT0)

#define ONCHANGE_INTERRUPT              PCINT0_vect
#define TIMER_RX_INTERRUPT              TIM1_COMPB_vect
#define TIMER_TX_INTERRUPT              TIM1_COMPA_vect

#define ENABLE_RX_INTERRUPT()           TIMSK |=  (1 << OCIE1B )
#define DISABLE_RX_INTERRUPT()          TIMSK &= ~(1 << OCIE1B )


volatile uint8_t      rxbyte            = 0;
volatile uint16_t     txbyte            = 0;
volatile uint8_t rx_complete   			= 1;


uint8_t			rxbitcount = 0;


uint8_t         rx_buffer[16];
uint8_t         rx_head  = 0;
uint8_t         rx_tail  = 0;

uint8_t         tx_buffer[16];
uint8_t         tx_head  = 0;
uint8_t         tx_tail  = 0;

uint8_t         tx_timer   = 0;


void uart_init()
{


		portb_input(UART_RX_PIN);
		portb_output(UART_TX_PIN);

		//Pull up line
		portb_set(UART_TX_PIN);

		PCMSK	=  0;
        GIMSK   |= (1 << PCIE);
        TIMSK   |= (1 << OCIE1A);
        TCCR1   =  UART_PRESCALER | (1<<CTC1);
        OCR1A   =  BIT_DELAY;
        OCR1C   =  BIT_DELAY;

}

void uart_start_receiver(){
        ENABLE_ONCHANGE_INTERRUPT();
}

void uart_stop_receiver()
{
        DISABLE_ONCHANGE_INTERRUPT();
}

int uart_recieve(uint8_t *b){
        
		uint8_t ret = 0;
		
		if( rx_head != rx_tail )
        {
                ret  = rx_buffer[rx_head];
                rx_head = ( ( rx_head + 1 ) & 0x0F );
                return ret;
        }

        return ret;
}


void uart_send(uint8_t b){

/*
        //Wait until previous byte sent
		        uint8_t active;
	    do
        {
               	ATOMIC_BLOCK(ATOMIC_FORCEON)
                {
                        active = ( txbyte != 0 ) ? 1 : 0;
                }

        }while(active);
		
        // Prepare data to send : StopBit(1) + data + StartBit(0) = 10 bits
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
                txbyte = 0x200 | ( b << 1 );
        }
*/

        tx_buffer[tx_tail] = b;
        tx_tail = ( ( tx_tail + 1 ) & 0x0F );
}


volatile static uint8_t stop_bit   			= 0;

ISR(TIMER_RX_INTERRUPT)
{

	if( rxbitcount )
	{
			rxbyte >>= 0x01;
			if( portb_get(UART_RX_PIN) == UART_SPACE )
									rxbyte |= 0x80;
			
			rxbitcount--;			

	}else if( rxbitcount == 0 )
	{
			
			DISABLE_RX_INTERRUPT();
        	
			rx_buffer[rx_tail] = rxbyte;
        	rx_tail = ( ( rx_tail + 1 ) & 0x0F );

			rxbyte = 0;

			GIFR |= (1<< PCIF);
			GIMSK |=  (1<<PCIE);
			ENABLE_ONCHANGE_INTERRUPT();
	}

}

ISR(TIMER_TX_INTERRUPT)
{


        if(txbyte)
        {
                portb_write(UART_TX_PIN, (txbyte & 0x01));
                txbyte >>= 0x01;
        }else{
			if( tx_head != tx_tail )
			{
				txbyte = 0x200 | ( tx_buffer[tx_head] << 1 );
				tx_head = ( ( tx_head + 1 ) & 0x0F );
			}		
		}

}

ISR(ONCHANGE_INTERRUPT)
{

		PORTB ^= (1<<2);
		
		if( portb_get(UART_RX_PIN) == UART_MARK )
		{
			rxbyte = 0x0;
			rxbitcount = 9;	
            tx_timer  = ( TCNT1 > ( BIT_DELAY / 2 ) ? TCNT1 - ( BIT_DELAY / 2 ) + 1 : TCNT1 + ( BIT_DELAY / 2 ) ) - 1;

			if( tx_timer < 5 )   tx_timer = 5;
			if( tx_timer > 100 ) tx_timer = 100;
			 
			RX_CNTR = tx_timer;
			TIFR |= (1<<OCF1B); //Clear interrupt flag 
		    DISABLE_ONCHANGE_INTERRUPT();
			ENABLE_RX_INTERRUPT();			
		}
}

