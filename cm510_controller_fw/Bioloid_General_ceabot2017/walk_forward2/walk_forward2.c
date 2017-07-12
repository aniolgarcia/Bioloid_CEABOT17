#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

  //Fa una crida a walk_forward cada vegada que passa per walk, sense comprovar si hi ha una acció executant-se cada vegad a.


typedef enum {wait_start, wait_ready, walk, stop} main_states;

void user_init(void)
{ 
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
//   balance_set_offset(11, -20); //intents de definir offsets fora de balance.c. Encara no està implementada la funció.
//   balance_set_offset(12, 20); 
}

 
void user_loop(void) 
{
  static main_states state= wait_start;
  
    
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
		      state = walk;
		    }
		    break;
		    
    case walk: if(is_button_rising_edge(BTN_DOWN))
               {
		  mtn_lib_stop_mtn();

		  state = stop;
               }
               else
	       {
 		 walk_forward();
		 state = walk;
	       }

	       break;
		    
    case stop: if(is_button_rising_edge(BTN_UP))
		{
		  state = wait_ready;
		}
		break;

  }
}
