#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Versió del laberint_compensating amb algunes millores estructurals

/////////////////////////////////////////////////////////// 
// Definició de variables globals i tipus 
///////////////////////////////////////////////////////////


typedef int bool; //Definim el boleà, que en C no existeix
#define true 1
#define false 0

#define err 100 //Màxim error permès a gira.

typedef enum {wait_start, wait_ready, walk_l, walk_r, ready_walk_f, walk_f, turn, ready_walk_return, walk_return, stop, correct_l, correct_r} main_states; //Estats de la màquina d'estats
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;

typedef uint8_t (*fnct_ptr)(void); //Parametrització de les funcions per a canvis ràpids
fnct_ptr fnct_r = turn_right;
// fnct_ptr fnct_r = turn_angle(15);

fnct_ptr fnct_l = turn_left;
// fnct_ptr fnct_l = turn_angle(-15);

main_states prev, next; //Variable de tipus main_states (estat) que servirà per guardar l'estat anterior a corregir per poder-hi tornar més fàcilment.


//Definim variables dels ports, que canvien segons si és una simulació o no ( més detalls a ../index.txt) 
const bool simulat = false;
adc_t davant, esquerra, dreta; 


/////////////////////////////////////////////////////////// 
//  Definició de funcions pròpies
///////////////////////////////////////////////////////////

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
	if(res < 0 )
	{
	  res+=3600;
	}
	else if(res>3600)
	{
	  res-=3600;
	}
	return res;
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


/////////////////////////////////////////////////////////// 
// INIT i variables específiques
///////////////////////////////////////////////////////////


int cont = 0;
int valor_base, valor_actual;

//Parametritzacions de valors límit dels sensors
static int comp_error = 15;
static int dist_frontal = 80;
static int dist_lateral = 250;
 
void user_init(void) //S'executa una sola vegada (setup de l'arduino)
{
  serial_console_init(57600);
  balance_init();
  balance_calibrate_gyro();
  balance_enable_gyro(); //Alerta: el gyro està activat per defecte, s'ha de desactivar abans de girar i tornar a activar quan ha acabat. No sabem si les funcions *_compensating() necessiten tenir-lo activat o no.
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
  
  if(simulat == true) //Es defineixen els ports segons si es tracta d'una simulació o no. Més info a l'índex
  {
    davant = ADC5;
    esquerra = ADC3;
    dreta = ADC6;
  }
  else
  {
    davant = ADC6;
    esquerra = ADC4;
    dreta = ADC7;
  }
 
}

/////////////////////////////////////////////////////////// 
// Màquina d'estats
///////////////////////////////////////////////////////////

