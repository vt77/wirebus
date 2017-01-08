#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
#include "wirebus.h"

#define         FSM_START               0
#define         FSM_STATE_SRC           1
#define         FSM_STATE_DST           2
#define         FSM_STATE_SIZE          3
#define         FSM_STATE_DATA          4
#define         FSM_IDLE                0xFE
#define         FSM_COMPLETE            0xFF


//All states above defined bellow will be ignored by fsm
#define         MAX_FSM_STATE           4

typedef struct wbfsm{
        uint8_t         state;
        uint8_t         lastchar;
        wirebus_packet  *p;
}wbfsm_t;

int   fsmprocess(char c, wbfsm_t * m);
void  proto_init();
void  reset_sm();
wirebus_packet  *  proto_check(char c);

#endif
