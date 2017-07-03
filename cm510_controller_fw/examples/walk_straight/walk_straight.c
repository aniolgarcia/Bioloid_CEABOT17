#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include <stdlib.h>

typedef enum {wait_start,wait_walk_ready,check_compass,turn} main_states;
typedef enum {stop_motion,wait_stop_motion,check_angle,wait_stop_turn} turn_states;

uint16_t get_avg_heading_inc(uint16_t initial_compass)
{
  int16_t actual_compass;
  int16_t increase;
  actual_compass = exp_compass_get_avg_heading();
  increase=actual_compass-(int16_t)initial_compass;

  if(increase<-1800){
    increase=increase+3600;
  }else if (increase>1800){
    increase=increase-3600;
  }
  return increase;
}

uint8_t turn_angle(int16_t angle)
{
  static turn_states state=stop_motion;
  uint8_t done=0x00,turn_error;
  static uint16_t initial_heading;

  switch(state)
  {
    case stop_motion: if(is_action_running())
                      {
                        action_stop_page();
                        state=wait_stop_motion;
                      }
                      else
                      {
                        if(angle>0)
                          action_set_page(68);
                        else
                          action_set_page(56);
                        action_start_page();
                        initial_heading=exp_compass_get_avg_heading();
                        state=check_angle;
                      }
                      break;
    case wait_stop_motion: if(is_action_running())
                             state=wait_stop_motion;
                           else
                           {
                             if(angle>0)
                               action_set_page(68);
                             else
                               action_set_page(56);
                             action_start_page();
                             initial_heading=exp_compass_get_avg_heading();
                             state=check_angle;
                           }
                           break;
    case check_angle: turn_error=abs(get_avg_heading_inc(initial_heading)-angle);
                      if(turn_error<100)
                      {
                        action_stop_page();
                        state=wait_stop_turn;
                      }
                      else
                        state=check_angle;
                      break;
    case wait_stop_turn: if(is_action_running())
                           state=wait_stop_turn;
                         else
                         {
                           state=stop_motion;
                           done=0x01;
                         }
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
  static uint16_t initial_heading;
  static int16_t increment_heading;

  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
                     {
                       action_set_page(31);
                       action_start_page();
                       state=wait_walk_ready;
                     }
                     else
                       state=wait_start;
                     break;
    case wait_walk_ready: if(is_action_running())
                            state=wait_walk_ready;
                          else
                          {
//                            initial_heading=exp_compass_get_avg_heading();
//                            action_set_page(32);
//                            action_start_page();
//                            state=check_compass;
                          }
                          break;
    case check_compass: if((increment_heading=get_avg_heading_inc(initial_heading))>200 || (increment_heading=get_avg_heading_inc(initial_heading))<-200)
                          state=turn;
                        else
                          state=check_compass;
                        break;
    case turn: if(turn_angle(increment_heading))
                 state=wait_walk_ready;
               else
                 state=turn;
               break;
  }
}

