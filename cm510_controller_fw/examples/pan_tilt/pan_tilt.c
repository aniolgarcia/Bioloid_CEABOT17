#include <util/delay.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "pan_tilt.h"

typedef enum {wait_start,wait_cmd,wait_pan_left,wait_pan_right,wait_tilt_up,wait_tilt_down} main_states;

#define PAN_SERVO_ID    19
#define TILT_SERVO_ID   20

void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  pan_tilt_init(PAN_SERVO_ID,TILT_SERVO_ID);
}

void user_loop(void)
{
  static main_states state=wait_start;

  if(user_time_is_period_done())
  {
    switch(state)
    {
      case wait_start: if(is_button_rising_edge(BTN_START))
                       {
                         action_set_page(31);
                         action_start_page();
                         state=wait_cmd;
                       }
                       else
                         state=wait_start;
                       cm510_printf("wait_start %d\n",(int)state);
                       break;
      case wait_cmd: if(is_button_rising_edge(BTN_LEFT))
                     {
                       pan_set_speed(10);
                       pan_move_angle(70);
                       state=wait_pan_left;
                     }
                     else if(is_button_rising_edge(BTN_RIGHT))
                     {
                       pan_set_speed(200);
                       pan_move_angle(-70);
                       state=wait_pan_right;
                     } 
                     else if(is_button_rising_edge(BTN_UP))
                     {
                       tilt_set_speed(10);
                       tilt_move_angle(70);
                       state=wait_tilt_up;
                     } 
                     else if(is_button_rising_edge(BTN_DOWN))
                     {
                       tilt_set_speed(200);
                       tilt_move_angle(-70);
                       state=wait_tilt_down;
                     } 
                     break;
      case wait_pan_left: if(pan_is_moving())
                            state=wait_pan_left;
                          else
                            state=wait_cmd;
                          break; 
      case wait_pan_right: if(pan_is_moving())
                             state=wait_pan_right;
                           else
                             state=wait_cmd;
                           break;
      case wait_tilt_up: if(tilt_is_moving())
                           state=wait_tilt_up;
                         else
                           state=wait_cmd;
                         break;
      case wait_tilt_down: if(tilt_is_moving())
                             state=wait_tilt_down;
                           else
                             state=wait_cmd;
                           break;
    }
  }
}