void user_loop(void) //Es repeteix infinitament (equivalent al loop d'arduino o a while(1))
{
  static main_states state=wait_start;

  switch(state)
  {
    /* INICIALITZACIÓ */
    //Wait_start i wait_ready són les inicialitzacions. El robot s'aixeca i executa la pàgina 31 per posar-se en la posició inicial preparat per caminar.
    
    case wait_start: if(is_button_rising_edge(BTN_START))
                     {
		       valor_base = bno055_correction(exp_bno055_get_heading());
                       action_set_page(31);
                       action_start_page();
                       state=wait_ready;
                     }
                     else
		     {
                       state=wait_start;
		     }
                     break;
		     
    case wait_ready: if(is_action_running())
		     {
		       state=wait_ready;
		     }
                     else
		     {
		       //Comencem caminant cap a l'esquerra (perquè abans caminava millor cap a aquest costat)
		       state=walk_r; 
		     }
                     break;

		     
    /* MOVIMENT */
    //Estat per caminar cap a diverses direccions
    
    //Caminar lateralment cap a l'esquerra
    case walk_l:  if(!balance_is_gyro_enabled())
		  {
		    balance_enable_gyro();
		  }
		  
		  //Comprovem si s'ha desviat. En cas afirmatiu, guardem l'estat des del qual venim a prev i canviem l'estat a corregir.
		  if(compass(valor_base) > comp_error)
		  {
		    prev = walk_l;
		    state = correct_l;
		    break;
		  }
		  else if(compass(valor_base) < -comp_error)
		  {
		    prev = walk_l;
		    state = correct_r;
		    break;
		  }
		  else //Si no s'ha desviat
		  {
		    if(exp_adc_get_avg_channel(davant) < dist_frontal) //Comprovem si hi ha un forat davant
		    {
		      mtn_lib_stop_mtn();
		       next = ready_walk_f;
// 		      next = stop;
		    }
		    else if(exp_adc_get_avg_channel(esquerra) > dist_lateral) //Comprovem si ha arribat a la paret
		    {
		      mtn_lib_stop_mtn();
		      next = walk_r;
		    }
		    
		    if(walk_left() == 0x01)
		    {
		      state = next;
		    }
		    else
		    {
		      state = walk_l;
		    } 
		  }
		  break;
		  
		  
		  
    
    //Cas idèntic a walk_l, amb la única diferència de que cap a la dreta.
    case walk_r:  if(!balance_is_gyro_enabled())
		  {
		    balance_enable_gyro();
		  }
		  
		  //Comprovem si s'ha desviat. En cas afirmatiu, guardem l'estat des del qual venim a prev i canviem l'estat a corregir.
		  if(compass(valor_base) > comp_error)
		  {
		    prev = walk_r;
		    state = correct_l;
		    break;
		  }
		  else if(compass(valor_base) < -comp_error)
		  {
		    prev = walk_r;
		    state = correct_r;
		    break;
		  }
		  else //Si no s'ha desviat
		  {
		    if(exp_adc_get_avg_channel(davant) < dist_frontal) //Comprovem si hi ha un forat davant
		    {
		      mtn_lib_stop_mtn();
		       next = ready_walk_f;
// 		      next = stop;
		    }
		    else if(exp_adc_get_avg_channel(dreta) > dist_lateral) //Comprovem si ha arribat a la paret
		    {
		      mtn_lib_stop_mtn();
		      next = walk_l;
		    }
		    
		    if(walk_right() == 0x01)
		    {
		      state = next;
		    }
		    else
		    {
		      state = walk_r;
		    } 
		  }
		  break;
		 
//Cas per corregir la desviaió en començar a tirar endavant		  
    case ready_walk_f:  if(compass(valor_base) > comp_error)
			{
			  prev = ready_walk_f;
			  state = correct_l;
			}
			else if(compass(valor_base) < -comp_error)
			{
			  prev = ready_walk_f;
			  state = correct_r;
			}
			else
			{
			  state = walk_f;
			}
			break;
			
    //Cas de caminar endavant. Ho fa mentre hi hagi una distància de seguretat
    case walk_f:    if(!balance_is_gyro_enabled())
		    {
		      balance_enable_gyro();
		    }
		    
		    if(exp_adc_get_avg_channel(davant) > 350)
		    {
		      mtn_lib_stop_mtn();
		    }
		    

		    if(walk_forward_compensating(valor_base, bno055_correction(exp_bno055_get_heading())) == 0x01)
		    {
		      state = turn;
		    }
		    else
		    {
		      state = walk_f;
		    }
		 break;


		 
    case turn:   turn_led_on(LED_AUX);
		if(balance_is_gyro_enabled()) //Si el balance està activat, el desactivem
	       {
		 balance_disable_gyro();
	       }
	       if(gira(180) == 0x01)
	       {
		 state = ready_walk_return;
// 		 state = stop;
		 valor_base = suma_angles(valor_base, 1800);
// 		 valor_base = 400;
	       }
	       else
	       {
		 state  = turn;
	       }
	       break;
	       
    case ready_walk_return:  if(compass(valor_base) > comp_error)
			{
			  prev = ready_walk_return;
			  state = correct_l;
			}
			else if(compass(valor_base) < -comp_error)
			{
			  prev = ready_walk_return;
			  state = correct_r;
			}
			else
			{
			  state = walk_return;
			}
			break;
	       
    case walk_return: 	if(!balance_is_gyro_enabled())
			{
			  balance_enable_gyro();
			}
			
			if(exp_adc_get_avg_channel(davant) > 350)
			{
			  mtn_lib_stop_mtn();
			}
			

			if(walk_forward_compensating(valor_base, bno055_correction(exp_bno055_get_heading())) == 0x01)
			{
			  state = turn;
			}
			else
			{
			  state = walk_f;
			}
			break;
    /* CORRECIÓ */
    //Casos de correció: paren el moviment de correcció (cridat des d'on s'ha detectat l'error i canviat l'estat i tornen a l'estat on eren mitjançant la variable prev.
    
    case correct_l: if(balance_is_gyro_enabled())
		    {
		      balance_disable_gyro();
		    }
    
		    if(turn_left() == 0x01)
		    {
		      state = prev;
		    }
		    else
		    {
		      state  = correct_l;
		      mtn_lib_stop_mtn();
		    }
		    break;
		      
    case correct_r: if(balance_is_gyro_enabled())
		    {	
		      balance_disable_gyro();
		    }
		    
		    if(turn_right() == 0x01)
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
		      
