#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa de la banda del cm510 per a la prova de visió. La beaglebone/raspberrypi decodifica la camera i envia un número per serial segons el contingut del QR. El cas 0 de la màquina d'estats llegeix el serial (que mai pot enviar 0, no us preocupeu) i els altres casos són cadascun dels moviments. Cal posar altres sistemes de seguretat, ja que ara mateix estem confiant plenament amb el bno055, i no es contemplen casos com per exemple que es vegin 2 QR, que no en trobi cap, que trobi el codi abans de l'esperat o que no el trobi...

unsigned int data = 0,num;

void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
  exp_adc_start();
  exp_bno055_start();
  if(is_button_pressed(BTN_UP))
  {
    exp_bno055_erase_calibration();
  }
  while(exp_bno055_is_calibrated()!=0x01)
  {
    _delay_ms(100);
  }
}

int user_loop(void)
{  
  switch(data)
  {
    case 0: do{
	      num=cm510_read(&data,1);
	      _delay_ms(100);
	    }while(num == 0);
	    break;
	    
    case 1: if(turn_angle(-45) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 1;
	    }
	    break;
	    
    case 2: if(turn_angle(45) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 2;
	    }
	    break;
	    
    case 3: if(turn_angle(-90) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 3;
	    }
	    break;
	    
    case 4: if(turn_angle(90) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 4;
	    }
	    break;	  
    case 5: if(turn_angle(-135) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 5;
	    }
	    break;
	    
    case 6: if(turn_angle(135) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 6;
	    }
	    break;

    case 7: if(turn_angle(-180) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 7;
	    }
	    break;

    case 8: if(turn_angle(180) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 8;
	    }
	    break;	      
  }
}
