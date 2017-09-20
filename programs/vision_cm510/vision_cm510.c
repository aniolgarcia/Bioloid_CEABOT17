#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa final de visió. La part de visió va funcionar perfectament, el que va fallar va ser la precisió del robot a l'hora de fer els girs. 
//Estaria bé implementar un sistema de Pan & Tilt, de manera que si el robot no veu cap QR mogui el cap.
//S'hauria d'afegir mecanismes de seguretat, per exemple que no legeixi el mateix QR 2 vegades.



//Variables globals
int num, target_degrees;
unsigned char data;

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

//Funicó per sumar angles
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

//Com que el rang és de [-1800, -250(aprox)] U [0, 2050(aprox)], quan passi de -250, simplement li restem 250.
int bno055_correction(int value)
{
	if(value > -250)
	{
	  return value -250;
	}
	else
	{
	  return value;
	}
}

#define err 100
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states; //Estats de la funció de gir

uint8_t turn_realtive(int angle){
	static turn_states s = t_init;
	static int comp_ini = 0;
	static int comp_end = 0;
	int done = 0;

	switch (s){
		case t_init:
			comp_ini = bno055_correction(exp_bno055_get_heading());
 			comp_end = suma_angles (comp_ini,angle*10);
//			comp_end = angle;

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
//  balance_enable_gyro(); // En aquest cas hem de tenir el gyro desactivat, ja que si no intenta compensar els propis moviments de gir
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


//Estats de la màquina principal
typedef enum {wait_start, wait_ready, wait_5, read_QR, get_target_degrees, turn} main_states; 
main_states state = wait_start;

void user_loop(void)
{

  	switch(state)
  	{
  	case wait_start:
  		if(is_button_rising_edge(BTN_START))
        {
            action_set_page(31);
            action_start_page();
			user_time_set_period(5000);
            state=wait_5;
        }
        else
        {
            state=wait_start;
        }
        break;


    case wait_5: 	 
    	if(user_time_is_period_done())
        {
            state = wait_ready;
        }
        else
        {
            state = wait_5;
        }
        break;

    case wait_ready: 
    	if(is_action_running())
        {
            state = wait_ready;
        }
        else
        {
        	state = read_QR;
        }
        break;

    case read_QR:
		cm510_printf("ready\n"); //Enviem ready per serial, per tal que vision_computer enviï la lectura
    	num=cm510_read(&data,1); //Llegim la lectura que envia vision_computer

		if(num == 0) //Si vision_computer tira 0, és que no ha llegit res
		{
			state = read_QR;
		}
		else
		{
			state = get_target_degrees;
		}
		
		break;

	case get_target_degrees: //Fem un switch sobre la lectura de QR
		switch(data)
		{
		case '1':
			target_degrees = -45;
			break;
		case '2':
			target_degrees = 45;
			break;
		case '3':
			target_degrees = -90;
			break;
		case '4':
			target_degrees = 90;
			break;
		case '5':
			target_degrees = -135;
			break;
		case '6':
			target_degrees = 135;
			break;
		case '7':
			target_degrees = -175;
			break;
		case '8':
			target_degrees = 175;
			break;
		}
		state = turn;
		break;

		case turn:
			if(turn_realtive(target_degrees) == 0x01) //Gira els graus que diu el QR
			{
				state = read_QR;
			}
			else
			{
				state = turn;
			}
			break;



  	}
}
