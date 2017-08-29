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

typedef enum {wait_start, wait_ready,wait_5, walk_l, walk_r, walk_f, turn, walk_f_return, stop} main_states; //Estats de la màquina d'estats
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;
typedef enum {w_move, w_correct_l, w_correct_r} walk_to_wall_states;

typedef uint8_t (*fnct_ptr)(void); //Parametrització de les funcions per a canvis ràpids
fnct_ptr fnct_r = turn_right;
// fnct_ptr fnct_r = turn_angle(15);

fnct_ptr fnct_l = turn_left;
// fnct_ptr fnct_l = turn_angle(-15);

main_states prev, older_prev, next; //Variable de tipus main_states (estat) que servirà per guardar l'estat anterior a corregir per poder-hi tornar més fàcilment.


//Definim variables dels ports, que canvien segons si és una simulació o no ( més detalls a ../index.txt)
const bool simulat = false;
adc_t davant, esquerra, dreta;

bool forat = false;

int cont = 0;
int valor_base, valor_actual, prev_distance, fnct_value;

//Parametritzacions de valors límit dels sensors
static int comp_error = 15;
static int dist_frontal = 80;
static int dist_lateral = 250;

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
//     angle += 180; //Crec que la funció treballa en un rang de [0, 3600], no en [-1800, 1800] No, gira utilitza un rang de [-1800, 1800]!
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

uint8_t walk_to_wall(fnct_ptr mov_fnct, adc_t sensor, int distance)
{
    static walk_to_wall_states w = w_move;
    int done = 0;

    switch (w)
    {
        case w_move:
            if(!balance_is_gyro_enabled())
            {
                balance_enable_gyro();
            }

            if(compass(valor_base) > comp_error) //Comprova si està desviat
            {
                mtn_lib_stop_mtn();
                w = w_correct_l;
            }
            else if(compass(valor_base) < -comp_error)
            {
                mtn_lib_stop_mtn();
                w = w_correct_r;
            }
            else
            {
                if(exp_adc_get_avg_channel(sensor) > distance)
                {
                    mtn_lib_stop_mtn();
                    done = 0x01;
                }
            }

            if(mov_fnct() != 0x01)
            {
                w = w_move;
            }
            break;


        case w_correct_l:
            if(balance_is_gyro_enabled())
            {
                balance_disable_gyro();
            }

            if(turn_left() == 0x01)
            {
                w = w_move;
            }
            else
            {
                w = w_correct_l;
                mtn_lib_stop_mtn();
            }
            break;

        case w_correct_r:
            if(balance_is_gyro_enabled())
            {
                balance_disable_gyro();
            }

            if(turn_right() == 0x01)
            {
                w = w_move;
            }
            else
            {
                w  = w_correct_r;
                mtn_lib_stop_mtn();
            }
            break;


    }
    return done;
}

uint8_t walk_to_wall_exception(fnct_ptr mov_fnct, adc_t sensor, int distance, adc_t sensor_exception, int distance_exception)
{
    static walk_to_wall_states w = w_move;
    int done = 0;

    switch (w)
    {
        case w_move:
            if(!balance_is_gyro_enabled())
            {
                balance_enable_gyro();
            }

            if(compass(valor_base) > comp_error) //Comprova si està desviat
            {
                mtn_lib_stop_mtn();
                w = w_correct_l;
            }
            else if(compass(valor_base) < -comp_error)
            {
                mtn_lib_stop_mtn();
                w = w_correct_r;
            }
            else
            {
                if(exp_adc_get_avg_channel(sensor) > distance)
                {
                    mtn_lib_stop_mtn();
                    done = 0x01;
                }

                if(exp_adc_get_avg_channel(sensor_exception) > distance_exception)
                {
                    mtn_lib_stop_mtn();
                    done = 0x02;
                }
            }

            if(mov_fnct() != 0x01)
            {
                w = w_move;
            }
            break;


        case w_correct_l:
            if(balance_is_gyro_enabled())
            {
                balance_disable_gyro();
            }

            if(turn_left() == 0x01)
            {
                w = w_move;
            }
            else
            {
                w  = w_correct_l;
                mtn_lib_stop_mtn();
            }
            break;

        case w_correct_r:
            if(balance_is_gyro_enabled())
            {
                balance_disable_gyro();
            }

            if(turn_right() == 0x01)
            {
                w = w_move;
            }
            else
            {
                w  = w_correct_r;
                mtn_lib_stop_mtn();
            }
            break;


    }
    return done;
}

///////////////////////////////////////////////////////////
// INIT i variables específiques
///////////////////////////////////////////////////////////


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

    case wait_start:
		if(is_button_rising_edge(BTN_START))
        {
            valor_base = bno055_correction(exp_bno055_get_heading());
            action_set_page(30);
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
            state=wait_ready;
        }
        else
        {
        //Comencem caminant cap a l'esquerra (perquè abans caminava millor cap a aquest costat)
            state = walk_r;
        }
        break;

    case walk_r:
        fnct_value = walk_to_wall_exception(walk_right, dreta, dist_lateral, davant, dist_frontal);
        if(fnct_value == 0x01)
        {
            state = walk_l;
        }
        else if (fnct_value == 0x02)
        {
            state = walk_f;
        }
        else
        {
            state = walk_r;
        }
        break;

        case walk_l:
            fnct_value = walk_to_wall_exception(walk_left, esquerra, dist_lateral, davant, dist_frontal);
            if(fnct_value == 0x01)
            {
                state = walk_r;
            }
            else if (fnct_value == 0x02)
            {
                state = walk_f;
            }
            else
            {
                state = walk_l;
            }
            break;

        case walk_f:
            if(walk_to_wall(walk_forward, davant, dist_frontal) == 0x01)
            {
                state = turn;
            }
            else
            {
                state = walk_f;
            }
            break;

        case turn:
            if(balance_is_gyro_enabled()) //Si el balance està activat, el desactivem
            {
                balance_disable_gyro();
            }

            if(gira(175) == 0x01)
            {
                state = walk_f_return;
                if(valor_base > 0) //Fem que el seu valor base sigui l'oposat al que tenia
                {
                    valor_base -= 1800;
                }
                else
                {
                    valor_base += 1800;
                }
            }
            else
            {
                state  = turn;
            }
            break;

        case walk_f_return:
            if(walk_to_wall(walk_forward, davant, dist_frontal) == 0x01)
            {
                state = stop;
            }
            else
            {
                state = walk_f_return;
            }
            break;

        case stop:
            printf("STOP!\n");
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
