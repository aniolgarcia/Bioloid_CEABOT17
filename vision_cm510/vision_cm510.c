#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa de la banda del cm510 per a la prova de visió. La beaglebone/raspberrypi decodifica la camera i envia un número per serial segons el contingut del QR. El cas 0 de la màquina d'estats llegeix el serial (que mai pot enviar 0, no us preocupeu) i els altres casos són cadascun dels moviments. Cal posar altres sistemes de seguretat, ja que ara mateix estem confiant plenament amb el bno055, i no es contemplen casos com per exemple que es vegin 2 QR, que no en trobi cap, que trobi el codi abans de l'esperat o que no el trobi...

int compass_param(int ini, int actual)
{
	short int inc = actual - ini;
	if(inc < -1800)
	{ 
	  inc += 3600;
	}
	else if(inc > 1800)
	{
	  inc -= 3600;
	}
	return inc;
}



int bno055_correction(int value)
{
	if(value > -240)
	{
	  return value - 240;
	}
	else
	{
	  return value;
	}
}

//Funció que suma angles deixant-los sempre en un rang de [0, 3600]
int suma_angles(int a, int b)
{
	int res = a + b;
	if(res < 0 )
	{
	  res += 3600;
	}
	else if(res > 3600)
	{
	  res -= 3600;
	}
	return res;
}

typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;
#define err 100 //Màxim error permès a turn.
uint8_t gira(int angle){
	static turn_states s = t_init;
	static int comp_ini = 0;
	static int comp_end = 0;
	int done = 0;
	
	switch (s){
		case t_init:
			comp_ini = bno055_correction(exp_bno055_get_heading());
			comp_end = suma_angles (comp_ini,angle*10);
			
			s=t_middle;
			break;
		case t_middle:
			if (abs (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end))>err){
			// ("diff = %d\n",compass_param (bno055_correction(exp_bno055_get_heading()),comp_end));
				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)>err){
					s =t_right;
				}
				else if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)<-err){
					s=t_left;
				}
			}
			else {
				s = t_wait_end;		
			}
			break;
		case t_right:
			if (turn_right()){
				s = t_wait_end;
			}
			else {
				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)<err){ 
						mtn_lib_stop_mtn();
				}
				else s = t_right;
				
			}
			break;
			
		case t_left:
			if (turn_left()){
				s = t_wait_end;
			}
			else {
				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)>-err){
						mtn_lib_stop_mtn();
				}
				else s = t_left;
				
			}
			break;
		case t_wait_end:
			done =0x01;
			s = t_init;
			break;
	
	}
	return done;
}


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
  cm510_printf("Init");

}

int state = -2;
unsigned char num;




void user_loop(void)
{
  int data;
  switch(state)
  {
    case -2: if(is_button_rising_edge(BTN_START))
	      {
		action_set_page(31);
		action_start_page();
		data = -1;
	      }
	      else
		data = -2;
	      break;
	      
    case -1: if(is_action_running())
	     {
	       data = -1;
	     }
	     else
	     {
	       data = 0;
	     }
	     break;
	     
    case 0: cm510_printf("cas 0");
	    do{
	      num=cm510_read(&data,1);
	      _delay_ms(100);
	    }while(num == 0);
	    break;

    case 1: cm510_write(&data,1);
	    if(gira(-45) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 1;
	    }
	    break;
	    
    case 2: if(gira(45) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 2;
	    }
	    cm510_write(&data,1);

	    break;
	    
    case 3: if(gira(-90) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 3;
	    }
	    cm510_write(&data,1);
	    break;
	    
    case 4: if(gira(90) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 4;
	    }
	    cm510_write(&data,1);
	    break;	  
    case 5: if(gira(-135) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 5;
	    }
	    cm510_write(&data,1);
	    break;
	    
    case 6: if(gira(135) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 6;
	    }
	    cm510_write(&data,1);
	    break;

    case 7: if(gira(-180) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 7;
	    }
	    cm510_write(&data,1);
	    break;

    case 8: if(gira(180) == 0x01)
	    {
	      data = 0;
	    }
	    else
	    {
	      data = 8;
	    }
	    cm510_write(&data,1);
	    break;	      
  }
  state = data;
}
