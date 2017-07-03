/*
 *  Copyright (c) 2011 Humanoid Lab
 * 
 *  This file is part of iri-libbioloid. iri-libbioloid is free software: you
 *  can redistribute it and/or modify it under the terms of the GNU Lesser
 *  Public License as published by the Free Software Foundation, either 
 *  version 3 of the License, or (at your option) any later version.
 * 
 *  iri-libbioloid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser Public License
 *  along with iri-libbioloid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cm510.h"
#include <stdio.h>
#include <util/delay.h>

void user_init(void)
{
  serial_console_init(57600);
  user_time_set_period(10);
}

void user_loop(void)
{
  if(user_time_is_period_done())
  {
    cm510_printf("Gyro X: %d\n",get_adc_channel(ADC_PORT_3));
    cm510_printf("Gyro Y: %d\n",get_adc_channel(ADC_PORT_4));
  }
  if(is_button_pressed(BTN_START))
    turn_led_on(LED_AUX);
  else
    turn_led_off(LED_AUX);
  if(is_button_falling_edge(BTN_UP))
    buzzer_start(NOTE_SOL,30,30);
  else
  {
    if(is_button_falling_edge(BTN_DOWN))
      buzzer_stop();
  }
}
