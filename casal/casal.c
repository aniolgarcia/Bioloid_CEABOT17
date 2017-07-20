#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa per intentar fer que el robot camini recte. Quant detecta una desviació de n graus, crida una vegada a la funció turn_left() o turn_right() segons convingui. Alternativament es pot cridar la funció turn_angle() amb l'angle en positiu o negatiu I AMB VALORS ENTRE 0 I 360 (la funció ja multiplica el valor *10) o fer-ho de manera automàtica amb turn_angle(compass(valor_base)).

typedef enum {wait_start, wait_ready, walk, check_compass, stop, correct_l, correct_r} main_states;

typedef int bool;
#define true 1
#define false 0

int valor_base, state_aux;
bool error_r = false;
bool error_l = false;

const int compass_tolerance = 15;

typedef uint8_t (*fnct_ptr)(void);

// fnct_ptr fnct1=fast_walk_forward;
fnct_ptr fnct1=walk_forward;
//fnct_ptr fnct1=walk_right;
//fnct_ptr fnct1=walk_backward_turn_left;

fnct_ptr fnct2=turn_left;
// fnct_ptr fnct2=walk_forward_turn_left;
//fnct_ptr fnct2=walk_left;
//fnct_ptr fnct2=walk_backward_turn_right;

fnct_ptr fnct3=turn_right;
// fnct_ptr fnct3=walk_forward_turn_right;

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




void user_loop(void)
{
  static main_states state=wait_start;


  switch(state)
  {
    case wait_start: if(is_button_rising_edge(BTN_START))
                     {
		       valor_base = exp_compass_get_avg_heading();
                       action_set_page(31);
                       action_start_page();
                       state=wait_ready;
                     }
                     else
                       state=wait_start;
                     break;
		     
    case wait_ready: if(is_action_running())
		     {
		       state=wait_ready;
		     }
                     else
		     {
		       state=walk;
		     }
                     break;
		     
    case walk: if(exp_adc_get_avg_channel(ADC6) < 430)
	       {
		  fnct1();
		  state = check_compass;
	       }
	       else
	       {
		 mtn_lib_stop_mtn();
// 		 state = stop;
	       }
	       break;

    case check_compass: if(compass(valor_base) > 15)
			{
			  mtn_lib_stop_mtn();
			  if(fnct1() == 0x01)
			  {
			    state = correct_l;
			    fnct2();
			  }
			  else
			  {
			    state = check_compass;
			  }
			}
			else if(compass(valor_base) < -15)
			{
			  mtn_lib_stop_mtn();
			  if(fnct1() == 0x01)
			  {
			    state = correct_r;
			    fnct3();
			  }
			  else
			  {
			    state = check_compass;
			  }
			}
			else
			{
			  state=walk;
			}
			break;
		   
      case correct_l: mtn_lib_stop_mtn();
		      if(fnct2() == 0x01)
		      {
			state = walk;
		      }
		      else
		      {
			state  = correct_l;
		      }
		      break;
		      
      case correct_r: mtn_lib_stop_mtn();
		      if(fnct3() == 0x01)
		      {
			state = walk;
		      }
		      else
		      {
			state  = correct_r;
		      }
		      break;


	       
//       case correct: if(error_r == true)
// 		    {
// 		      mtn_lib_stop_mtn();
// 		      if(fnct2() == 0x01)
// 		      {
// 			error_r = false;
// 			state = walk;
// 		      }
// 		      else
// 		      {
// 			state = correct;
// 		      }
// 		      
// 		    }
// 		    else if(error_l == true)
// 		    {
// 		      mtn_lib_stop_mtn();
// 		      if(fnct3() == 0x01)
// 		      {
// 			error_l = false;
// 			state = walk;
// 		      }
// 		      else
// 		      {
// 			state = correct;
// 		      }
// 		      		      
// 		    }
// 		    break;
		    

		      
		  
		    
      case stop: mtn_lib_stop_mtn();
		 if(fnct1() == 0x01)
		 {
		    state = walk;
		  }
		  else
		  {
		    state = stop;
		  }
		break;
  }
}

