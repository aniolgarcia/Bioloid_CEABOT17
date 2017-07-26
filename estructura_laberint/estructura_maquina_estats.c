#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start, wait_ready, wait_f, walk_l, walk_r, turn, stop} main_states;



void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
}



void user_loop(void)
{
  static main_states state=wait_start;
  static main_states actual = walk_l;
  int adc7;
  
  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
		     {
                       action_set_page(31);
                       action_start_page();
                       state = wait_ready;
                     }
                     else
		     {
                       state = wait_start;
		     }
                     break;
		      
    case wait_ready: if(is_action_running())
		     {
			state = wait_ready;
		     }
		     else
		     {
		        if(actual == walk_l)
			{
			  walk_left();
			}
			else if(acutal == walk_r)
			{
			  walk_right():
			}
			else if(actual == walk_f)
			{
			  walk_forward();
			}
			state = actual;
		     }
		     break;
    case walk_l: if(exp_adc_get_avg_channel(ADC7) > 190) //si sensor davant
		 {
		   actual = walk_f;		   
		 }
		 else if(exp_adc_get_avg_channel(ADC6) < 20) //si sensor esquerra
		 {
		   actual = walk_r;		   
		 } 
}