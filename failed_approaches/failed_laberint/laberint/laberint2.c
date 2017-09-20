#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Intent inical de l'estructura del laberint. La base de l'algorisme està bé, però l'execució no és del tot bona. Hi ha problemes sobretot a l'hora de corregir els errors quan es camina recte. COMPROVAR QUE ELS balance_enable_gyro ESTIGUIN BEN POSATS I FER balance_disable_gyro() QUAN ES GIRA!!!!

typedef int bool; //Definim el boleà, que en C no existeix
#define true 1
#define false 0

typedef enum {wait_start, wait_ready, walk_l, walk_r, walk_f, turn, walk_return, stop, correct_l, correct_r} main_states; //Estats de la màquina d'estats
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;

#define err 100

typedef uint8_t (*fnct_ptr)(void);
fnct_ptr fnct_r = turn_right;
// fnct_ptr fnct_r = turn_angle(15);

fnct_ptr fnct_l = turn_left;
// fnct_ptr fnct_l = turn_angle(-15);

main_states prev; //Variable per guardar els estats abans de corregir


//Definim variables dels ports, que canvien segons si és una simulació o no ( més detalls a ../index.txt) 
const bool simulat = false;
adc_t davant, esquerra, dreta; 


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


//Funció per calcular el desviament del robot respecte l'orientació inicial
int compass(int valor_base)
{
        
	int nou_valor, desviament;
	
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

int suma_angles (int a, int b)
{
	int res = a+b;
	if (res < 0 ) res+=3600;
	else if (res>3600){
		res-=3600;
	}
	return res;

}

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

//Funció turn_angle() adaptada a la bno055
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





int valor_base, valor_actual;
static int comp_error = 20; //Màxima desviació permesa per la brúixola
static int dist_frontal = 80;
static int dist_lateral = 250;
 
void user_init(void) //S'executa una sola vegada (setup de l'arduino)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro(); //Alerta: el gyro està activat per defecte, s'ha de desactivar abans de cada gir i tornar a activar posteriorment.
  user_time_set_period(100);
  mtn_lib_init();
  exp_adc_start();
//   exp_compass_start();
  exp_bno055_start();
  if(is_button_pressed(BTN_UP))
  {
    exp_bno055_erase_calibration();
  }
  while(exp_bno055_is_calibrated()!=0x01)
  {
    _delay_ms(100);
  }
  
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

void user_loop(void) //Es repeteix infinitament (equivalent al loop d'arduino o a while(1))
{
  static main_states state=wait_start;

  switch(state)
  {
    /* INICIALITZACIÓ */
    //Wait_start i wait_ready són les inicialitzacions
    
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
		   //En cas de ser-hi, aturem el moviment lateral i caminem endavant.
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
    
    //Cas idèntic a walk_l, amb la única diferència de que cap a la dreta.
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
    //Gira 180 graus per tornar pel camí per on ha vingut.
    case turn: if(balance_is_gyro_enabled()) //Si el balance està activat, el desactivem
	       {
		 balance_disable_gyro();
	       }
		    
	       if(gira(-180) == 0x01) //Comprovem si ha completat el gir.
	       {
		 state = walk_return; 
		 valor_base = suma_angles(valor_base, 1800); //Definim el 0 del compass desplaçat 180 graus respete l'inicial
		 balance_enable_gyro(); //Tornem a activar el balance
	       }
	       else
	       {
		 state  = turn;
	       }
	       break;
    //Cas per tornar un cop s'ha arribat al final
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
    //Casos de correció: Fan *una* iteració del moviment de correcció i el paren. Quan ha acabat tornen a l'estat on eren mitjançant la variable prev. POSSOBLE MILLORA: En lloc de fer una sola iteració, es podria fer que anessin corregint mentre hi hagués error, resultant un moviment més fluid. Ara per ara, es comprova si hi ha error als estats de moviment, i llavors es criden les funcions de moviment entre les correcions.
    
    case correct_l:/* if(balance_is_gyro_enabled())*/ //Com que és un gir, si hi ha el gyro activat, el desactivem
// 		    {
		      balance_disable_gyro();
// 		    }
      
		    if(fnct_l() == 0x01)
		    {
		      state = prev;
		      balance_enable_gyro(); //Després del gir tornem a activar-lo
		    }
		    else
		    {
		      state  = correct_l;
		      mtn_lib_stop_mtn();
		    }
		    break;
		      
    case correct_r: /*if(balance_is_gyro_enabled())*/
// 		    {
			balance_disable_gyro();			
// 		    }
		    
		    if(fnct_r() == 0x01)
		    {
		      state = prev;
		      balance_enable_gyro();
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
		      
