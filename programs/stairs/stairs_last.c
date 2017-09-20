
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include "action_id.h"
#include "small_steps.h"


typedef enum {wait_start,wait_ready,stop, up_stairs, walk} main_states; //Definició d'estats de la màquina principal

//Versió final de les escales. Simplement camina endavant fins que troba l'escala i la puja. ALERTA!: Aquest programa puja escales INFINITAMENT, mai les baixa (no vam tenir temps d'implementar-ho correctament...)


//Definim els ports als quals estan connectats els sensors
#define left_foot_forward 			GPIO1	
#define left_foot_forward_down		GPIO2
#define left_foot_lateral_down		GPIO3
#define right_foot_forward			GPIO18
#define right_foot_forward_down		GPIO17
#define right_foot_lateral_down		GPIO16

#define STEPS_TOP 7
#define MEASURE_TIME 500 
#define TURN_VALUE 200


void user_init(void)
{
    serial_console_init(57600);
    balance_init();
    balance_calibrate_gyro();
    balance_enable_gyro();
    mtn_lib_init();
    exp_adc_start();
    exp_compass_start();
    init_user_time();
}

void user_loop(void)
{
    static main_states state=wait_start;
    static fallen_t fall_state=robot_standing;

	switch(state)
	{
	case wait_start: 
		if(is_button_rising_edge(BTN_START))
        {
            user_time_set_one_time (5000);
            action_set_page(30);
            action_start_page();
            state=wait_ready;
        }
        else
            state=wait_start;
        break;
		
    case wait_ready: 
		if(is_action_running())
		{       
        	state=wait_ready;
		}
        else
        {
            if (user_time_is_done())
			{
                state=walk;
            }
        }
        break;


	case walk:
		if (exp_gpio_get_value(right_foot_forward)==0 || exp_gpio_get_value(left_foot_forward)==0)
		{
			mtn_lib_stop_mtn();
		}
		
		if(fast_walk_forward()==0x01)
		{
			state = up_stairs;
			fall_state = balance_robot_has_fallen();
			stairs_up_process(fall_state);
		}
		else
		{
			state = walk;
		}
		break;
		
	case stop:
		if(is_button_rising_edge(BTN_UP))
		{
			state = walk;
		}
		else
		{
			state = stop;
		}
	break;
	
	 case up_stairs:
        if(stairs_up_process(fall_state)==0x01)
		{
            state=walk;
        }

        break;
		
	
	}
}
    
    