#include "small_steps.h"


typedef enum {first_step,second_step,wait_end} ws_states;
uint8_t walk_stairs_SL(void){
	int done =0x00;
	static ws_states s = first_step;
	switch (s){
		case first_step:
			if (is_action_running()) s = first_step;
			else {
				action_set_page (F_S_L);
				action_start_page();
				s = second_step;
			}
			break;
		case second_step:
			if (is_action_running()) s= second_step;
			else {
				action_set_page (F_E_R);
				action_start_page();
				s=wait_end;
			}
			break;
		case wait_end:
			if (is_action_running()) s=wait_end;
			else {
				done = 0x01;
				s = first_step;
			}
		
	}
	return done;

}
uint8_t walk_stairs_SR(void){
	int done =0x00;
	static ws_states s = first_step;
	switch (s){
		case first_step:
			if (is_action_running()) s = first_step;
			else {
				action_set_page (F_S_R);
				action_start_page();
				s = second_step;
			}
			break;
		case second_step:
			if (is_action_running()) s= second_step;
			else {
				action_set_page (F_E_L);
				action_start_page();
				s=wait_end;
			}
			break;
		case wait_end:
			if (is_action_running()) s=wait_end;
			else {
				done = 0x01;
				s = first_step;
			}
		
	}
	return done;

}
 uint8_t walk_back_SL(void){
	int done =0x00;
	static ws_states s = first_step;
	switch (s){
		case first_step:
			if (is_action_running()) s = first_step;
			else {
				action_set_page (B_S_L);
				action_start_page();
				s = second_step;
			}
			break;
		case second_step:
			if (is_action_running()) s= second_step;
			else {
				action_set_page (B_E_R);
				action_start_page();
				s=wait_end;
			}
			break;
		case wait_end:
			if (is_action_running()) s=wait_end;
			else {
				done = 0x01;
				s = first_step;
			}
		
	}
	return done;

}
uint8_t walk_back_SR(void){
	int done =0x00;
	static ws_states s = first_step;
	switch (s){
		case first_step:
			if (is_action_running()) s = first_step;
			else {
				action_set_page (B_S_R);
				action_start_page();
				s = second_step;
			}
			break;
		case second_step:
			if (is_action_running()) s= second_step;
			else {
				action_set_page (B_E_L);
				action_start_page();
				s=wait_end;
			}
			break;
		case wait_end:
			if (is_action_running()) s=wait_end;
			else {
				done = 0x01;
				s = first_step;
			}
		
	}
	return done;

}
uint8_t walk_stairs (int x){
	if (x==0) return walk_stairs_SL();
	else return walk_stairs_SR();
}

uint8_t walk_back (int x){
	if (x==0) return walk_back_SL();
	else return walk_back_SR();
}
uint8_t walk_left_SL(void){
	int done =0x00;
	static ws_states s = first_step;
	switch (s){
		case first_step:
			if (is_action_running()) s = first_step;
			else {
				action_set_page (L_S_L);
				action_start_page();
				s = second_step;
			}
			break;
		case second_step:
			if (is_action_running()) s= second_step;
			else {
				action_set_page (L_E_R);
				action_start_page();
				s=wait_end;
			}
			break;
		case wait_end:
			if (is_action_running()) s=wait_end;
			else {
				done = 0x01;
				s = first_step;
			}
		
	}
	return done;
}
uint8_t walk_right_SR(void){
	int done =0x00;
	static ws_states s = first_step;
	switch (s){
		case first_step:
			if (is_action_running()) s = first_step;
			else {
				action_set_page (R_S_R);
				action_start_page();
				s = second_step;
			}
			break;
		case second_step:
			if (is_action_running()) s= second_step;
			else {
				action_set_page (R_E_L);
				action_start_page();
				s=wait_end;
			}
			break;
		case wait_end:
			if (is_action_running()) s=wait_end;
			else {
				done = 0x01;
				s = first_step;
			}
		
	}
	return done;
}

uint8_t turn_right_SR (void){
	int done =0x00;
	static ws_states s = first_step;
	switch (s){
		case first_step:
			if (is_action_running()) s = first_step;
			else {
				action_set_page (RT_S_R);
				action_start_page();
				s = second_step;
			}
			break;
		case second_step:
			if (is_action_running()) s= second_step;
			else {
				action_set_page (RT_E_L);
				action_start_page();
				s=wait_end;
			}
			break;
		case wait_end:
			if (is_action_running()) s=wait_end;
			else {
				done = 0x01;
				s = first_step;
			}
		
	}
	return done;
}
uint8_t turn_left_SL (void){
	int done =0x00;
	static ws_states s = first_step;
	switch (s){
		case first_step:
			if (is_action_running()) s = first_step;
			else {
				action_set_page (LT_S_L);
				action_start_page();
				s = second_step;
			}
			break;
		case second_step:
			if (is_action_running()) s= second_step;
			else {
				action_set_page (LT_E_R);
				action_start_page();
				s=wait_end;
			}
			break;
		case wait_end:
			if (is_action_running()) s=wait_end;
			else {
				done = 0x01;
				s = first_step;
			}
		
	}
	return done;
}
