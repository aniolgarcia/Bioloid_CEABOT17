#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Intent inical de l'estructura del laberint. L'algorisme funciona correctament, però l'execució no és del tot bona. Hi ha problemes sobretot a l'hora de corregir els errors quan es camina recte.

typedef int bool; //Definim el boleà, que en C no existeix
#define true 1
#define false 0

typedef enum {wait_start, wait_ready, walk_l, walk_r, walk_f, turn, walk_return, stop, correct_l, correct_r} main_states; //Estats de la màquina d'estats

typedef uint8_t (*fnct_ptr)(void);
fnct_ptr fnct_r = turn_right;
// fnct_ptr fnct_r = turn_angle(15);

fnct_ptr fnct_l = turn_left;
// fnct_ptr fnct_l = turn_angle(-15);

main_states prev; //Variable per guardar els estats abans de corregir


//Definim variables dels ports, que canvien segons si és una simulació o no ( més detalls a ../index.txt) 
const bool simulat = false;
adc_t davant, esquerra, dreta; 



int cont = 0;
int valor_base, valor_actual;
static int comp_error = 20;
static int dist_frontal = 80;
static int dist_lateral = 250;
 
void user_init(void) //S'executa una sola vegada (setup de l'arduino)
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
    davant = ADC6;
    esquerra = ADC4;
    dreta = ADC7;
  }
 
}

//Funció per calcular el desviament del robot respecte l'orientació inicial
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

int suma_angles (int a, int b)
{
	int res = a+b;
	if (res < 0 ) res+=3600;
	else if (res>3600){
		res-=3600;
	}
	return res;

}


