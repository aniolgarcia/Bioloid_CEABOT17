#include "stairs.h"
typedef enum {up_idle,up_wait_stop,up_wait_step1,up_delay1,up_wait_step2,up_wait_step3,up_wait_step4,wait_ready_up} stairs_up_states;
typedef enum {down_idle,down_wait_stop,down_wait_step1,down_wait_step2,down_wait_step3,wait_ready_down} stairs_down_states;
typedef enum {wait_stop_motion,wait_getting_up,wait_stabilize} get_up_states;

uint8_t stairs_up_process(fallen_t fall_state)
{
  static stairs_up_states state=up_idle;
  uint8_t done=0x00;

  switch(state)
  {
    case up_idle: if(is_action_running())
                  {
                    action_stop_page();
                    state = up_wait_stop;
                  }
                  else
                  {
                    action_set_page(227);
                    action_start_page();
                    state = up_wait_step1;
                  }
                  break;
    case up_wait_stop: if(is_action_running())
                         state = up_wait_stop;
                       else
                       { 
                         action_set_page(227);
                         action_start_page();
                         state = up_wait_step1;       
                       }
                       break;
    case up_wait_step1: if(is_action_running())
                          state = up_wait_step1;
                        else
                        {
                          user_time_set_one_time(1500);
                          state = up_delay1;
                        }
                        break;
    case up_delay1: if(user_time_is_done())
                    {
                      action_set_page(224);
                      action_start_page();
                      state = up_wait_step2;
                    }
                    else
                      state = up_delay1;
                    break;
    case up_wait_step2: if(is_action_running())
                          state = up_wait_step2;
                        else
                        {
                          action_set_page(225);
                          action_start_page();
                          state = up_wait_step3;
                        }
                        break;
    case up_wait_step3: if(is_action_running())
                          state = up_wait_step3;
                        else
                        {
                          action_set_page(226);
                          action_start_page();
                          state = up_wait_step4;
                        }
                        break;
    case up_wait_step4: if(is_action_running())
                          state = up_wait_step4;
                        else
                        {
                          action_set_page(31);
                          action_start_page();
                          state = wait_ready_up;
                        }
                        break;
    case wait_ready_up: if(is_action_running())
                          state = wait_ready_up;
                        else
                        {
                          state=up_idle;
                          done=0x01;
                        }
                        break;
  }

  if (fall_state!=robot_standing) state=up_idle;
  return done;
}


uint8_t stairs_down_process(void)
{
  static stairs_down_states state=down_idle;
  uint8_t done=0x00;

  switch(state)
  {
    case down_idle: if(is_action_running())
                    {
                      action_stop_page();
                      state = down_wait_stop;
                    }
                    else
                    {
                      action_set_page(228);
                      action_start_page();
                      state = down_wait_step1;
                    }
                    break;
    case down_wait_stop: if(is_action_running())
                           state = down_wait_stop;
                         else
                         { 
                           action_set_page(228);
                           action_start_page();
                           state = down_wait_step1;       
                         }
                         break;
    case down_wait_step1: if(is_action_running())
                            state = down_wait_step1;
                          else
                          {
                            action_set_page(229);
                            action_start_page();
                            state = down_wait_step2;
                          }
                          break;
    case down_wait_step2: if(is_action_running())
                            state = down_wait_step2;
                          else
                          {
                            action_set_page(230);
                            action_start_page();
                            state = down_wait_step3;
                          }
                          break;
    case down_wait_step3: if(is_action_running())
                            state = down_wait_step3;
                          else
                          {
                            action_set_page(31);
                            action_start_page();
                            state = wait_ready_down;
                          }
                          break;
    case wait_ready_down: if(is_action_running())
                            state = wait_ready_down;
                          else
                          {
                            state=down_idle;
                            done=0x01;
                          }
                          break;
  }

  return done;
}
uint8_t get_up_process(fallen_t fall_state)
{
  static get_up_states state;
  uint8_t done=0x00;

  switch(state)
  {
    case wait_stop_motion: if(!is_action_running())
			{
                        if(fall_state==robot_face_down)
                              action_set_page(F_getup);
                        else
                            action_set_page(B_getup);
                        action_start_page();
                        state=wait_getting_up;
                        }
                        else//if the robot was moving we wait until it stops
                            state= wait_stop_motion;
                        break;
    case wait_getting_up: if(!is_action_running()) //if it stopped getting up
                          {
                            state=wait_stabilize;
                            user_time_set_one_time(1000); //we set a timer for the robot to stabilize
                          }
                          else
                            state=wait_getting_up;
                          break;
    case wait_stabilize: if(user_time_is_done())
                         {
                           balance_reset_fall_state();
                           state=wait_stop_motion;
                           done=0x01;
                         }
                         else
                           state=wait_stabilize;
                         break;
  }

  return done;
}

