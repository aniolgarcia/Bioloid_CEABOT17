#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start, wait_ready, decode, move, move_l, move_r} main_states;

int valor_base, valor_actual, target_angle = 0;

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
	
	//desviament = desviament/10;
	
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
    case wait_start:
      
      if(is_button_rising_edge(BTN_START))
      {
	valor_base = exp_compass_get_avg_heading();	
        action_set_page(31);
        action_start_page();
        state=wait_ready;
       }
       else
	 state=wait_start;
       break;
       
    case wait_ready:
      
      if(is_action_running())
      {
	state=wait_ready;
      }
      else
      {
	state = move;
      }
      break;
      
    case decode:
      
      break;
      
    case move:
      
      valor_actual = exp_compass_get_avg_heading();
      if(is_button_rising_edge(BTN_LEFT))
      {
// 	turn_angle(-45);
	state = move_l;
	if(valor_actual - 450 < 0)
	{
	  target_angle = 3600 + (valor_actual - 450);
	}
	else if(target_angle == 3600)
	{
	  target_angle = 0;
	}
	else
	{
	  target_angle = valor_actual - 450;	  
	}
// 	state = move;
      }
      else if(is_button_rising_edge(BTN_RIGHT))
      {
//  	turn_angle(45);
// 	state = move;
	state = move_r;
	if(valor_actual + 450 > 3600)
	{
	  target_angle = (valor_actual + 450)%3600;
	}
	else if(target_angle == 3600)
	{
	  target_angle = 0;
	}
	else
	{
	  target_angle = valor_actual + 450;	  
	}
      }
      else 
      {
	state = move;
      }
    break;
   
    case move_l:
    {


      if(exp_compass_get_avg_heading() > target_angle)
      {
// 	turn_left();
        turn_angle(-15);
	state = move_l;
      }
      else
      {
	state = move;
      }
      
    }
    break;
      
      
    case move_r:
    {
      

      if(exp_compass_get_avg_heading() < target_angle)
      {
// 	turn_right();
	turn_angle(15);
	state = move_r;
      }
      else
      {
	state = move;
      }
    }
    break;
    
  }
}
    
