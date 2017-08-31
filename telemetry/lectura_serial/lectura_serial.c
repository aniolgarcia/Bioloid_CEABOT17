#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

void user_init(void)
{
  serial_console_init(9600);
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
  
  unsigned char data='a', num;
  cm510_write(&data,1);
  while(1)
  {
    do{
      num=cm510_read(&data,1);
      _delay_ms(100);
    }while(num == 0);
    toggle_led(LED_AUX);
    
	cm510_write(&data,1);
	if(data == '1')
	{
		toggle_led(LED_PLAY);
	}
	else if(data == '2')
	{
		toggle_led(LED_RxD);
	}
//     unsigned char aChar = '0' + 1; //convert int to char
//     serial.write(&aChar, 1);
    
  }

  return 0;
}
