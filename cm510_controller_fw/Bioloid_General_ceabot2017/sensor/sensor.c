#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start,wait_ready,read_sensor} main_states;

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
                       state=wait_ready;
		       valor_base = exp_compass_get_avg_heading();
                     }
                     else
		     {
                       state=wait_start;
		     }
                     break;
		     
    case wait_ready:  if(is_action_running())
		      {
			state=wait_ready;
		      }
                      else
		      {
			state = read_sensor;
		      }
                     break;
		     
    case read_sensor: if(user_time_is_period_done())
		      {
//  			cm510_printf("Exp. Board ADC port 7: %d\n",exp_adc_get_avg_channel(ADC7));
//  			cm510_printf("CM510 ADC port 1: %d\n",get_adc_avg_channel(ADC_PORT_2));
//  			cm510_printf("Exp. Board compass: %d\n",exp_compass_get_avg_heading() );
//  			cm510_printf("Desviament: %d\n",compass(valor_base));
  			cm510_printf("GYRO X: %d  ", get_adc_channel(BALANCE_GYRO_X_CHANNEL));
 			cm510_printf("GYRO Y: %d\n", get_adc_channel(BALANCE_GYRO_Y_CHANNEL));
			user_time_set_period(500);
		      }
                      break;
  }
}
