#ifndef _MTN_LIBRARY_H
#define _MTN_LIBRARY_H

#include "balance.h"
typedef enum {left_foot,right_foot} foot_t;

void mtn_lib_init(void);
void mtn_lib_stop_mtn(void);
void mtn_lib_set_start_foot(foot_t foot);

void reset_steps(void);
int get_steps (void);

uint8_t walk_forward(void);
uint8_t walk_backward(void);
uint8_t turn_left(void);
uint8_t turn_right(void);
uint8_t walk_left(void);
uint8_t walk_right(void);
uint8_t walk_forward_turn_left(void);
uint8_t walk_forward_turn_right(void);
uint8_t walk_backward_turn_left(void);
uint8_t walk_backward_turn_right(void);
uint8_t walk_forward_left(void);
uint8_t walk_forward_right(void);
uint8_t walk_backward_left(void);
uint8_t walk_backward_right(void);

uint8_t fast_walk_forward(void);
uint8_t fast_walk_backward(void);

uint8_t fast_turn_left(void);
uint8_t fast_turn_right(void);
uint8_t fast_walk_left(void);
uint8_t fast_walk_right(void);
uint8_t fast_walk_forward_turn_left(void);
uint8_t fast_walk_forward_turn_right(void);
uint8_t fast_walk_backward_turn_left(void);
uint8_t fast_walk_backward_turn_right(void);


uint8_t walk_forward_compensating(int comp_ini, int current);
uint8_t walk_backward_compensating(int comp_ini, int current);
uint8_t get_up_process(fallen_t fall_state);
uint8_t stairs_up_process (fallen_t fall_state);
uint8_t stairs_down_process(void);
uint8_t walk_left_compensating (int comp_ini, int comp_act);
uint8_t walk_right_compensating (int comp_ini, int comp_act);
uint8_t turn_angle (int angle);
//uint8_t fast_walk_forward_compensating(int comp_ini, int current);
#endif
