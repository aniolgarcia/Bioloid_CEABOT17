#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include "action_id.h"
typedef enum {wait_start,wait_ready,measure,end} main_states;

#define left_foot_forward 				GPIO1	
#define left_foot_forward_down		GPIO2
#define left_foot_lateral_down		GPIO3
#define right_foot_forward				GPIO18
#define right_foot_forward_down		GPIO17
#define right_foot_lateral_down		GPIO16


void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  mtn_lib_init();
  init_leds();
  exp_board_init(192);
  exp_adc_start();
  exp_compass_start();
 	if ( exp_board_is_present()) turn_led_on (LED_MANAGE);
}

void user_loop(void)
{
  static main_states state=wait_start;
 // printf ("state=%d\n",state);
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
                       state=measure; 
                     }
                     break;
    case measure: 
    
    	if (is_button_rising_edge (BTN_RIGHT)){
    		state = end;
    		
    	}
    	else {
    	
    	//1 ==> there is no obstacle --> foot LED on
    	//0 ==> there is an obstacle --> foot LED off
						if (exp_gpio_get_value (left_foot_forward)) turn_led_on (LED_MANAGE);
						else turn_led_off (LED_MANAGE);
						if (exp_gpio_get_value (left_foot_forward_down)) turn_led_on (LED_PROGRAM);
						else turn_led_off (LED_PROGRAM);
						if (exp_gpio_get_value (left_foot_lateral_down)) turn_led_on (LED_PLAY);	
						else turn_led_off (LED_PLAY);
						if (exp_gpio_get_value (right_foot_forward)) turn_led_on (LED_TxD);
						else turn_led_off (LED_TxD);
						if (exp_gpio_get_value (right_foot_forward_down)) turn_led_on (LED_RxD);
						else turn_led_off (LED_RxD);
						if (exp_gpio_get_value (right_foot_lateral_down)) turn_led_on (LED_AUX);
						else turn_led_off (LED_AUX);			
						
					  printf ("%d,%d,%d,%d,%d,%d\n, ", exp_gpio_get_value(left_foot_forward),exp_gpio_get_value(left_foot_forward_down),exp_gpio_get_value(left_foot_lateral_down),exp_gpio_get_value(right_foot_forward),exp_gpio_get_value(right_foot_forward_down),exp_gpio_get_value(right_foot_lateral_down));


       }
       		
       break;
  case end:
  	if (is_action_running()) state = end;	
  }
}

