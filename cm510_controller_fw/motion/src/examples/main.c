#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"

void user_init(void)
{
  serial_console_init(57600);
}

void user_loop(void)
{
  if(is_button_falling_edge(BTN_UP))
  {
    if(is_action_running()==0x00)
    {
      cm510_printf("Walk ready\n");
      if(action_set_page(25)==0)
        action_start_page();
      else
        cm510_printf("Error loading page\n");
    }
  }
  else if(is_button_falling_edge(BTN_RIGHT))
  {
    if(is_action_running()==0x00)
    {
      cm510_printf("start walking\n");
      if(action_set_page(26)==0)
        action_start_page();
      else
        cm510_printf("Error loading page\n");
    }
  }
  else
  {
    if(is_button_falling_edge(BTN_DOWN))
    {
      if(is_action_running())
      {
        cm510_printf("stop action\n");
        action_stop_page();
      }
    }
  }
}

