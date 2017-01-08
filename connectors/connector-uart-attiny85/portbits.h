#ifndef _PORTBITS_H


#define INPUT   0
#define OUTPUT  1

#define BIT(a) (1 << a)
#define portb_get(a)         (PINB  & (1 << a))
#define portb_set(a)         (PORTB |= (1 << a))
#define portb_clear(a)       (PORTB &= ~(1 << a))
#define portb_toggle(a)      (PORTB ^= (1 << a))
#define portb_write(a,b)     ( b ? portb_set(a) : portb_clear(a) )
#define portb_input(a)       DDRB &= ~(1 << a)
#define portb_output(a)      DDRB |= (1 << a)
#define portb_config(a,b)    ( b ? ( portb_output(a) ): ( portb_input(a) ))
#endif
