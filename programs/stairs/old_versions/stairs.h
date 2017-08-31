
#include "cm510.h"
#include "balance.h"
#include "action_id.h"

#ifndef STAIRS_H
#define STAIRS_H

uint8_t stairs_up_process(fallen_t fall_state);
uint8_t stairs_down_process(void);
uint8_t get_up_process(fallen_t fall_state);

#endif