void user_loop(void) //Es repeteix infinitament (equivalent al loop d'arduino o a while(1))
{
  static main_states state=wait_start;

  switch(state)
  {
    /* INICIALITZACIÓ */
    //Wait_start i wait_ready són les inicialitzacions
    
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
		       walk_left(); //Comencem caminant cap a l'esquerra (perquè camina millor)
		       state=walk_l; 
		     }
                     break;

		     
    /* MOVIMENT */
    //Estat per caminar cap a diverses direccions
    
    case walk_l: if(exp_adc_get_avg_channel(davant) < dist_frontal) //Primer mirem si té un forat davant
		 {
		   //En cas de ser-hi, aturem el moviment lateral i de moment simplment aturem el robot.
		   mtn_lib_stop_mtn(); 
		   if(walk_left() == 0x01) 
		   {
		     state = walk_f;
// 		     state = stop;
		   }
		   else //En cas contrari, continuem amb el moviment
		   {
		     state = walk_l;
		   }
		   break;
		 }
		 else if(exp_adc_get_avg_channel(esquerra) > dist_lateral || is_button_rising_edge(BTN_DOWN)) //Si troba una paret a l'esquerra vol dir que ha arribat al final del recinte, i com que ja ha coemnçat anant cap a la dreta, simplement para. Aquest és un cas teòricament impossible, però cal considerar-lo. Es pot solucionar enviant-lo altre cop cap a l'esquerra
		 {
		   mtn_lib_stop_mtn();
		 }
		 
		 //Comprovem si algun cas anterior ha fet parar el robot, i al mateix temps cridem la funció de moviment. Si algú l'ha fet parar, para. En cas contrari, comprova si s'ha desviat de la trajectòria inicial.
		 if(walk_left() == 0x01)
		 {
// 		   state = stop; 
		   state = walk_r;		   
		 }
		 else
		 {
		   //Comprovem si s'ha desviat. En cas afirmatiu, guardem l'estat des del qual venim a prev i canviem l'estat a corregir.
		   if(compass(valor_base) > comp_error)
		   {
		     prev = walk_l;
		     state = correct_l;
// 		     turn_left();
		   }
		   else if(compass(valor_base) < -comp_error)
		   {
		     prev = walk_l;
		     state = correct_r;
// 		     turn_right();
		   }
		   else
		   {
		     state = walk_l;
		   }		   
		 }
		 break;
    
    //Cas idèntic a walk_l, amb la única diferència de que cap a la dreta. De moment aquest és l'estat inicial.
    case walk_r: if(exp_adc_get_avg_channel(davant) < dist_frontal)
		 {
		   mtn_lib_stop_mtn();
		   if(walk_right() == 0x01)
		   {
		     state = walk_f;
// 		     state  = stop;
		   }
		   else
		   {
		     state = walk_r;
		   }
		   break;
		 }
		 else if(exp_adc_get_avg_channel(dreta) > dist_lateral || is_button_rising_edge(BTN_DOWN))
		 {
		   mtn_lib_stop_mtn();
		 }
		 
		 if(walk_right() == 0x01)
		 {
		   state = walk_l;
// 		   state = stop;
		 }
		 else
		 {
		   if(compass(valor_base) > comp_error)
		   {
		     prev = walk_r;
		     state = correct_l;
// 		     turn_left();
		   }
		   else if(compass(valor_base) < -comp_error)
		   {
		     prev = walk_r;
		     state = correct_r;
// 		     turn_right();
		   }
		   else
		   {
		     state = walk_r;
		   }
		 }
		 break;		 
		 
    //Cas de caminar endavant. Ho fa mentre hi hagi una distància de seguretat
    case walk_f: if(exp_adc_get_avg_channel(davant) > 350)
		 {
		   mtn_lib_stop_mtn();
		 }
		 
		 //Com abans, si res l'ha parat, comprovem desviacions.
		 if(walk_forward() == 0x01)
		 {
		   state = turn;
		 }
		 else
		 {
		   if(compass(valor_base) > comp_error)
		   {
		     prev = walk_f;
		     state = correct_l;
// 		     walk_left();
		   }
		   else if(compass(valor_base) < -comp_error)
		   {
		     prev = walk_f;
		     state = correct_r;
// 		     walk_right();
		   }
		   else
		   {
		     state = walk_f;
		   }
		 }
		 break;
		 
    case turn: if(turn_angle(-180) == 0x01)
	       {
		 state = walk_return;
		 valor_base = suma_angles(valor_base, 1800);
	       }
	       else
	       {
		 state  = turn;
	       }
	       break;
	       
    case walk_return: if(exp_adc_get_avg_channel(davant) > 350)
		      {
			mtn_lib_stop_mtn();
		      }
		      
		      //Com abans, si res l'ha parat, comprovem desviacions.
		      if(walk_forward() == 0x01)
		      {
			state = stop;
		      }
		      else
		      {
			if(compass(valor_base) > comp_error)
			{
			  prev = walk_return;
			  state = correct_l;
      // 		     walk_left();
			}
			else if(compass(valor_base) < -comp_error)
			{
			  prev = walk_return;
			  state = correct_r;
      // 		     walk_right();
			}
			else
			{
			  state = walk_return;
			}
		      }
		      break;

    /* CORRECIÓ */
    //Casos de correció: paren el moviment de correcció (cridat des d'on s'ha detectat l'error i canviat l'estat i tornen a l'estat on eren mitjançant la variable prev.
    
    case correct_l: /*mtn_lib_stop_mtn();*/
		    if(fnct_l() == 0x01)
		    {
		      state = prev;
		    }
		    else
		    {
		      state  = correct_l;
		      mtn_lib_stop_mtn();
		    }
		    break;
		      
    case correct_r: /*mtn_lib_stop_mtn();*/
		    if(fnct_r() == 0x01)
		    {
		      state = prev;
		    }
		    else
		    {
		      state  = correct_r;
		      mtn_lib_stop_mtn();
		    }
		    break;
		      
 
		 
    /* PARADA */
    //Estat estacionari del robot. Possibilitat de tornar a activar el sistema mitjançant un botó.
    
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
		      
