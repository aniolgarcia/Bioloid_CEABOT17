#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Intent inical de l'estructura del laberint. Ara mateix només funcionen els estats wait_start, wait_ready i walk_l, i el que fa és caminar lateralment fins a trobar un objecte amb el sensor IR. Els altres estats tan sols s'han utilitzat per fer proves i no sempre reflecteixen l'estructura correcta.

typedef enum {wait_start, wait_ready, wait_cmd, walk_l, walk_r, stop} main_states;

int cont = 0;

void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
  exp_adc_start();
  exp_compass_start();
}



void user_loop(void)
{
  static main_states state=wait_start;
  int adc7;
	
  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
		      {
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
			state = walk_l; //Ara l'enviem a walk_l perquè només volem que tiri cap a l'esquerra.
		      }
		      break;

// Estat utilitzat per fer proves de girs, molt probablement no estarà al programa final
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
		      state= wait_ready; 
                    }
                    else
		    {
		      state = wait_ready;
		    }
		    break;
		    
    case walk_l:    if(exp_adc_get_avg_channel(ADC7) < 450 && !is_button_rising_edge(BTN_DOWN))
		    {
		      walk_left();
		      state = walk_l;
		    }
		    else
		    {
		      mtn_lib_stop_mtn();
		      state = stop;		      
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
		    
    case stop: if(is_button_rising_edge(BTN_UP))
		{
		  state = wait_ready;
		}
		else
		{
		  state = stop; 
		}
		break;
  }
}
		      