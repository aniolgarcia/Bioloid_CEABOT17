#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"

typedef enum {wait_start,wait_ready,read_sensors} main_states;

void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
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
    case wait_ready: if(is_action_running())
                       state=wait_ready;
                     else
                       state=read_sensors;
                     break;
    case read_sensors: cm510_printf("CM510 ADC port 1: %d\n",get_adc_avg_channel(ADC_PORT_2));
                       cm510_printf("Exp. Board compass: %d\n",exp_compass_get_avg_heading());
                       cm510_printf("Exp. Board ADC port 7: %d\n",exp_adc_get_avg_channel(ADC7));
                       break;
  }
}
