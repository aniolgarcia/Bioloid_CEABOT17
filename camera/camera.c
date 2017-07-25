#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>
// //   char in;
//   uint8_t in;
// 
// 
// void user_init(void)
// {
//   serial_console_init(57600);
//   balance_init();
//   balance_calibrate_gyro();
//   balance_enable_gyro();
//   user_time_set_period(100);
//   mtn_lib_init();
//   exp_adc_start();
//   exp_compass_start();
// 
// }
// 
// 
// void user_loop(void)
// {
//   
// //   cm510_scanf("%d",&in);
//   cm510_read(&in, 2);
//   if(user_time_is_period_done())
//   {
//     cm510_printf("In: %d\n", in);
//     user_time_set_period(500);
//   }
// }


// #include "cm510.h"
// #include <util/delay.h>
// #include <stdio.h>
void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
  exp_adc_start();
  exp_compass_start();
}

int user_loop(void)
{
//   serial_console_init(57600);
//   init_leds();
//   sei();
  

  
  
  unsigned char data='a',num;
  cm510_write(&data,1);
  turn_led_on(LED_AUX);
  while(1)
  {
    do{
      num=cm510_read(&data,1);
      _delay_ms(100);
    }while(num == 0);
    toggle_led(LED_AUX);
    cm510_write(&data,1);
    
    if (data=='A'){
      toggle_led(LED_PLAY);
      turn_angle(30);
    }
    if (data=='B'){
      toggle_led(LED_PROGRAM);
      turn_angle(-30);
    }
  }

  return 0;
}
