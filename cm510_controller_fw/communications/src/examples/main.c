#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "dynamixel_master.h"
#include "serial_console.h"
#include <stdio.h>

int16_t main(void) 
{
  unsigned char num;
  unsigned char ids[32];

  dyn_master_init();
  serial_console_init(57600);
  sei();

  while (1) 
  {
    dyn_master_scan(&num,ids);
    if(num==1) 
    {
      cm510_printf("Found one device\n");
      PORTC=0x3F;
    }
    else
    {
      cm510_printf("No device found\n");
      PORTC=0x7F;
    }
  }
}
