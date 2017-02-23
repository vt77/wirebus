#ifndef WIREBUSTIMER_H
#define WIREBUSTIMER_H

#define         TIMER_PERIOD_QUARTER    300
#define         TIMER_PERIOD_HALF       600
#define         TIMER_PERIOD_SECOND     1200 

#include <stdint.h>


uint16_t	wirebus_time_elapsed;
typedef uint8_t (*timer_func)(uint8_t);

typedef struct{
	uint8_t period;
	uint8_t curr_time;
	timer_func func;
	uint8_t param;
}wirebus_timer;

wirebus_timer * timer_ptr;

uint8_t inline timer_setup(wirebus_timer* timer,uint8_t period,timer_func f, uint8_t param )
{
	timer_ptr = timer;
	timer_ptr->func = f;
	timer_ptr->param = param;		
	timer_ptr->period = period;
	timer_ptr->curr_time = 0;
	return 0;
}


void wirebus_timer_handler()
{

}

#endif
