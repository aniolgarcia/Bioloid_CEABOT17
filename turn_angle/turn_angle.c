#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa per testejar la funció turn angle i comprovar la seva precisió. Si s'apreta un botó lateral, el robot gira 45 graus en aquella direcció.

typedef enum {wait_start, wait_ready, gir_dreta, gir_esquerra} main_states;




const int compass_tolerance = 15;





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
                       action_set_page(31);
                       action_start_page();
                       state=wait_ready;
                     }
                     else
                       state=wait_start;
                     break;
		     
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
		  if (turn_angle(45)==0x01){
		    state = wait_ready;
	       }
	       break;
	       
	       
    case gir_esquerra: 
		  if (turn_angle(-45)==0x01){
		  state = wait_ready;
	       }
	       break;
	       
  }
}



