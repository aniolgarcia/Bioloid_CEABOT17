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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>
#include "cm510_cfg.h"
#include "dynamixel_master.h"
#include "cm510.h"
#include "exp_board.h"

/* external user functions */
extern void user_init(void);
extern void user_loop(void);

/* internal loop functions */
extern void pushbuttons_loop(void);
extern void adc_loop(void);
extern void buzzer_loop(void);
extern void manager_loop(void);
extern void user_time_loop(void);

// general interface
void init_cm510(void)
{
  init_leds();
  init_buttons();
  init_adc();
  init_buzzer();
  init_user_time();
}

int16_t main(void)
{
  uint8_t num_servos;

  init_cm510();
  turn_led_off(LED_BAT);
  sei();
  _delay_ms(3000);
  /* call the user initialization function */
  dyn_master_init();
  exp_board_init(EXP_BOARD_ID);
  num_servos=18+exp_pwm_num_servos();
  manager_init(num_servos);
  user_init();
  // turn BAT_LED on to indicate the initialization is done
  turn_led_on(LED_BAT);

  while(1)
  {
    pushbuttons_loop();
    adc_loop();
    buzzer_loop();
    manager_loop();
    user_time_loop();
    user_loop();
  }
}
