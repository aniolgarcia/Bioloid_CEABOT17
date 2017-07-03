#include <util/delay.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "pan_tilt.h"
#include <stdio.h>

typedef enum {wait_start,wait_cmd,wait_pan_left,wait_pan_right} main_states;

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
  uint8_t cmd;

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
                       break;
      case wait_cmd: if(serial_console_get_num_data()>0)
                     {
//                       printf("New data received\n");
                       cm510_scanf("%c",&cmd);
                       printf("%c",cmd);
/*                       switch(cmd)
                       {
                         case 'l': pan_set_speed(200);
                                   pan_move_angle(-70);
                                   state=wait_pan_right;
                                   printf("move left\n");
                                   break;
                         case 'r': pan_set_speed(200);
                                   pan_move_angle(70);
                                   state=wait_pan_left;
                                   printf("move right\n");
                                   break;
                         default: state=wait_cmd;
                                  break;
                       }*/
                       state=wait_cmd;
                     }
                     else
                       state=wait_cmd;
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
    }
  }
}

