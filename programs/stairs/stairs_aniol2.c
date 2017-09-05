
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include "action_id.h"
#include "small_steps.h"


typedef enum {wait_start,wait_ready,walk_first,walk_to_stairs,measure10,walk_back_stairs,up_stairs1,walk_top_stairs,get_up,turn_left_little,turn_right_little,down_stairs2,move_right,move_left} main_states;
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
    static int diff = 0;
    //cm510_printf ("state=%d,diff=%d\n",state,diff);

    diff = compass_diff (comp_ini, exp_compass_get_avg_heading());
    switch(state)
    {
    case wait_start: if(is_button_rising_edge(BTN_START))
        {
            user_time_set_one_time (5000);
            action_set_page(31);
            action_start_page();
            state=wait_ready;
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
                state=walk_first;
            }
        }
        break;
    case walk_first:
        fall_state=balance_robot_has_fallen();
        if(fall_state!=robot_standing) state=get_up;
        else {
            if (walk_forward_compensating(comp_ini,exp_compass_get_avg_heading())) {
                if (exp_gpio_get_value(right_foot_forward)==1 && exp_gpio_get_value(left_foot_forward)==1) state = walk_to_stairs;
                else state = up_stairs1;
            }
            else {

                if (exp_gpio_get_value(left_foot_forward) == 1 || exp_gpio_get_value(right_foot_forward)==1){
                    mtn_lib_stop_mtn();
                }
            }
        }
        break;
    case walk_to_stairs:
        fall_state=balance_robot_has_fallen();
        if(fall_state!=robot_standing) state=get_up;
        else {
            if (walk_stairs(walk_stairs_val)) {
                if (walk_stairs_val==0) walk_stairs_val=1;
                else walk_stairs_val=0;
                if (diff > 150) walk_stairs_val = 0;
                else if (diff < -150) walk_stairs_val = 1;
                state = measure10;
                user_time_set_one_time (MEASURE_TIME);
            }
        }

        break;
    case measure10:
        if (!user_time_is_done()){
            if (exp_gpio_get_value(left_foot_forward_down)) measureFDL[1]++;
            else measureFDL[0]++;
            if (exp_gpio_get_value(right_foot_forward_down))measureFDR[1]++;
            else measureFDR[0]++;
            if (exp_gpio_get_value(left_foot_lateral_down)) measureLDL[1]++;
            else measureLDL[0]++;
            if (exp_gpio_get_value(right_foot_lateral_down)) measureLDR[1]++;
            else measureLDR[0]++;
            if (exp_gpio_get_value(left_foot_forward)) measureFFL[1]++;
            else measureFFL[0]++;
            if (exp_gpio_get_value(right_foot_forward)) measureFFR[1]++;
            else measureFFR[0]++;
        }
        else {
            set_measures();
            printf ("****%d,%d,%d,%d,%d,%d, dif =%d\n", measureFDL[2],measureFDR[2],measureLDL[2],measureLDR[2],measureFFR[2],measureFFL[2],diff);
            if ((measureFFL[2] == 0 || measureFFR[2]==0) && measureFDL[2]==0 && measureFDR[2]==0){
                if (measureFFR[2]==0 && measureFFL[2]==1) state = walk_to_stairs;
                else state = up_stairs1;
            }
            else {
                if ((measureFDL[2]==1 || measureFDR[2]==1) && (measureLDL[2]==0 && measureLDR[2]==0)) {
                    if (measureFDL[2]==1 &&measureFDR[2]==0) state = walk_back_stairs;
                    else if (diff>TURN_VALUE || diff<-TURN_VALUE){//REVIEW THIS!!!!!************************************
                        if (diff>0) state = turn_left_little;
                        else state = turn_right_little;
                    }
                    else state = down_stairs2;
                }
                else {
                    if (measureFDL[2]==1 && measureLDL[2] ==1 && measureLDR[2]==0) { //if left foot is out (swapped after testing) 0-1
                        if (measureFDR[2]==0) {
                            if (diff<TURN_VALUE && diff>-TURN_VALUE)state = move_right;
                            else state = turn_left_little;
                        
                        }
                        else {
                            walk_back_val=1;
                            walk_back(1);
                            state = walk_back_stairs;
                        }
                    }
                    else if (measureFDR[2]==1 && measureLDR[2]==1 && measureLDL[2]==0) { //if right foot is out (swapped)
                        if (measureFDL[2]==0){
                         if (diff<TURN_VALUE && diff>-TURN_VALUE)state = move_left;
                         else state = turn_right_little;
                         }
                        else {
                            walk_back(0);
                            walk_back_val=0;
                            state = walk_back_stairs;
                        }
                    }
                    else if (measureLDL[2]==1 && measureFDL[2]==0 && measureLDR[2]==0 && measureFDR[2]==0 && (diff<TURN_VALUE && diff>-TURN_VALUE)){ //if we're too on the left but looking forward
                        state = move_right;
                    }
                    else if (measureLDR[2]==1 && measureFDR[2]==0 && measureLDL[2]==0 && measureFDL[2]==0 && (diff<TURN_VALUE && diff>-TURN_VALUE)){ //if we're too on the right but looking forward
                        state = move_left;
                    }
                    else {
                        if (measureLDL[2]==1 && measureFDL[2]==1 && measureLDR[2]==1 && measureFDR[2]==1) state = walk_back_stairs;
                        else {
                            if (diff>TURN_VALUE || diff<-TURN_VALUE){
                                if (diff>0) state = turn_left_little;
                                else state = turn_right_little;

                            }
                            else state = walk_to_stairs;
                        }
                    }
                }
            }
            reset_measures();
        }

        break;
    case walk_back_stairs:
        if (walk_back(walk_back_val)){
            user_time_set_one_time (MEASURE_TIME);
            state = measure10;
        }
        else state = walk_back_stairs;
        break;
    case walk_top_stairs:
        if (walk_forward_compensating(comp_ini,exp_compass_get_avg_heading())) {
            state = measure10;
            user_time_set_one_time (MEASURE_TIME);
        }
        else {
            if (get_steps() >= STEPS_TOP) mtn_lib_stop_mtn();
            state =walk_top_stairs;
        }
        break;
    case get_up:
        staircount= 0;
        if(get_up_process(fall_state)==0x01) state = walk_to_stairs;
        else state = get_up;
        break;

    case up_stairs1:
        if(stairs_up_process(fall_state)){

            staircount++;
            //printf ("stairs = %d\n", staircount);
            if (staircount<3){
                //printf ("staircount<3\n");
                state = measure10;
                user_time_set_one_time (MEASURE_TIME);
            }
            else {
                state = walk_top_stairs;

                reset_steps();
            }
        }
        else {
            fall_state = balance_robot_has_fallen();
            if(fall_state!=robot_standing) state = get_up;
            else state = up_stairs1;
        }
        break;

    case turn_left_little:
        if (turn_left_SL()){//if we turn left it means diff was posiitive -> stop when its < 0

            //if (diff<10) {
            user_time_set_one_time (MEASURE_TIME);
            state = measure10;
            //}
        }
        else state = turn_left_little;
        break;
    case turn_right_little:
        if (turn_right_SR()){ //if we turn right it means diff was negative -> stop when its > 0

            //if (diff>-10) {
            user_time_set_one_time (MEASURE_TIME);
            state = measure10;
            //}
        }
        else state = turn_right_little;
        break;

    case down_stairs2 :
        if (stairs_down_process()) {
            staircount++;
            //printf ("stairs = %d\n", staircount);
            if (staircount<6){
                user_time_set_one_time (MEASURE_TIME);			                	
                state = measure10;
            }
            else {
                state = walk_first;
                reset_steps();
            }
        }
        else state = down_stairs2;
        break;

    case move_right:
        if (walk_right_SR()){
            user_time_set_one_time (MEASURE_TIME);
            state = measure10;
        }
        else state = move_right;
        break;
    case move_left:
        if (walk_left_SL()){
            user_time_set_one_time (MEASURE_TIME);
            state = measure10;

        }
        else state = move_left;
        break;
    }
}

