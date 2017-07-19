#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>


// Fa una crida a walk_forward() cada vegada que passa pel wait_ready, mirant primer si hi ha alguna acció executant-se. Moviment continuat.


typedef enum {wait_start, wait_ready, walk, stop} main_states;

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
                       state = wait_ready;
                     }
                     else
                       state = wait_start;
                     break;

    case wait_ready: if(is_action_running())
		    {
		      state = wait_ready;
		    }
                    else
		    {
		       walk_forward();
		       state = walk;
		    }
		    break;
		    
/*    case wait_stop: if(is_action_running()) //Parada suau (?)
		    {
		      state = wait_stop;
		    }
		    else
		    {
		      mtn_lib_stop_mtn();
		      state = stop;
		    }
		    break;
*/		      
    case walk: if (is_button_rising_edge(BTN_DOWN))
               { 
		  //state = wait_stop;
		  mtn_lib_stop_mtn();
		  state = stop;
               }
               else
	       {
		 state = wait_ready;
	       }
	       break;
		    
    case stop: if(is_button_rising_edge(BTN_UP))
		{
		  state = wait_ready;
		}
		break;
		
    
		
  }
}
