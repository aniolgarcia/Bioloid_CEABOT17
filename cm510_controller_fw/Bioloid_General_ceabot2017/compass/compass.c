#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
//#include "mtn_library.h"
//#include <stdlib.h>

typedef enum {wait_start,wait_ready, read_sensors} main_states;
int valor_base;

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
	
	return desviament;
}


int compass_correction(int value, int upper_end, int bottom_end)
{
	return (((upper_end - bottom_end)/3600)*value)+bottom_end;
}


void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  //mtn_lib_init();
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
                       //action_set_page(31);
                       //action_start_page();
                       state=wait_ready;
                     }
                     else
                       state=wait_start;
                     break;
    case wait_ready: if(is_action_running())
                       state=wait_ready;
                     else
                       state=read_sensors;
                     break;
    case read_sensors: if(is_button_rising_edge(BTN_UP))
			{
			  cm510_printf("Exp. Board compass: %d\n",exp_compass_get_avg_heading());
			  cm510_printf("Exp. Board ADC port 7: %d\n",exp_adc_get_avg_channel(ADC7));
			  //user_time_set_period(2000);
			}
			
                       break;
  }
	
	/*switch(state)
	  {
	    case wait_start: if(is_button_rising_edge(BTN_START))
		             {
			       valor_base = exp_compass_get_avg_heading();
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
			      state= read_compass;	
		             break;
	}*/
}
