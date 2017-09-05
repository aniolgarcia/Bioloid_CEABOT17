#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include "action_id.h"
#include "stairs.h"

// Walks up the stairs using "walk forward compensating" to get to the stairs
typedef enum {wait_start,wait_ready,walk_to_stairs,wait_stop1,up_stairs1,get_up,wait_stop2,down_stairs2} main_states;
#define left_foot_forward 				GPIO1	
#define left_foot_forward_down		GPIO2
#define left_foot_lateral_down		GPIO3
#define right_foot_forward				GPIO18
#define right_foot_forward_down		GPIO17
#define right_foot_lateral_down		GPIO16
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
  static int comp_ini = 0;
  
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
                       walk_forward_compensating(comp_ini,exp_compass_get_avg_heading());
                       state=walk_to_stairs; 
                     }
                     break;
    case walk_to_stairs: 
		  fall_state=balance_robot_has_fallen();
			if(fall_state!=robot_standing) {
				get_up_process (fall_state);
				state=get_up;
			}
			else {
			    printf ("%d,%d,%d,%d,%d,%d\n, ", exp_gpio_get_value(left_foot_forward),exp_gpio_get_value(left_foot_forward_down),exp_gpio_get_value(left_foot_lateral_down),exp_gpio_get_value(right_foot_forward),exp_gpio_get_value(right_foot_forward_down),exp_gpio_get_value(right_foot_lateral_down));
			      //si detectamos con alguno de los dos sensores delanteros que hay que subir una escalon
	    			if((exp_gpio_get_value(left_foot_forward) || exp_gpio_get_value(right_foot_forward))&&(exp_gpio_get_value(left_foot_forward_down) || exp_gpio_get_value(right_foot_forward_down))){   
				          if (exp_gpio_get_value(right_foot_forward) && !exp_gpio_get_value(left_foot_forward)) state = walk_to_stairs;
				          else {
				            printf ("up\n");
				            mtn_lib_stop_mtn();
				            state=wait_stop1;
			          }
		        }
/*		        else {
		          if ((!exp_gpio_get_value(left_foot_forward_down) || !exp_gpio_get_value(right_foot_forward_down)) && (exp_gpio_get_value(left_foot_lateral_down) && exp_gpio_get_value(right_foot_lateral_down))){
		            mtn_lib_stop_mtn();
		            state = wait_stop2;
		          }
		          else state=walk_to_stairs; 
		        
		        }*/
		        walk_forward_compensating(comp_ini,exp_compass_get_avg_heading());
       }
       break;
    case wait_stop1:
    
     if(walk_forward_compensating(comp_ini,exp_compass_get_avg_heading()))
                       state=up_stairs1;
                     else
                       state=wait_stop1;
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
  
  case get_up: 
		  if(get_up_process(fall_state)==0x01) state=wait_ready;
		  else state=get_up;
		  break;
  case wait_stop2: if(walk_forward_compensating(comp_ini,exp_compass_get_avg_heading()))
                       state=down_stairs2;
                     else
                       state=wait_stop2;
                     break;
  case down_stairs2 : 
    if (stairs_down_process()) state = walk_to_stairs;
    else state = down_stairs2;			
  }
  
}

