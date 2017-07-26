#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa per comprovar la brúixola. Al engegar fixa la posició i quan algú el desvia d'aquesta posició, hi torna automàticament. 


typedef enum {wait_start,wait_ready, torna} main_states;

int valor_base;

int compass(int valor_base)
{
        
	int nou_valor, desviament;
	
	nou_valor = exp_compass_get_avg_heading();
	desviament = nou_valor - valor_base;
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
		               action_set_page(31);
		               action_start_page();
		               state=wait_ready;
		             }
		             else
			     {
		               state=wait_start;
			     }
		             break;
	    case wait_ready: if(is_action_running())  
			     {
			       state=wait_ready;
			       cm510_printf("Action running");
			     }
		             else
		             {
		               state= torna; 
		             }
		             break;
	    case torna: cm510_printf("Dins torna"); 
	      if(compass(valor_base) > 5)
			{
			  turn_left();
			  state = wait_ready;
			}
			else if(compass(valor_base) < -5)
			{
			  turn_right();
			  state = wait_ready;
			}
			else
			{
			  state = wait_ready;
			}
			break;
	  }
} 
 