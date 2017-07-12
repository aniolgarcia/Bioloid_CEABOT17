#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start,wait_ready,move} main_states;

void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
}


void user_loop(void)
{
  static main_states state=wait_start;

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
			state=move;
		     }
			break;

    case move: if (is_button_rising_edge(BTN_UP))
                     {
                 walk_forward(); 
		 //walk_backward();
		 //turn_left();
		 //turn_right();
		 //walk_left();
		 //walk_right();
		 //walk_forward_turn_left();
		 //walk_forward_turn_right();
		 //walk_backward_turn_left();
		 //walk_backward_turn_right();
		 //walk_forward_left();
		 //walk_forward_right();
		 //walk_backward_left();
		 //walk_backward_right();

		 //fast_walk_forward();
		 //fast_walk_backward();

		 //fast_turn_left();
		 //fast_turn_right();
		 //fast_walk_left();
		 //fast_walk_right();
		 //fast_walk_forward_turn_left();
		 //fast_walk_forward_turn_right();
		 //fast_walk_backward_turn_left();
		 //fast_walk_backward_turn_right();


		 //walk_forward_compensating(int comp_ini, int current);
		 //walk_backward_compensating(int comp_ini, int current);
		 //get_up_process(fallen_t fall_state);
		 //stairs_up_process (fallen_t fall_state);
		 //stairs_down_process(void);
		 //walk_left_compensating (int comp_ini, int comp_act);
		 //walk_right_compensating (int comp_ini, int comp_act);
// 		 mtn_lib_stop_mtn();
		 state = wait_ready;
                   }
		     
		     if (is_button_rising_edge(BTN_DOWN))
                       {
                       walk_forward();   
		       state= wait_ready; 
                       }
                   break;


  }
}
