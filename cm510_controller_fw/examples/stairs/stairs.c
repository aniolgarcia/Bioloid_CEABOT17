#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"

typedef enum {wait_start,wait_ready,walk_to_stairs1,wait_stop1,up_stairs1} main_states;

void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
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
                       walk_forward();
                       state=walk_to_stairs1; 
                     }
                     break;
    case walk_to_stairs1: if(exp_gpio_get_value(GPIO1)==0x00 || exp_gpio_get_value(GPIO18)==0x00)
                          {   
                            mtn_lib_stop_mtn();
                            state=wait_stop1;
                          }
                          else
                            state=walk_to_stairs1;
                          walk_forward();
                          break;
    case wait_stop1: if(walk_forward())
                       state=up_stairs1;
                     else
                       state=wait_stop1;
                     break;
    case up_stairs1: if(stairs_up_process(robot_standing))
                       state=wait_start;
                     else
                       state=up_stairs1; 
                     break;
  }
}

