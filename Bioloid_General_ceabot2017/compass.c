#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start,wait_walk_ready,read_compass} main_states;

int compass( int valor_base)
{
        
	int nou_valor, desviament;
	
	nou_valor = exp_compass_get_avg_heading();
	desviament = nou_valor - valor_base;
	if(desviament>1800){
		desviament = desviament - 3600;
	}else if (desviament<-1800){
		desviament = desviament + 3600;
	}
	
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
  int valor_base = exp_compass_get_avg_heading();
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
		               state= read_compass ; 
		             }
		             break;
	    case read_compass: 
			      cm510_printf("Desviacio de: %dº", compass(valor_base)); 
			      user_time_set_period(500); 
			      state=read_IRadc7;	
		             break;
	}
}
