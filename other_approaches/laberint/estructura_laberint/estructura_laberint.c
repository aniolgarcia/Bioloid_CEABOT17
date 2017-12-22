#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

typedef int bool;
#define true 1
#define false 0

//Intent inical de definir l'estructura del laberint, no és un programa funcional. Ara mateix només funcionen els estats wait_start, wait_ready i walk_l,
// i el que fa és caminar lateralment fins a trobar un objecte amb el sensor IR. 
//Els altres estats tan sols s'han utilitzat per fer proves i no sempre reflecteixen l'estructura correcta.

typedef enum {wait_start, wait_ready, wait_cmd, walk_l, walk_r, walk_f, walk_return, turn, stop} main_states;

const bool simulat = true;
adc_t davant, esquerra, dreta;



int cont = 0;
int valor_base, valor_actual;
 
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
  
  if(simulat == true)
  {
    davant = ADC3;
    esquerra = ADC4;
    dreta = ADC5;
  }
  else
  {
    davant = ADC4;
    esquerra = ADC5;
    dreta = ADC6;
  }
 
}

int compass(int valor_base)
{
        
	int nou_valor, desviament;
	
	nou_valor = exp_compass_get_avg_heading();
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


void user_loop(void)
{
  static main_states state=wait_start;
	
  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
		      {
		       action_set_page(31);
		       action_start_page();
		       valor_base = exp_compass_get_avg_heading();
		       valor_actual = valor_base;
		       state=wait_ready;
		       cont = 0;
		      }
		      else
		      {
			state=wait_start;
		      }
		      break;
		      
    case wait_ready: if(is_action_running())
		      {
			state = wait_ready;
		      }
		      else
		      {
			state = walk_r; //Ara l'enviem a walk_l perquè només volem que tiri cap a l'esquerra.
		      }
		      break;
 
// // Estat utilitzat per fer proves de girs, molt probablement no estarà al programa final
//     case wait_cmd: if(is_button_rising_edge(BTN_LEFT))
// 		    {
// 		      state = walk_l;
// 		    }
// 		    else if(is_button_rising_edge(BTN_RIGHT))
// 		    {
// 		      state = walk_r; 
// 		    }
// 		    else if(is_button_rising_edge(BTN_UP))
// 		    {
// 		      adc7 = exp_adc_get_avg_channel(ADC7);
// 		      cm510_printf("Sensor IR adc7: %d", adc7); 
// 		      user_time_set_period(500);
// 		      state = wait_ready;
// 		    }
// 		    else if (is_button_rising_edge(BTN_DOWN)) 
//                     { 
// 		      //walk_forward();
//                       mtn_lib_stop_mtn(); 
// 		      state= wait_ready; 
//                     }
//                     else
// 		    {
// 		      state = wait_ready; 
// 		    }
// 		    break;

    case walk_l: if(exp_adc_get_avg_channel(davant) < 60)
		 { 
		   walk_left();
		   state = stop;
		 }
		 else
		 {
		   printf("Davant: %d", exp_adc_get_avg_channel(davant));
		   printf("  Esquerra: %d\n", exp_adc_get_avg_channel(esquerra)); 
		    if(exp_adc_get_avg_channel(esquerra) > 100 && !is_button_rising_edge(BTN_DOWN))
		    {
		      walk_left();
		      if(compass(valor_base) > 5) 
		      {
// 			mtn_lib_stop_mtn();
			turn_left();
			state = walk_l;

		      }
		      else if(compass(valor_base) < -5)
		      {	
// 			mtn_lib_stop_mtn();
			turn_right();
			state = walk_l;

		      } 
		      state = walk_l;
		    } 
		    else
		    { 
		      mtn_lib_stop_mtn();
		      state = stop;		      
		    }
		 }
		    break;
		    
    case walk_r: if(exp_adc_get_avg_channel(davant) < 60)
		 { 
		   walk_right();
		   state = stop;
		 }
		 else
		 {
		   printf("Davant: %d", exp_adc_get_avg_channel(davant));
		   printf("  Dreta: %d\n", exp_adc_get_avg_channel(dreta));
		    if(exp_adc_get_avg_channel(dreta) > 100 && !is_button_rising_edge(BTN_DOWN))
		    {
		      
		      walk_right();
		      if(compass(valor_base) > 5) 
		      {
// 			mtn_lib_stop_mtn();
// 			walk_forward_turn_left();
			turn_left();
			state = walk_r;
 
		      }
		      else if(compass(valor_base) < -5)
		      {
// 			mtn_lib_stop_mtn();
// 			walk_forward_turn_right();
			turn_right();
			state = walk_r;

		      }
		      state = walk_r;
		    }
		    else
		    {
		      mtn_lib_stop_mtn();
		      state = walk_l;		      
		    }
		 }
		    break;
		    
		    
   case walk_f: printf("Davant: %d", exp_adc_get_avg_channel(davant));
		if(exp_adc_get_avg_channel(davant) < 450)
		{
		  walk_forward();
		  state = walk_f;
		}
		else
		{
		  mtn_lib_stop_mtn();
		  state = turn;
		}
		
		
   case turn: turn_angle(180);
	      valor_base = exp_compass_get_avg_heading();
	      state = walk_return;
	      
   case walk_return: if(exp_adc_get_avg_channel(davant) < 450)
		     {
		       walk_forward();
		       state = walk_f;
		     }
		     else
		     {
		       mtn_lib_stop_mtn();
		       state = stop;
		     }
     
    case stop: 	printf("STOP!\n");
		if(is_button_rising_edge(BTN_UP))
		{
		  state = wait_ready;
		}
		else
		{
		  state = stop; 
		}
		break;
  }
}
		      