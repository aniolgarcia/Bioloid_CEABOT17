#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Modificació del walk correction seguint un dels exemples del robot Dexter i utilitzant funcions que no acabem de saber ben bé què fan. No es recomana pujar-lo al robot per si de cas.

typedef enum {wait_start, wait_ready, walk, stop, correct_l, correct_r} main_states;

typedef uint8_t (*fnct_ptr)(void);
fnct_ptr fnct_l=turn_left;
fnct_ptr fnct_r=turn_right;


int valor_base, valor_actual;

int compass(int valor_base)
{
        
	int nou_valor, desviament;
	
	nou_valor = exp_compass_get_avg_heading();
	desviament = nou_valor - valor_base;
	//Com que el desviament no pot ser més de 180, si es passa és que ha passat de 360 o 0
	if(desviament > 1800)
	{
		desviament = desviament - 3600;
	}
	else if(desviament < -1800)
	{
		desviament = desviament + 3600;
	}
	
	desviament = desviament/10;
	
	return desviament;
}

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

  

  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
                     {
		       valor_base = exp_compass_get_avg_heading();
		       valor_actual = valor_base;
                       action_set_page(31);
                       action_start_page();
                       state = wait_ready;
                     }
                     else
                       state = wait_start;
                     break;

    case wait_ready: //valor_actual = exp_compass_get_avg_heading();
		    if(is_action_running())
		    {
		      state = wait_ready;
		    }
                    else
		    {
		      //walk_forward_compensating(valor_base, valor_actual);
		      //walk_forward();
		      state = walk;
		    }
		    break;
		    		     

    case walk: if (is_button_rising_edge(BTN_DOWN))
               {
		  mtn_lib_stop_mtn();
		  state = stop;
               }
               else if(compass(valor_base) > 20) 
	       {
			fnct_l();
 			state = correct_l;
	       }
	       else if(compass(valor_base) < -20)
	       {
			fnct_r();
			state = correct_r; 
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
		else
		{
		  state = stop;
		}
		break;
		
    case correct_l: if(fnct_l() == 0x00)
		    {
		      state = walk;
		    }
		    else
		    {
		      state = correct_l;
		    }

    case correct_r: if(fnct_r() == 0x00)
		    {
		      state = walk;
		    }
		    else
		    {
		      state = correct_r;
		    }
		  
	       /*if (compass(valor_base) > 150 )
               {
 		  turn_left();
// 		  walk_forward_turn_left();

		  state = wait_ready ;
               }
               else if(compass(valor_base) < -150)
	       {
 		 turn_right();
// 		 walk_forward_turn_right();
		 state = wait_ready;
	       }
               else
	       {
                 state = wait_ready;
	       }*/
	break;
    
		
  }
} 
