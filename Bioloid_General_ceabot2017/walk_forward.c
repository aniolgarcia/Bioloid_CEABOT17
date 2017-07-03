#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start,wait_walk_ready,walk_forward} main_states;

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
			state=walk_forward;
			break;

    case walk_forward: if (is_button_rising_edge(BTN_UP))
                     {
                       ‭walk_forward();
                     }
		     
		     if (is_button_rising_edge(BTN_DOWN))
                       {
                       mtn_lib_stop();   
		       state= wait_ready; 
                       }
                   break;


  }
}
