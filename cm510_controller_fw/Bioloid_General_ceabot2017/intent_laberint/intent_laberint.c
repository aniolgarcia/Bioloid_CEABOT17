#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start, wait_ready, wait_cmd, walk_l, walk_r} main_states;

int cont = 0;

void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
  cont = 0;
}



void user_loop(void)
{
  static main_states state=wait_start;
  int adc7;
  cont = 0;

	
  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
		      {
		       //valor_base = exp_compass_get_avg_heading();
		       action_set_page(31);
		       action_start_page();
		       state=wait_ready;
		       cont = 0;
		      }
		      else
		      {
			state=wait_start;
		      }
		      break;
		      
    case wait_ready: if(is_action_running())
		      {
			state = wait_ready;
		      }
		      else
		      {
			state = wait_cmd;
		      }
		      break;

		    
    case wait_cmd: if(is_button_rising_edge(BTN_LEFT))
		    {
		      state = walk_l;
		    }
		    else if(is_button_rising_edge(BTN_RIGHT))
		    {
		      state = walk_r; 
		    }
		    else if(is_button_rising_edge(BTN_UP))
		    {
		      adc7 = exp_adc_get_avg_channel(ADC7);
		      cm510_printf("Sensor IR adc7: %d", adc7); 
		      user_time_set_period(500);
		      state = wait_ready;
		    }
		    else if (is_button_rising_edge(BTN_DOWN))
                    {
		      //walk_forward();
                      mtn_lib_stop_mtn();
		      //printf("fdsgndnht");
		      state= wait_ready; 
                    }
                    else
		    {
		      state = wait_ready;
		    }
		    break;
		    
    case walk_l:    cm510_printf("Inici walk_l %d", cont);
		    if(cont < 5)
		    {
		      walk_left();
		      user_time_set_period(1000);		      
		      cont = cont + 1;
		      state = walk_l;
		      cm510_printf("dins walk_l %d", cont);
		        
		    }
		    else
		    {
		      cont = 0;
		      mtn_lib_stop_mtn();
		      state = wait_cmd;		      
		    }
		    break;
		    
   case walk_r: if(cont < 5)
		    {
		      walk_right();
		      cont = cont + 1;
		      state = walk_r;
		        
		    }
		    else
		    {
		      cont = 0;
		      mtn_lib_stop_mtn();
		      state = wait_cmd;		      
		    }
		    break;
  }
}
		      