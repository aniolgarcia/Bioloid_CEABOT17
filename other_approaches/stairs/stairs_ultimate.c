
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include "action_id.h"
#include "small_steps.h"


typedef enum {wait_start,wait_ready,walk_to_stairs,stop, up_stairs,measure10, walk, pre_walk_top,  walk_top,down_stairs} main_states;
// Walks up the stairs using "Forward-Start + Forward-End + Wait and measure" to get to the stairs
//Climbs the stairs, and when it has climbed 3 steps, it goes forward fast.Then it goes down 3 steps
//Working on 16/06/2017
#define left_foot_forward 			GPIO1	
#define left_foot_forward_down		GPIO2
#define left_foot_lateral_down		GPIO3
#define right_foot_forward			GPIO18
#define right_foot_forward_down		GPIO17
#define right_foot_lateral_down		GPIO16

#define STEPS_TOP 8
#define MEASURE_TIME 500 //0.5 second measuring - could be less?
#define TURN_VALUE 200//what is the minimum value of deviation when we should turn

//robot standing = 0 0 0 0 1 1

int measureFDL[3]; // nº of zeros | nº of ones | filtered result
int measureFDR[3];
int measureLDL[3];
int measureLDR[3];
int measureFFL[3];
int measureFFR[3];

int compass_diff (int ini, int actual){ //Calculates the difference between initial orientation and actual orientation of compass.
    short int inc = actual - ini;
    if (inc<-1800) inc+=3600;
    else if (inc>1800) inc-=3600;
    return inc;
} //return value is between -1800 and 1800


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
    measureFFR[2] = (measureFFR[1]>measureFFR[0]);
}
void user_init(void)
{
    serial_console_init(57600);
    balance_init();
    balance_calibrate_gyro();
    balance_enable_gyro();
    mtn_lib_init();
    exp_adc_start();
    exp_compass_start();
    init_user_time(); //is it necessary?
}

void user_loop(void)
{
    static main_states state=wait_start;
    static fallen_t fall_state=robot_standing;
    static int comp_ini=0;
    static int staircount = 0;
    static int walk_stairs_val=0;
    static int walk_back_val=0;
    int diff = 0;
    //cm510_printf ("state=%d,diff=%d\n",state,diff);

    diff = compass_diff (comp_ini, exp_compass_get_avg_heading());
	
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
                reset_measures();
                comp_ini = exp_compass_get_avg_heading();
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
            staircount++;
			if(staircount==3)
			{
				state = walk_top;
// 				state=pre_walk_top;
				reset_steps();
			}else
			{
				state=walk;
			}
        }
        else
		{
            state=up_stairs;
        }
        break;
		
	case pre_walk_top:
		if (walk_forward_compensating(comp_ini, exp_compass_get_avg_heading()))
		{
			state = walk_top;
		}
		else if(get_steps() >= STEPS_TOP)
		{
			mtn_lib_stop_mtn();
			state = pre_walk_top;
		}
		break;
		
		
	case walk_top:
		if (exp_gpio_get_value(right_foot_forward_down)==0 || exp_gpio_get_value(left_foot_forward_down)==0)
		{
			mtn_lib_stop_mtn();
		}
		
		if(walk_forward_compensating(comp_ini,exp_compass_get_avg_heading())==0x01)
// 		if(fast_walk_forward() == 0x01)
		{
			state = down_stairs;
			fall_state = balance_robot_has_fallen();
			stairs_down_process();
		}
		else
		{
			state = walk_top;
		}
		break;
		
	 case down_stairs:
        if(stairs_up_process(fall_state)==0x01)
		{
            state=walk_top;

        }
        else
		{
            state=down_stairs;
        }
        break;
		
	}
}