#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Base del programa de sumo

typedef enum {wait_start, wait_ready, gir_dreta, gir_esquerra} main_states;
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;


#define err 100 



//Funció per calcular el desviament del robot respecte l'orientació inicial. És com compass però amb la posició inicial parametritzada.




int bno055_correction(int value)
{
	if(value > -240)
	{
	  return value -240;
	}
	else
	{
	  return value;
	}
}


uint8_t gira(int angle){
	static turn_states s = t_init;
	static int comp_ini = 0;
	static int comp_end = 0;
	int done = 0;
	
	switch (s){
		case t_init:
			comp_ini = bno055_correction(exp_bno055_get_heading());
			comp_end = suma_angles (comp_ini,angle*10);
			
			s=t_middle;
			break;
		case t_middle:
			if (abs (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end))>err){
			// ("diff = %d\n",compass_param (bno055_correction(exp_bno055_get_heading()),comp_end));
				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)>err){
					s =t_right;
				}
				else if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)<-err){
					s=t_left;
				}
			}
			else {
				s = t_wait_end;		
			}
			break;
		case t_right:
			if (turn_right()){
				s = t_wait_end;
			}
			else {
				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)<err){ 
						mtn_lib_stop_mtn();
				}
				else s = t_right;
				
			}
			break;
			
		case t_left:
			if (turn_left()){
				s = t_wait_end;
			}
			else {
				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)>-err){
						mtn_lib_stop_mtn();
				}
				else s = t_left;
				
			}
			break;
		case t_wait_end:
			done =0x01;
			s = t_init;
			break;
	
	}
	return done;
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
  exp_bno055_start();
  if(is_button_pressed(BTN_UP))
  {
    exp_bno055_erase_calibration();
  }
  while(exp_bno055_is_calibrated()!=0x01)
  {
    _delay_ms(100);
  }
}



static int valor_base;
user_time_set_period(5000);
void user_loop(void)
{
  static main_states state=wait_start;


  switch(state)
  {
    case wait_start: if(user_time_is_period_done())
		      {
			
		      }
		     if(is_button_rising_edge(BTN_START))
                     {
		       valor_base = bno055_correction(exp_bno055_get_heading());
                       action_set_page(31);
                       action_start_page();
                       state=wait_ready;
                     }
                     else
                       state=wait_start;
                     break;fall_state = balance_robot_has_fallen();
		     
    case wait_ready: if(is_button_rising_edge(BTN_RIGHT)){
			state = gir_dreta;
		      }
		     if (is_button_rising_edge(BTN_LEFT))
		        state = gir_esquerra;
		      /*if(is_action_running())
		     {
		       state=wait_ready;
		     }
                     else
		     {
		       state=gir;
		     }*/
                     break;
		     
    case gir_dreta: 
		  if (gira(45)==0x01){
		    state = wait_ready;
	       }
	       break;
	       
	       
    case gir_esquerra: 
		  if (gira(-45)==0x01){
		  state = wait_ready;
	       }
	       break;
	       
  }
}



