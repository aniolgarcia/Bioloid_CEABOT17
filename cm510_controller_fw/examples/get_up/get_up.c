#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"

typedef enum {wait_start,check_fallen,get_up} main_states;
typedef enum {stop_motion,wait_stop_motion,wait_getting_up,wait_stabilize} get_up_states;

uint8_t get_up_process(fallen_t fall_state)
{
  static get_up_states state;
  uint8_t done=0x00;

  switch(state)
  {
    case stop_motion: if(is_action_running())
                      {
                        action_stop_page();
                        state=wait_stop_motion;
                      }
                      else
                      {
                        if(fall_state==robot_face_down)
                          action_set_page(27);
                        else
                          action_set_page(28);
                        action_start_page();
                        state=wait_getting_up;
                      }
                      break;
    case wait_stop_motion: if(!is_action_running())
                           {
                             if(fall_state==robot_face_down)
                               action_set_page(27);
                             else
                               action_set_page(28);
                             action_start_page();
                             state=wait_getting_up;
                           }
                           else
                             state= wait_stop_motion;
                           break;
    case wait_getting_up: if(!is_action_running())
                          {
                            state=wait_stabilize;
                            user_time_set_one_time(1000);
                          }
                          else
                            state=wait_getting_up;
                          break;
    case wait_stabilize: if(user_time_is_done())
                         {
                           balance_reset_fall_state();
                           state=stop_motion;
                           done=0x01;
                         }
                         else
                           state=wait_stabilize;
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
}

void user_loop(void)
{
  static main_states state=wait_start;
  static fallen_t fall_state;

  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
                       state=check_fallen;
                     else
                       state=wait_start;
                     break;
    case check_fallen: fall_state=balance_robot_has_fallen();
                       if(fall_state!=robot_standing)
                       {
                         state=get_up;
                       }
                       else
                         state=check_fallen;
                       break;
    case get_up: if(get_up_process(fall_state)==0x01)
                   state=check_fallen;
                 else
                   state=get_up;
                 break;
  }
}
