
;#include <avr/io.h>
;#include "transport.h"
.include "tn13Adef.inc"


#define		WIRE_PIN	1

.global sendStart
.global sendByte
.global onByteReceived


#define counter 		r19
#define sreg_save		r2
#define tmp				r16
#define state			r18
#define receive_buffer 	r17

#define		SET_SPACE	sbi		0x18, WIRE_PIN
#define		SET_MARK	cbi		0x18, WIRE_PIN
.macro DELAY p
	ldi	tmp,\p
	rcall delayPeriods
.endm


#define		SM_SPACE			0
#define		SM_MARK				1
#define		SM_STATE_IDLE		0
#define		SM_STATE_READING	2

#define		MIN_PREAMBULE_LENGTH	7



delayPeriods:
	//Reset timer
	out	0x32,r1  //TCNT0
	and counter,r1
loop:
	cp	tmp, counter
	brne loop
	ret

//enum  wirebusErrorCode sendStart()
sendStart:

//		ldi	   r24,1 		
//		cpse   state,r0		//Check state
//		ret					//return SEND_ABORT

		push    tmp

		SET_MARK				//Set Mark
		DELAY(8)
		SET_SPACE
		DELAY(1)

		clr	 r24
		pop    tmp
		ret

//enum  wirebusErrorCode sendByte(uint8_t byte)
sendByte:
		push	r25
		push    tmp

		ldi		r25, 0x8
nextbit:
		SET_MARK
		DELAY(1)	
		sbrs	r24,7
		rjmp	zerodelay
		DELAY(2)
zerodelay:
		SET_SPACE
		DELAY(1)
		add	r24, r24	//data <<= 1; 
		dec	r25	
		brne  nextbit
		
		pop	 tmp	
		pop  r25
		ldi	 r24,0
		ret

.global TIM0_COMPA_vect
TIM0_COMPA_vect:
	in	sreg_save, _SFR_IO_ADDR(SREG)
	inc	counter
	//clr	tmp
	out	_SFR_IO_ADDR(SREG), sreg_save
	reti


.global  INT0_vect
INT0_vect:


/*
		FSM States:
		SM_STATE_IDLE 	 & SM_SPACE		=	0
		SM_STATE_IDLE 	 & SM_MARK		=	1
		SM_STATE_READING & SM_SPACE		=	2
		SM_STATE_READING & SM_MARK		=	3
*/	

		cli	
		push	r30
		push	r31

		cbr		state,0
		sbic	0x16,1			//PINB
		inc 	state

		ldi r30, pm_lo8( FUNCTIONSTABLE )
		ldi r31, pm_hi8( FUNCTIONSTABLE )
		add r30, state
		ijmp

ENDOFISR:
		pop	r31
		pop	r30

		sei
		reti


FUNCTIONSTABLE:
			rjmp	SM_CHECK_PREAMBULE
			rjmp	SM_START_COUNTER
			rjmp	SM_PROCESS_BIT
			rjmp	SM_START_COUNTER


SM_START_COUNTER:
			out	0x32,r1  //TCNT0
			clr	counter		
			rjmp RETURN

SM_PROCESS_BIT:
			cbr		counter, 1			//Clear last bit, not relevant 
			cpse 	counter,r0			//Skip if zero (<2)
			sec
	received_zero:
			rol receive_buffer
			brcc RETURN
			rcall onByteReceived
			ldi	receive_buffer,1
			ldi  state, SM_STATE_READING
			rjmp RETURN
			
SM_CHECK_PREAMBULE: 
			cpi	counter, MIN_PREAMBULE_LENGTH		
			brlt RETURN	//Preambule too short
			ldi  state, SM_STATE_READING
			ldi	 receive_buffer,1
RETURN:
			rjmp ENDOFISR

