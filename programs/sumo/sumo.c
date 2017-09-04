#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Base del programa de sumo

typedef enum {wait_start, wait_ready,wait_ready2, gir_dreta, gir_dreta2, attack1, attack2, attack3, attack4, attack5, attack6, jump_davant, jump_esquerra, jump_dreta, get_up} main_states;
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;


#define err 100

main_states prev, next;

//Funció per calcular el desviament del robot respecte l'orientació inicial. És com compass però amb la posició inicial parametritzada.

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
    // balance_enable_gyro(); //giro desactivat!!!!
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
user_time_set_period(5000);
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
                state=wait_ready;
                next=gir_dreta
            }
            else
                state=wait_start;
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
                state = attack1
            }
            break;
            
            
        case attack1:
            action_set_page(/*latreal_attack_esquerra*/);
            action_start_page();
            state=wait_ready;
            next=attack2;
            
            if(exp_adc_get_avg_channel(dreta) > 400){
                next=jump_dreta;
                prev=attack2;
            }
            
            if(exp_adc_get_avg_channel(davant) > 400){
                next=jump_davant;
                prev=attack2;
            }
            
            if(exp_adc_get_avg_channel(esquerra) > 400){
                next=jump_esquerra;
                prev=attack2;
            }
            break;
            
        case attack2:
            action_set_page(/*latreal_attack_esquerra*/);  int s
            action_start_page();
            state=wait_ready;
            next=gir_dreta2;
            
            if(exp_adc_get_avg_channel(dreta) > 400){
                next=jump_dreta;
                prev=gir_dreta2;
            }
            
            if(exp_adc_get_avg_channel(davant) > 400){
                next=jump_davant;
                prev=gir_dreta2;
            }
            
            if(exp_adc_get_avg_channel(esquerra) > 400){
                next=jump_esquerra;
                prev=gir_dreta2;
            }
            
            break;
            
            
        case gir_dreta2:
            if (gira(90)==0x01){
                state = attack3;
            }
            break;
            
            
        case attack3:
            action_set_page(/*latreal_attack_esquerra*/);
            action_start_page();
            state=wait_ready;
            next=attack4;

            if(exp_adc_get_avg_channel(dreta) > 400){
                next=jump_dreta;
                prev=attack4;
            }
            
            if(exp_adc_get_avg_channel(davant) > 400){
                next=jump_davant;
                prev=attack4;
            }
            
            if(exp_adc_get_avg_channel(esquerra) > 400){
                next=jump_esquerra;
                prev=attack4;
            }
            
            break;
            
            
        case attack4:
            action_set_page(/*latreal_attack_dreta*/);
            action_start_page();
            state=wait_ready;
            next=attack5;
 
            if(exp_adc_get_avg_channel(dreta) > 400){
                next=jump_dreta;
                prev=attack5;
            }
            
            if(exp_adc_get_avg_channel(davant) > 400){
                next=jump_davant;
                prev=attack5;
            }
            
            if(exp_adc_get_avg_channel(esquerra) > 400){
                next=jump_esquerra;
                prev=attack5;
            }
            
            break;
            
        case attack5:
            action_set_page(/*latreal_attack_esquerra*/);
            action_start_page();
            state=wait_ready;
            next=attack6;
         
            if(exp_adc_get_avg_channel(dreta) > 400){
                next=jump_dreta;
                prev=attack6;
            }
            
            if(exp_adc_get_avg_channel(davant) > 400){
                next=jump_davant;
                prev=attack6;
            }
            
            if(exp_adc_get_avg_channel(esquerra) > 400){
                next=jump_esquerra;
                prev=attack6;
            }
            
            break;
            
        case attack6:
            action_set_page(/*latreal_attack_esquerra*/);
            action_start_page();
            state=wait_ready;
            next=gir_dreta;
            
            if(exp_adc_get_avg_channel(dreta) > 400){
                next=jump_dreta;
                prev=gir_dreta;
            }
            
            if(exp_adc_get_avg_channel(davant) > 400){
                next=jump_davant;
                prev=gir_dreta;
            }
            
            if(exp_adc_get_avg_channel(esquerra) > 400){
                next=jump_esquerra;
                prev=gir_dreta;
            }
            
            break;
            
        case jump_dreta:
            action_set_page(/*salt_dreta*/);
            action_start_page();
            state=wait_ready;
            next=prev;
            
            break;
            
        case jump_esquerra:
            action_set_page(/*salt_esquerra*/);
            action_start_page();
            state=wait_ready;
            next=prev;
            break;
            
        case jump_davant:
            action_set_page(/*salt_frontal*/);
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
            
        case get_up;
            action_set_page(/*get_up*/);
            action_start_page();
            state=wait_ready;
            next=prev;
            break;
            
            
            
    }
}

