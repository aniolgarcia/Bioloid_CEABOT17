
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include "action_id.h"
#include "small_steps.h"
///////////////////////////////////////////////////////////
// Definició de variables globals i tipus
///////////////////////////////////////////////////////////


typedef int bool; //Definim el boleà, que en C no existeix
#define true 1
#define false 0

#define err 100 //Màxim error permès a gira.

typedef enum {wait_start, wait_ready,wait_5, walk_l, walk_r, ready_walk_f, walk_f, turn, ready_walk_return, walk_return, stop, correct_l, correct_r} main_states; //Estats de la màquina d'estats

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

bool forat = false;

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

//Funció turn_angle() adaptada a la bno055 ALERTA: No suma el nombre de graus a la posició actual, sinó que va a aquella posició de la brúixola!!!
uint8_t gira(int angle){
    static turn_states s = t_init;
    static int comp_ini = 0;
    static int comp_end = 0;
    int done = 0;
    
    switch (s){
        case t_init:
            comp_ini = bno055_correction(exp_bno055_get_heading());
            // 			comp_end = suma_angles (comp_ini,angle*10);
            comp_end = angle;
            
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
//  Definició de funcions Laia Madrid 2016
///////////////////////////////////////////////////////////

// Walks up the stairs using "Forward-Start + Forward-End + Wait and measure" to get to the stairs
//Climbs the stairs, and when it has climbed 3 steps, it goes forward fast.Then it goes down 3 steps
//Working on 16/06/2017
#define left_foot_forward 			GPIO1	
#define left_foot_forward_down		GPIO2
#define left_foot_lateral_down		GPIO3
#define right_foot_forward			GPIO18
#define right_foot_forward_down		GPIO17
#define right_foot_lateral_down		GPIO16

#define STEPS_TOP 7
#define MEASURE_TIME 500 //0.5 second measuring - could be less?
#define TURN_VALUE 200//what is the minimum value of deviation when we should turn

//robot standing = 0 0 0 0 1 1

int measureFDL[3]; // nº of zeros | nº of ones | filtered result
int measureFDR[3];
int measureLDL[3];
int measureLDR[3];
int measureFFL[3];
int measureFFR[3];

void reset_measures(void){
    measureFDL[0]=0;
    measureFDL[1]=0;
    measureFDR[0]=0;
    measureFDR[1]=0;
    measureLDL[0]=0;
    measureLDL[1]=0;
    measureLDR[0]=0;
    measureLDR[1]=0;
    measureFFL[0]=0;
    measureFFL[1]=0;
    measureFFR[0]=0;
    measureFFR[1]=0;
}
void set_measures(void){  // 0 = obstacle in front, 1 = no obstacle in front (in real robot. )
    measureFDL[2] = (measureFDL[1]>measureFDL[0]);
    measureFDR[2] = (measureFDR[1]>measureFDR[0]);
    measureLDL[2] = (measureLDL[1]>measureLDL[0]);
    measureLDR[2] = (measureLDR[1]>measureLDR[0]);
    measureFFL[2] = (measureFFL[1]>measureFFL[0]);
    mfall_state = balance_robot_has_fallen();easureFFR[2] = (measureFFR[1]>measureFFR[0]);
}

///////////////////////////////////////////////////////////
// INIT i variables específiques
///////////////////////////////////////////////////////////


int cont = 0;
int valor_base, valor_actual;

//Parametritzacions de valors límit dels sensors
static int comp_error = 15;

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
}

///////////////////////////////////////////////////////////
// Màquina d'estats
///////////////////////////////////////////////////////////

void user_loop(void)
{
    static main_states state=wait_start;
    static fallen_t fall_state=robot_standing;
    static int comp_ini=0;
    static int staircount = 0;
    static int walk_stairs_val=0;
    static int walk_back_val=0;
    static int diff = 0;
    //cm510_printf ("state=%d,diff=%fall_state = balance_robot_has_fallen();d\n",state,diff);

    diff = compass_diff (comp_ini, exp_compass_get_avg_heading());
    switch(state)
    {
    case wait_start: if(is_button_rising_edge(BTN_START))
        {
            user_time_set_one_time (5000);
            action_set_page(31);
            action_start_page();
            state=wait_ready;
			next=read_sensor_up;
        }
        else
            state=wait_start;
        break;
    case wait_ready: if(is_action_running())
            state=wait_ready;
        else
        {
            if (user_time_is_done()){
                reset_measures();
                comp_ini = exp_compass_get_avg_heading();
                state=next;
		}
        break;

	case read_sensor_up:
		fall_state = balance_robot_has_fallen();
        if(fall_state!=robot_standing) {
			staircount--;
		}
		if (exp_gpio_get_value(left_foot_forward) == 0 && exp_gpio_get_value(right_foot_forward)==0){
			state=walK_forward;
			next=read_sensor_up;
		}
		else if (staircount<3){
			state=up_stair;
			staircount++;	
		}else
			state=read_sensor_down;
        break;

	case up_stair:
        action_set_page(/*up*/);
        action_start_page();
        state=wait_ready;
		next=read_sensor_up;

        break;

	case down_stair:
        action_set_page(/*down*/);
        action_start_page();
        state=wait_ready;
		next=read_sensor_down;

        break;			
			
	case walK_forward:
		if(walk_right() == 0x01)
        {
            state =next;
		}	
        break;
	}
}
			
			
			
			
			
			
			
			
