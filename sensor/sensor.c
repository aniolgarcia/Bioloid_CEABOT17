#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa base per llegir qualsevol dels sensors del robot. Simplement imprimirà per serial el valor del snesor seleccionat. 
//Està fet per tal que no sigui necessari moure cap motor, però si hi ha ofsets definits, sí que es mourà per posar-se a lloc


typedef enum {wait_start,wait_ready,read_sensor} main_states;

int compass_correction(int value, int upper_end, int bottom_end)
{
	return (((upper_end - bottom_end)/3600)*value)+bottom_end;
}

int bno055_correction(int value) //Com que la bno055 té un rang extrany i a partir de -250 suma 250 a tot, hem de fer una petita correcció per aconseguir rnags normals.
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

int valor_base;
int compass(int valor_base)
{
        
	int nou_valor, desviament;
	
// 	nou_valor = exp_bno055_get_heading();
	nou_valor = bno055_correction(exp_bno055_get_heading());
	desviament = nou_valor - valor_base;
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
 





void user_init(void)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro();
  user_time_set_period(100);
  mtn_lib_init();
  exp_adc_start();
//   exp_compass_start(); //Ara utilitzem la bno055 en lloc de la brúixola, però es pot connectar i activar igualment, que no interferirà amb el bno055.
  exp_bno055_start();
  if(is_button_pressed(BTN_UP))
    exp_bno055_erase_calibration();
  while(exp_bno055_is_calibrated()!=0x01)
    _delay_ms(100);
}


void user_loop(void)
{
  static main_states state=wait_start;  
   
  switch(state)
  { 
    case wait_start: if(is_button_rising_edge(BTN_START))
                     {
                       state=wait_ready;
// 		       valor_base = exp_compass_get_avg_heading();
// 		       valor_base = exp_bno055_get_heading();
		       valor_base = bno055_correction(exp_bno055_get_heading()); 
                     }
                     else
		     {
                       state=wait_start;
		     }
                     break;
		     
    case wait_ready:  if(is_action_running())
		      {
			state=wait_ready;
		      } 
                      else
		      {
			state = read_sensor;
		      }
                     break;
		      
    case read_sensor: if(user_time_is_period_done())  
		      {
// 			printf("Davant (4): %d\n", exp_adc_get_avg_channel(ADC4));
// 			printf("Esquerre (5): %d\n", exp_adc_get_avg_channel(ADC5));
// 			printf("Dreta (6): %d\n", exp_adc_get_avg_channel(ADC3));
// 			cm510_printf("Exp. Board ADC port 6: %d\n   ",exp_adc_get_avg_channel(ADC6));
//   			cm510_printf("Exp. Board ADC port 4: %d\n",exp_adc_get_avg_channel(ADC4));
//   			cm510_printf("Exp. Board ADC port 7: %d\n",exp_adc_get_avg_channel(ADC7));
//  			cm510_printf("CM510 ADC port 1: %d\n",get_adc_avg_channel(ADC_PORT_2));
// 			cm510_printf("Exp. Board compass: %d  ", exp_compass_get_avg_heading()); 
			cm510_printf("Original: %d  ", exp_bno055_get_heading());
			cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			cm510_printf("Desviament: %d\n",compass(valor_base));
//    			cm510_printf("GYRO X: %d  ", get_adc_channel(BALANCE_GYRO_X_CHANNEL));
//   			cm510_printf("GYRO Y: %d\n", get_adc_channel(BALANCE_GYRO_Y_CHANNEL));
			user_time_set_period(500);
		      }
		      state = read_sensor;
                      break;
  } 
}
