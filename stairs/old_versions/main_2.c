#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include "action_id.h"
typedef enum {wait_start,wait_ready,walk_to_stairs,up_stairs1,get_up,down_stairs2} main_states;
typedef enum {first_step,second_step,wait_end} ws_states;
// Walks up the stairs using "Forward-Start + Forward-End" to get to the stairs
#define left_foot_forward 				GPIO1	
#define left_foot_forward_down		GPIO2
#define left_foot_lateral_down		GPIO3
#define right_foot_forward				GPIO18
#define right_foot_forward_down		GPIO17
#define right_foot_lateral_down		GPIO16


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
uint8_t walk_stairs (int x){
	if (x==0) return walk_stairs_SL();
	else return walk_stairs_SR();
}
int compass_diff (int ini, int actual){ //Calculates the difference between initial orientation and actual orientation of compass.
	short int inc = actual - ini;
	if (inc<-1800) inc+=3600;
	else if (inc>1800) inc-=3600;
	return inc;
} //return value is between -1800 and 1800


void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
  exp_board_init(192);
  exp_adc_start();
  exp_compass_start();
}

void user_loop(void)
{
  static main_states state=wait_start;
  static fallen_t fall_state=robot_standing;
  static int comp_ini=0;
  static int x=0;
  static int diff = 0;
  printf ("state=%d\n",state);
   switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
                     {
                       action_set_page(31);
                       action_start_page();
                       state=wait_ready;
                     }
                     else
                       state=wait_start;
                     break;
    case wait_ready: if(is_action_running())
                       state=wait_ready;
                     else
                     {
                     	 comp_ini = exp_compass_get_avg_heading();
                       state=walk_to_stairs; 
                     }
                     break;
    case walk_to_stairs: 
    	/*if (is_button_rising_edge (BTN_RIGHT)){
    		state = end_walking;
    	}
    	else {*/
				fall_state=balance_robot_has_fallen();
				if(fall_state!=robot_standing) {
					state=get_up;
				}
				else {
					
						if (walk_stairs(x)) {
				   		if (x==0) x=1;
				   		else x=0;
				   		diff = compass_diff (comp_ini, exp_compass_get_avg_heading());
				   		if (diff > 150) x = 0;
				   		else if (diff < 150) x = 1; 
		     		}		
		     		else state = walk_to_stairs;	
					  printf ("%d,%d,%d,%d,%d,%d\n, ", exp_gpio_get_value(left_foot_forward),exp_gpio_get_value(left_foot_forward_down),exp_gpio_get_value(left_foot_lateral_down),exp_gpio_get_value(right_foot_forward),exp_gpio_get_value(right_foot_forward_down),exp_gpio_get_value(right_foot_lateral_down));
		//si detectamos con alguno de los dos sensores delanteros que hay que subir una escalon
						if((!exp_gpio_get_value(left_foot_forward) || !exp_gpio_get_value(right_foot_forward))&&(!exp_gpio_get_value(left_foot_forward_down) && !exp_gpio_get_value(right_foot_forward_down))){  
							printf ("J"); 
				    	if (!exp_gpio_get_value(right_foot_forward) && exp_gpio_get_value(left_foot_forward)) state = walk_to_stairs;
				    	else {
				      	printf ("up\n");
				      	state=up_stairs1;
				  		}
						}
						else {
							if ((exp_gpio_get_value(left_foot_forward_down) && exp_gpio_get_value(right_foot_forward_down)) && (!exp_gpio_get_value(left_foot_lateral_down) && !exp_gpio_get_value(right_foot_lateral_down))){
								state = down_stairs2;
							}
							else state=walk_to_stairs; 
		
						}
		     }
       //}
       		
       break;
    case get_up: 
		  if(get_up_process(fall_state)==0x01) state=wait_ready;
		  else state=get_up;
		  break;

    case up_stairs1: 
      if(stairs_up_process(fall_state)){
    				printf ("end up stairs\n");
    				state=walk_to_stairs;
    	}
      else {
        fall_state=balance_robot_has_fallen();
			  if(fall_state!=robot_standing) state=get_up;
        else state=up_stairs1; 
      }
      break;
      
  case down_stairs2 : 
    if (stairs_down_process()) state = walk_to_stairs;
    else state = down_stairs2;		
  }
}

