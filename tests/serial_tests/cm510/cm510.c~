#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>


void user_init(void)
{

  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  exp_board_init(30);
}


void user_loop(void)
{
	if(is_button_rising_edge(BTN_UP))
	{
		cm510_printf("ready\n");
		//cm510_write(1, 1);
	}
}


