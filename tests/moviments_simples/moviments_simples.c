 
#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef enum {wait_start,wait_walk_ready,wait_cmd,walk,walk2,walk3} main_states;

void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
//   balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
}

typedef uint8_t (*fnct_ptr)(void);

fnct_ptr fnct1=fast_walk_forward;
// fnct_ptr fnct1 = walk_forward;
//fnct_ptr fnct1 = walk_backward;

//fnct_ptr fnct2 = turn_left;
fnct_ptr fnct2 = walk_left;
//fnct_ptr fnct2 = walk_backward_turn_left;

//fnct_ptr fnct3 = turn_right;
fnct_ptr fnct3 = walk_right;
//fnct_ptr fnct3 = walk_backward_turn_right;

void user_loop(void)
{
  static main_states state=wait_start;
  static unsigned char i=0;

  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
                     {
                       action_set_page(30);
                       action_start_page();
                       state=wait_walk_ready;
                     }
                     else
                       state=wait_start;
                     break;
    case wait_walk_ready: if(is_action_running())
                            state=wait_walk_ready;
                          else
                            state=wait_cmd;
                          break;
			  
    case wait_cmd: if(is_button_rising_edge(BTN_RIGHT))
                   {
		     if(balance_is_gyro_enabled())
		     {
		       balance_disable_gyro();
		     }
                     fnct3();
                     state=walk3;
                   }
                   else if(is_button_rising_edge(BTN_LEFT))
                   {
		     if(balance_is_gyro_enabled())
		     {
		       balance_disable_gyro();
		     }
                     fnct2();
                     state=walk2;
                   }
                   else if(is_button_rising_edge(BTN_UP))
		   {
		     if(!balance_is_gyro_enabled())
		     {
		       balance_enable_gyro();
		     }
		     fnct1();
		     state = walk;
		   }
                   else 
                     state=wait_cmd;
                   break;
		   
    case walk: if(is_button_rising_edge(BTN_DOWN))
                 mtn_lib_stop_mtn();
               if(fnct1()==0x01)
                 state=wait_cmd;
               else
               {
                 state=walk;
               }
               break;
	       
    case walk2: if(is_button_rising_edge(BTN_DOWN))
                  mtn_lib_stop_mtn();
                if(fnct2()==0x01)
                  state=wait_cmd;
                else
                  state=walk2;
                break;
		
    case walk3: if(is_button_rising_edge(BTN_DOWN))
                  mtn_lib_stop_mtn();
                if(fnct3()==0x01)
                  state=wait_cmd;
                else
                  state=walk3;
                break;
  }
}
