#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa de sumo. En principi es va movent fent atacs laterals i quan detecta l'enemic ataca cap a aqella direcció. PROBLEMA: Es mou massa i fa masses voltes, així que és MOLT difícil arribar a detectar l'enemic. S'hauria de fer una cerca de manera més estàtica i actuar de manera més inteligent. 
//APUNT: Poseu algun sensor de llarga distància, segurament funcionarà millot; amb els de mitja en tota la competició de sumo vam detectar 1 sol robot. 

typedef enum {wait_start, wait_5, wait_ready,wait_ready2, gir_dreta, gir_dreta2,attack0, attack1, attack2, attack3, attack4, attack5, attack6, jump_davant, jump_esquerra, jump_dreta, get_up, attack45} main_states;
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;


#define err 100

main_states prev, next;

adc_t davant, esquerra, dreta;



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

//Funció turn_angle() adaptada a la bno055 ALERTA: No suma el nombre de graus a la posició actual, sinó que va a aquella posició de la brúixola!!!
uint8_t gira(int angle){
	static turn_states s = t_init;
	static int comp_ini = 0;
	static int comp_end = 0;
	int done = 0;

	switch (s){
		case t_init:
			comp_ini = bno055_correction(exp_bno055_get_heading());
			comp_end = suma_angles (comp_ini,angle*10);
// 			comp_end = angle;

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
    davant = ADC6;
    esquerra = ADC4;
    dreta = ADC7;

}



static int valor_base;
//user_time_set_period(5000);
void user_loop(void)
{
    static main_states state=wait_start;
    
    
    switch(state)
    {
        case wait_start: if(user_time_is_period_done())
		      {
                  
              }
            if(is_button_rising_edge(BTN_START))
            {
                valor_base = bno055_correction(exp_bno055_get_heading());	
                action_set_page(31);
                action_start_page();
                state=wait_5;
                next=gir_dreta;
				user_time_set_period(5000);
                state=wait_5;
            }
            else
                state=wait_start;
            break;
			
			    case wait_5: 	 if(user_time_is_period_done())
                    {
                        state = wait_ready;
                    }
                    else
                    {
                        state = wait_5;
                    }
                    break;
			
            
        case wait_ready:if(is_action_running())
        {
            state=wait_ready;
        }
        else
        {
            state=next;
        }
            break;
            
        case gir_dreta:
            if (gira(90)==0x01){ //Fa el primer gir en la direccio dreta de 90 graus!
                state = attack1;
            }
                        if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            break;
			
         case attack0:
            action_set_page(232);
            action_start_page();
            state=wait_ready;
            next=attack1;
            if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            break;           
            
        case attack1:
            action_set_page(232);
            action_start_page();
            state=wait_ready;
            next=attack2;
            if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            break;
            
        case attack2:
            action_set_page(232);  
            action_start_page();
            state=wait_ready;
            next=gir_dreta2;
            
            if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            
            
            break;
            
            
        case gir_dreta2:
            if (gira(90)==0x01){
                state = attack3;
            }
                        if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            break;
            
            
        case attack3:
            action_set_page(232);
            action_start_page();
            state=wait_ready;
            next=attack4;

            if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            
            
            break;
            
            
        case attack4:
            action_set_page(231);
            action_start_page();
            state=wait_ready;
            next=attack45;
 
            if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            break;
			
        case attack45:
            action_set_page(231);
            action_start_page();
            state=wait_ready;
            next=attack5;
 
            if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            break; 
			
        case attack5:
            action_set_page(232);
            action_start_page();
            state=wait_ready;
            next=attack6;
         
            if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            break;
            
        case attack6:
            action_set_page(232);
            action_start_page();
            state=wait_ready;
            next=gir_dreta;
            
            if(exp_adc_get_avg_channel(davant) > 350){
                next=jump_davant;
                prev=attack2;
            }
            else if(exp_adc_get_avg_channel(dreta) > 200){
                next=jump_dreta;
                prev=attack2;
            }
          
            else if(exp_adc_get_avg_channel(esquerra) > 350){
                next=jump_esquerra;
                prev=attack2;
            }
            break;
            
        case jump_dreta:
            action_set_page(233);
            action_start_page();
            state=wait_ready;
            next=prev;
            
            break;
            
        case jump_esquerra:
            action_set_page(234);
            action_start_page();
            state=wait_ready;
            next=prev;
            break;
            
        case jump_davant:
            action_set_page(235);
            action_start_page();
            state=wait_ready2;
            break;
            
        case wait_ready2:
            if(is_action_running())
            {
                state=wait_ready2;
            }
            else
            {
                state=get_up;
            }

            break;
            
        case get_up:
            action_set_page(236);
            action_start_page();
            state=wait_ready;
            next=prev;
            break;
            
            
            
    }
}

