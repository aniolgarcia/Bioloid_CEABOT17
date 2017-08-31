
#include "cm510.h"
#include "balance.h"
#include "action_id.h"

#ifndef SMALL_STEPS_H
#define SMALL_STEPS_H

uint8_t walk_back(int x);
uint8_t walk_stairs(int x);
uint8_t walk_right_SR(void);
uint8_t walk_left_SL(void);
uint8_t turn_left_SL (void);
uint8_t turn_right_SR (void);
#endif
