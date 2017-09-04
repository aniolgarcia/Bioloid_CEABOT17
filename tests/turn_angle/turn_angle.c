#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa per testejar la funció turn angle i comprovar la seva precisió. Si s'apreta un botó lateral, el robot gira 45 graus en aquella direcció.

typedef enum {wait_start, wait_ready, gir_dreta, gir_dreta2, gir_esquerra} main_states;
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;


#define err 100 


const int compass_tolerance = 15;
//Funció per calcular el desviament del robot respecte l'orientació inicial. És com compass però amb la posició inicial parametritzada.
int compass_param(int ini, int actual)
{
	short int inc = actual - ini;
	if(inc<-1800)
	{ 
	  inc+=3600;
	}
	else if(inc>1800)
	{
	  inc-=3600;
	}
	return inc;
}



int bno055_correction(int value)
{
	if(value > -240)
	{
	  return value -240;
	}
	else
	{
	  return value;
	}
}

//Funció per calcular el desviament del robot respecte l'orientació inicial. Agafa angles en un rang de [0, 3600] i els retorna en un rang de [0, 360].
int compass(int valor_base)
{
        
	int nou_valor, desviament;
	
// 	nou_valor = exp_bno055_get_headig();
	nou_valor = bno055_correction(exp_bno055_get_heading());
	desviament = nou_valor - valor_base;
	//Com que el desviament no pot ser més de 180, si es passa és que ha passat de 360 o 0
	if(desviament > 1800)
	{
		desviament = desviament - 3600;
	}
	else if(desviament < -1800)
	{
		desviament = desviament + 3600;
	}
	
	desviament = desviament/10;
	
	return desviament;
}

//Funció que suma angles deixant-los sempre en un rang de [0, 3600]
int suma_angles(int a, int b)
{
	int res = a + b;
	if(res < -1800 )
	{
	  res+=3600;
	}
	else if(res > 1800)
	{
	  res-=3600;
	}
	return res;
}


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
}



static int valor_base;
void user_loop(void)
{
  static main_states state=wait_start;


  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
                     {
					valor_base = bno055_correction(exp_bno055_get_heading());
                       action_set_page(31);
                       action_start_page();
                       state=wait_ready;
                     }
                     else
                       state=wait_start;
                     break;
		     
    case wait_ready: if(is_button_rising_edge(BTN_RIGHT)){
			state = gir_dreta;
		      }
		     if (is_button_rising_edge(BTN_LEFT))
		        state = gir_esquerra;
		      /*if(is_action_running())
		     {
		       state=wait_ready;
		     }
                     else
		     {
		       state=gir;
		     }*/
                     break;
		     
    case gir_dreta: 
		  if (gira(90)==0x01){
		    state = gir_dreta2;
	       }
	       break;
	case gir_dreta2: 
		  if (gira(-90)==0x01){
		    state = wait_ready;
	       }
	       break;
	       
	       
    case gir_esquerra: 
		  if (gira(-175)==0x01){
		  state = wait_ready;
	       }
	       break;
	       
  }
}



