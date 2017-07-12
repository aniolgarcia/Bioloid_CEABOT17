#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start, do_nothing} main_states;


void user_init(void)
{
  mtn_lib_init();
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);

  exp_compass_start();
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
		               state=do_nothing;
		             }
		             else
			     {
		               state=wait_start;
			     }
		             break;
			     
	    case do_nothing: state = do_nothing;
			     break;
	  }        
}  
	   