#include <avr/io.h>
#include "mtn_library.h"
#include "action_id.h"
#include "action.h"
#include <stdio.h>
#include <stdlib.h>
#include "cm510.h"
// private variables

#define err 100 // the error allowed for when we turn (angle) now set at 10º = 100
typedef enum {mtn_fwd=0,mtn_bwd=1,mtn_turn_left=2,mtn_turn_right=3,mtn_left=4,mtn_right=5,mtn_fwd_turn_left=6,mtn_fwd_turn_right=7,
              mtn_bwd_turn_left=8,mtn_bwd_turn_right=9,mtn_fwd_left=10,mtn_fwd_right=11,mtn_bwd_left=12,mtn_bwd_right=13,
              mtn_fast_fwd=14,mtn_fast_bwd=15} mtn_t;
typedef enum {idle,wait_start,wait_middle,wait_end} full_states;

typedef enum {wf_idle,wf_middle,wf_adjust,wf_end} wf_states;
typedef enum {wb_idle,wb_middle,wb_adjust,wb_end} wb_states;
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;

typedef enum {wrc_idle,wrc_middle,wrc_wait_stop,wrc_adjust,wrc_end} wrc_states;
typedef enum {wlc_idle,wlc_middle,wlc_wait_stop,wlc_adjust,wlc_end} wlc_states;
typedef enum {wait_stop_motion,wait_getting_up,wait_stabilize} get_up_states;
typedef enum {up_idle,up_wait_stop,up_wait_step1,up_delay1,up_wait_step2,up_wait_step3,up_wait_step4,wait_ready_up} stairs_up_states;
typedef enum {down_idle,down_wait_stop,down_wait_step1,down_wait_step2,down_wait_step3,wait_ready_down} stairs_down_states;

typedef struct{
  uint8_t start_l;
  uint8_t start_r;
  uint8_t middle_l;
  uint8_t middle_r;
  uint8_t end_l;
  uint8_t end_r;
}TPages;
int step_count;
uint8_t mtn_lib_stop;
foot_t mtn_lib_start_foot;
foot_t mtn_lib_current_foot;
TPages mtn_pages[]={{F_S_L,F_S_R,F_M_L,F_M_R,F_E_L,F_E_R}, //mtn_fwd
                    {B_S_L,B_S_R,B_M_L,B_M_R,B_E_L,B_E_R}, //mtn_bwd
                    {LT_S_L,LT_S_R,LT_M_L,LT_M_R,LT_E_L,LT_E_R},
                    {RT_S_L,RT_S_R,RT_M_L,RT_M_R,RT_E_L,RT_E_R},
                    {L_S_L,L_S_R,L_M_L,L_M_R,L_E_L,L_E_R},
//                     {R_S_L,R_S_R,R_M_L,R_M_R,R_E_L,R_E_R},
		    {R_S_R,R_S_L,R_M_R,R_M_L,R_E_R,R_E_L}, //Al caminar a la dreta, comença amb el peu esquerre, i ho hauria de fer amb el dret...
                    {FLT_S_L,FLT_S_R,FLT_M_L,FLT_M_R,FLT_E_L,FLT_E_R},
                    {FRT_S_L,FRT_S_R,FRT_M_L,FRT_M_R,FRT_E_L,FRT_E_R},
                    {BLT_S_L,BLT_S_R,BLT_M_L,BLT_M_R,BLT_E_L,BLT_E_R},
                    {BRT_S_L,BRT_S_R,BRT_M_L,BRT_M_R,BRT_E_L,BRT_E_R},
                    {FL_S_L,FL_S_R,FL_M_L,FL_M_R,FL_E_L,FL_E_R},
                    {FR_S_L,FR_S_R,FR_M_L,FR_M_R,FR_E_L,FR_E_R},
                    {BL_S_L,BL_S_R,BL_M_L,BL_M_R,BL_E_L,BL_E_R},
                    {BR_S_L,BR_S_R,BR_M_L,BR_M_R,BR_E_L,BR_E_R},
                    {fst_F_L_S,fst_F_R_S,fst_F_R_L_M,fst_F_L_R_M,fst_F_R_E,fst_F_L_E}, //fast forward
                    {fst_B_L_S,fst_B_R_S,fst_B_L_M,fst_B_R_M,fst_B_L_E,fst_B_R_E}}; //fast backward

/* private functions */
uint8_t mtn_lib_full(TPages *pages)
{
  static full_states state=idle;
  uint8_t done=0x00;

  switch(state)
  {
    case idle: if(mtn_lib_start_foot==left_foot)
                 action_set_page(pages->start_l);
               else
                 action_set_page(pages->start_r);
               action_start_page();
               mtn_lib_current_foot=mtn_lib_start_foot;
               state=wait_start;
               break;
    case wait_start: if(is_action_running())
                       state=wait_start;
                     else
                     {
                       if(mtn_lib_stop==0x01)
                       {
                         if(mtn_lib_current_foot==left_foot)
                         {
                           action_set_page(pages->end_r);
                           mtn_lib_current_foot=right_foot;
                         }
                         else
                         {
                           action_set_page(pages->end_l);
                           mtn_lib_current_foot=left_foot;
                         }
                         action_start_page();
                         state=wait_end;
                       }
                       else
                       {
                         if(mtn_lib_current_foot==left_foot)
                         {
                           action_set_page(pages->middle_r);
                           mtn_lib_current_foot=right_foot;
                         }
                         else
                         {
                           action_set_page(pages->middle_l);
                           mtn_lib_current_foot=left_foot;
                         }
                         action_start_page();
                         state=wait_middle;
                       }
                     }
                     break;
    case wait_middle: if(is_action_running())
                        state=wait_middle;
                      else
                      {
                        if(mtn_lib_stop==0x01)
                        {
                          if(mtn_lib_current_foot==left_foot)
                          {
                            action_set_page(pages->end_r);
                            mtn_lib_current_foot=right_foot;
                          }
                          else
                          {
                            action_set_page(pages->end_l);
                            mtn_lib_current_foot=left_foot;
                          }
                          action_start_page();
                          state=wait_end;
                        }
                        else
                        {
                          if(mtn_lib_current_foot==left_foot)
                          {
                            action_set_page(pages->middle_r);
                            mtn_lib_current_foot=right_foot;
                          }
                          else
                          {
                            action_set_page(pages->middle_l);
                            mtn_lib_current_foot=left_foot;
                          }
                          action_start_page();
                          state=wait_middle;
                        }
                      }
                      break;
    case wait_end: if(is_action_running())
                     state=wait_end;
                   else
                   {
                     mtn_lib_stop=0x00;
                     state=idle;
                     done=0x01;
                   }
                   break;
  }

  return done;
}

uint8_t mtn_lib_left(TPages *pages)
{
  static full_states state=idle;
  uint8_t done=0x00;

  switch(state)
  {
    case idle: if(mtn_lib_start_foot==left_foot)
                 action_set_page(pages->start_l);
               else
                 action_set_page(pages->start_r);
               action_start_page();
               mtn_lib_current_foot=mtn_lib_start_foot;
               state=wait_start;
               break;
    case wait_start: if(is_action_running())
                       state=wait_start;
                     else
                     {
                       if(mtn_lib_current_foot==left_foot)
                       {
                         if(mtn_lib_stop==0x01)
                         {
                           action_set_page(pages->end_r);
                           state=wait_end;
                         }
                         else
                         {
                           action_set_page(pages->middle_r);
                           state=wait_middle;
                         }
                         mtn_lib_current_foot=right_foot;
                         action_start_page();
                       }
                       else
                       {
                         action_set_page(pages->middle_l);
                         mtn_lib_current_foot=left_foot;
                         action_start_page();
                         state=wait_middle;
                       }
                     }
                     break;
    case wait_middle: if(is_action_running())
                        state=wait_middle;
                      else
                      {
                        if(mtn_lib_current_foot==left_foot)
                        {
                          if(mtn_lib_stop==0x01)
                          {
                            action_set_page(pages->end_r);
                            state=wait_end;
                          }
                          else
                          {
                            action_set_page(pages->middle_r);
                            state=wait_middle;
                          }
                          mtn_lib_current_foot=right_foot;
                          action_start_page();
                        }
                        else
                        {
                          action_set_page(pages->middle_l);
                          mtn_lib_current_foot=left_foot;
                          action_start_page();
                          state=wait_middle;
                        }
                      }
                      break;
    case wait_end: if(is_action_running())
                     state=wait_end;
                   else
                   {
                     mtn_lib_stop=0x00;
                     state=idle;
                     done=0x01;
                   }
                   break;
  }

  return done;
}

uint8_t mtn_lib_right(TPages *pages)
{
  static full_states state=idle;
  uint8_t done=0x00;

  switch(state)
  {
    case idle: if(mtn_lib_start_foot==left_foot)
                 action_set_page(pages->start_l);
               else
                 action_set_page(pages->start_r);
               action_start_page();
               mtn_lib_current_foot=mtn_lib_start_foot;
               state=wait_start;
               break;
    case wait_start: if(is_action_running())
                       state=wait_start;
                     else
                     {
                       if(mtn_lib_current_foot==right_foot)
                       {
                         if(mtn_lib_stop==0x01)
                         {
                           action_set_page(pages->end_l);
                           state=wait_end;
                         }
                         else
                         {
                           action_set_page(pages->middle_l);
                           state=wait_middle;
                         }
                         mtn_lib_current_foot=left_foot;
                         action_start_page();
                       }
                       else
                       {
                         action_set_page(pages->middle_r);
                         mtn_lib_current_foot=right_foot;
                         action_start_page();
                         state=wait_middle;
                       }
                     }
                     break;
    case wait_middle: if(is_action_running())
                        state=wait_middle;
                      else
                      {
                        if(mtn_lib_current_foot==right_foot)
                        {
                          if(mtn_lib_stop==0x01)
                          {
                            action_set_page(pages->end_l);
                            state=wait_end;
                          }
                          else
                          {
                            action_set_page(pages->middle_l);
                            state=wait_middle;
                          }
                          mtn_lib_current_foot=left_foot;
                          action_start_page();
                        }
                        else
                        {
                          action_set_page(pages->middle_r);
                          mtn_lib_current_foot=right_foot;
                          action_start_page();
                          state=wait_middle;
                        }
                      }
                      break;
    case wait_end: if(is_action_running())
                     state=wait_end;
                   else
                   {
                     mtn_lib_stop=0x00;
                     state=idle;
                     done=0x01;
                   }
                   break;
  }

  return done;
}
void change_foot(void){
	if (mtn_lib_current_foot==left_foot) mtn_lib_current_foot = right_foot;
	else mtn_lib_current_foot = left_foot;

}
int compass_deviation (int ini, int actual){ //Calculates the difference between initial orientation and actual orientation of compass.
	short int inc = actual - ini;
	if (inc<-1800) inc+=3600;
	else if (inc>1800) inc-=3600;
	return inc;
} //return value is between -1800 and 1800

int add_angles (int a, int b){
	int res = a+b;
	if (res < 0 ) res+=3600;
	else if (res>3600){
		res-=3600;
	}
	return res;

}


/* public functions */
void mtn_lib_init(void)
{
  mtn_lib_stop=0x00;
  mtn_lib_start_foot=left_foot;
  mtn_lib_current_foot=left_foot;
  step_count = 0;
}

void mtn_lib_stop_mtn(void)
{
  mtn_lib_stop=0x01;
}

void mtn_lib_set_start_foot(foot_t foot)
{
  mtn_lib_start_foot=foot;
}

void reset_steps(void)
{
  step_count = 0;
}

int get_steps (void){
	return step_count;
}

uint8_t walk_forward(void)
{
  return mtn_lib_full(&mtn_pages[mtn_fwd]);
}

uint8_t walk_backward(void)
{
  return mtn_lib_full(&mtn_pages[mtn_bwd]);
}

uint8_t turn_left(void)
{
  return mtn_lib_left(&mtn_pages[mtn_turn_left]);
}

uint8_t turn_right(void)
{
  return mtn_lib_right(&mtn_pages[mtn_turn_right]);
}

uint8_t walk_left(void)
{
  return mtn_lib_left(&mtn_pages[mtn_left]);
}

uint8_t walk_right(void)
{
  return mtn_lib_right(&mtn_pages[mtn_right]);
}

uint8_t walk_forward_turn_left(void)
{
  return mtn_lib_left(&mtn_pages[mtn_fwd_turn_left]);
}

uint8_t walk_forward_turn_right(void)
{
  return mtn_lib_right(&mtn_pages[mtn_fwd_turn_right]);
}

uint8_t walk_backward_turn_left(void)
{
  return mtn_lib_left(&mtn_pages[mtn_bwd_turn_left]);
}

uint8_t walk_backward_turn_right(void)
{
  return mtn_lib_right(&mtn_pages[mtn_bwd_turn_right]);
}

uint8_t walk_forward_left(void)
{
  return mtn_lib_left(&mtn_pages[mtn_fwd_left]);
}

uint8_t walk_forward_right(void)
{
  return mtn_lib_right(&mtn_pages[mtn_fwd_right]);
}

uint8_t walk_backward_left(void)
{
  return mtn_lib_left(&mtn_pages[mtn_bwd_left]);
}

uint8_t walk_backward_right(void)
{
  return mtn_lib_right(&mtn_pages[mtn_bwd_right]);
}


uint8_t fast_walk_forward(void)
{
  return mtn_lib_full(&mtn_pages[mtn_fast_fwd]);
}

uint8_t fast_walk_backward(void)
{
  return mtn_lib_full(&mtn_pages[mtn_fast_bwd]);
}

uint8_t fast_turn_left(void)
{
	//return mtn_lib_left (&mtn_pages[mtn_]);
}

uint8_t fast_turn_right(void)
{
}

uint8_t fast_walk_left(void)
{
}

uint8_t fast_walk_right(void)
{
}

uint8_t fast_walk_forward_turn_left(void)
{
}

uint8_t fast_walk_forward_turn_right(void)
{
}

uint8_t fast_walk_backward_turn_left(void)
{
}

uint8_t fast_walk_backward_turn_right(void)
{
}


uint8_t walk_forward_compensating(int comp_ini, int current){ //el primer cop que es crida ha d'estar en walk_ready
	static wf_states state = wf_idle;
	uint8_t done=0x00;
	int diff = compass_deviation(comp_ini,current);
	switch (state) {
		case wf_idle:
				if (mtn_lib_start_foot==left_foot) action_set_page (F_S_L);
				else action_set_page (F_S_R);
				action_start_page();
				mtn_lib_current_foot = (mtn_lib_start_foot + 1 )%2;
				state = wf_middle;
				break;
		case wf_middle:
				if (is_action_running()) state = wf_middle;
				else {
					step_count++;
					if (mtn_lib_stop==0x01){
						if (mtn_lib_current_foot==left_foot) action_set_page (F_E_L);
						else action_set_page (F_E_R);
						action_start_page();
						state = wf_end;
					}
					else {
						//si no se ha desviado o se ha desviado pero toca andar con el otro pie
						if (abs (diff)<100 || (diff<-100 && mtn_lib_current_foot==left_foot)|| (diff>100 && mtn_lib_current_foot==right_foot )){	
							if (mtn_lib_current_foot==left_foot) action_set_page (F_M_L);
							else action_set_page (F_M_R);
							action_start_page();
							state = wf_middle;
						}
						else { //se ha desviado
							if (diff<0){ //si tenemos que corregir hacia la derecha solo podemos empezar con el pie derecho
								if (mtn_lib_current_foot==right_foot) {//--> quizas este if no es necesario
								action_set_page (FRT_M_R);
								state = wf_adjust; 
								action_start_page();
								}
								else state = wf_middle;
							}
							else { //si tenemos que corregir hacia la izquierda solo podemos empezar con el pie izquierdo
								if (mtn_lib_current_foot==left_foot) {
								state = wf_adjust;
								action_set_page (FLT_M_L);
								action_start_page();
								}
								else state = wf_middle;
							}
						}
					
					}
					change_foot();
				}
				break;
		case wf_adjust:
				if (is_action_running()) state = wf_adjust;
				else {
					step_count++;
					if (mtn_lib_stop==0x01){
						if (diff > 0){ // si estamos girando a la izq solo podemos acabar con la derecha
							if (mtn_lib_current_foot==right_foot){
								action_set_page (FLT_E_R);
								state=wf_end;	
							}
							else {
							action_set_page (FLT_M_L);
							state = wf_adjust;
							}
						}
						else {// si estamos girando a la derecha solo podemos acabar con la izq
							if (mtn_lib_current_foot==left_foot) {
							action_set_page (FRT_E_L);
							state = wf_end;
							}
							else {
								action_set_page (FRT_M_R);
								state = wf_adjust;
							}
						}
						action_start_page();
						change_foot();
						
					}
					else {
						if (abs (diff) > 100) {
							if (diff > 0){ //Girar a la izq
								if (mtn_lib_current_foot==left_foot) action_set_page (FLT_M_L);//Forward Left Turn - wf_middle -Left foot
								else action_set_page (FLT_M_R);//Forward Left Turn - wf_middle -Right foot
							}
							else { //Girar a la derecha
								if (mtn_lib_current_foot==left_foot) action_set_page (FRT_M_L);//Forward Right Turn - wf_middle -Left foot
								else action_set_page (FRT_M_R);//Forward Right Turn - wf_middle -Right foot					
							}
							action_start_page();
							state = wf_adjust;
							change_foot();
						
						}
						else { //podemos volver a andar normal
							state = wf_middle;
						}
					}
				}
				break;
		case wf_end:
				if (is_action_running()) state = wf_end;
				else {
					step_count++;
					mtn_lib_stop =0x00;
					state = wf_idle;
					done = 0x01;
				}
				break;
	}
	return done;


}

uint8_t walk_backward_compensating(int comp_ini, int current){ //el primer cop que es crida ha d'estar en walk_ready
	static wb_states state = wb_idle;
	uint8_t done=0x00;
	int diff = compass_deviation(comp_ini,current);
	switch (state) {
		case wb_idle:
				if (mtn_lib_start_foot==left_foot) action_set_page (B_S_L);
				else action_set_page (B_S_R);
				action_start_page();
				mtn_lib_current_foot = (mtn_lib_start_foot + 1 )%2;
				state = wb_middle;
				break;
		case wb_middle:
				if (is_action_running()) state = wb_middle;
				else {
					step_count--;
					if (mtn_lib_stop==0x01){
						if (mtn_lib_current_foot==left_foot) action_set_page (B_E_L);
						else action_set_page (B_E_R);
						action_start_page();
						state = wb_end;
					}
					else {
						//si no se ha desviado o se ha desviado pero toca andar con el otro pie
						if (abs (diff)<100 || (diff>100 && mtn_lib_current_foot==left_foot)|| (diff<-100 && mtn_lib_current_foot==right_foot )){	
							if (mtn_lib_current_foot==left_foot) action_set_page (B_M_L);
							else action_set_page (B_M_R);
							action_start_page();
							state = wb_middle;
						}
						else { //se ha desviado
							if (diff>0){ //si tenemos que corregir hacia la derecha solo podemos empezar con el pie derecho
								if (mtn_lib_current_foot==right_foot) {//--> quizas este if no es necesario
								action_set_page (BRT_M_R);
								state = wb_adjust; 
								action_start_page();
								}
								else state = wb_middle;
							}
							else { //si tenemos que corregir hacia la izquierda solo podemos empezar con el pie izquierdo
								if (mtn_lib_current_foot==left_foot) {
								state = wb_adjust;
								action_set_page (BLT_M_L);
								action_start_page();
								}
								else state = wb_middle;
							}
						}
					
					}
					change_foot();
				}
				break;
		case wb_adjust:
				if (is_action_running()) state = wb_adjust;
				else {
					step_count--;
					if (mtn_lib_stop==0x01){
						if (diff < 0){ // si estamos girando a la izq solo podemos acabar con la derecha
							if (mtn_lib_current_foot==right_foot){
								action_set_page (BLT_E_R);
								state=wb_end;	
							}
							else {
							action_set_page (BLT_M_L);
							state = wb_adjust;
							}
						}
						else {// si estamos girando a la derecha solo podemos acabar con la izq
							if (mtn_lib_current_foot==left_foot) {
							action_set_page (BRT_E_L);
							state = wb_end;
							}
							else {
								action_set_page (BRT_M_R);
								state = wb_adjust;
							}
						}
						action_start_page();
						change_foot();
						
					}
					else {
						if (abs (diff) > 100) {
							if (diff < 0){ //Girar a la izq
								if (mtn_lib_current_foot==left_foot) action_set_page (BLT_M_L);//Forward Left Turn - wb_middle -Left foot
								else action_set_page (BLT_M_R);//Forward Left Turn - wb_middle -Right foot
							}
							else { //Girar a la derecha
								if (mtn_lib_current_foot==left_foot) action_set_page (BRT_M_L);//Forward Right Turn - wb_middle -Left foot
								else action_set_page (BRT_M_R);//Forward Right Turn - wb_middle -Right foot					
							}
							action_start_page();
							state = wb_adjust;
							change_foot();
						
						}
						else { //podemos volver a andar normal
							state = wb_middle;
						}
					}
				}
				break;
		case wb_end:
				if (is_action_running()) state = wb_end;
				else {
					step_count--;
					mtn_lib_stop =0x00;
					state = wb_idle;
					done = 0x01;
				}
				break;
	}
	return done;


}


uint8_t get_up_process(fallen_t fall_state)
{
  static get_up_states state;
  uint8_t done=0x00;

  switch(state)
  {
    case wait_stop_motion: 
    	if(!is_action_running())	{
      	if(fall_state==robot_face_down)
        	action_set_page(F_getup);
        else
          action_set_page(B_getup);
        action_start_page();
        state=wait_getting_up;
      }
			else//if the robot was moving we wait until it stops
      	state= wait_stop_motion;
      break;
    case wait_getting_up: if(!is_action_running()) //if it stopped getting up
                          {
                            state=wait_stabilize;
                            user_time_set_one_time(1000); //we set a timer for the robot to stabilize
                          }
                          else
                            state=wait_getting_up;
                          break;
    case wait_stabilize: if(user_time_is_done())
                         {
                           balance_reset_fall_state();
                           state=wait_stop_motion;
                           done=0x01;
                         }
                         else
                           state=wait_stabilize;
                         break;
  }

  return done;
}

uint8_t stairs_up_process(fallen_t fall_state)
{
  static stairs_up_states state=up_idle;
  uint8_t done=0x00;
  switch(state)
  {
    case up_idle: if(is_action_running())
                  {
                    action_stop_page();
                    state = up_wait_stop;
                  }
                  else
                  {
                    action_set_page(227);
                    action_start_page();
                    state = up_wait_step1;
                  }
                  break;
    case up_wait_stop: if(is_action_running())
                         state = up_wait_stop;
                       else
                       { 
                         action_set_page(227);
                         action_start_page();
                         state = up_wait_step1;       
                       }
                       break;
    case up_wait_step1: if(is_action_running())
                          state = up_wait_step1;
                        else
                        {
                          user_time_set_one_time(1500);
                          state = up_delay1;
                        }
                        break;
    case up_delay1: if(user_time_is_done())
                    {
                      action_set_page(224);
                      action_start_page();
                      state = up_wait_step2;
                    }
                    else
                      state = up_delay1;
                    break;
    case up_wait_step2: if(is_action_running())
                          state = up_wait_step2;
                        else
                        {
                          action_set_page(225);
                          action_start_page();
                          state = up_wait_step3;
                        }
                        break;
    case up_wait_step3: if(is_action_running())
                          state = up_wait_step3;
                        else
                        {
                          action_set_page(226);
                          action_start_page();
                          state = up_wait_step4;
                        }
                        break;
    case up_wait_step4: if(is_action_running())
                          state = up_wait_step4;
                        else
                        {
                          action_set_page(31);
                          action_start_page();
                          state = wait_ready_up;
                        }
                        break;
    case wait_ready_up: if(is_action_running())
                          state = wait_ready_up;
                        else
                        {
                          state=up_idle;
                          done=0x01;
                        }
                        break;
  }

  if (fall_state!=robot_standing) state=up_idle;
  return done;
}

uint8_t stairs_down_process(void)
{
  static stairs_down_states state=down_idle;
  uint8_t done=0x00;

  switch(state)
  {
    case down_idle: if(is_action_running())
                    {
                      action_stop_page();
                      state = down_wait_stop;
                    }
                    else
                    {
                      action_set_page(228);
                      action_start_page();
                      state = down_wait_step1;
                    }
                    break;
    case down_wait_stop: if(is_action_running())
                           state = down_wait_stop;
                         else
                         { 
                           action_set_page(228);
                           action_start_page();
                           state = down_wait_step1;       
                         }
                         break;
    case down_wait_step1: if(is_action_running())
                            state = down_wait_step1;
                          else
                          {
                            action_set_page(229);
                            action_start_page();
                            state = down_wait_step2;
                          }
                          break;
    case down_wait_step2: if(is_action_running())
                            state = down_wait_step2;
                          else
                          {
                            action_set_page(230);
                            action_start_page();
                            state = down_wait_step3;
                          }
                          break;
    case down_wait_step3: if(is_action_running())
                            state = down_wait_step3;
                          else
                          {
                            action_set_page(31);
                            action_start_page();
                            state = wait_ready_down;
                          }
                          break;
    case wait_ready_down: if(is_action_running())
                            state = wait_ready_down;
                          else
                          {
                            state=down_idle;
                            done=0x01;
                          }
                          break;
  }

  return done;
}


uint8_t walk_left_compensating (int comp_ini, int comp_act){
	static wlc_states s = wlc_idle;
	int done =0;
	int diff = compass_deviation (comp_ini,comp_act);
	switch (s){

		case wlc_idle:
			if (mtn_lib_start_foot==left_foot) action_set_page (L_S_L);
			else action_set_page (L_S_R);
			action_start_page();
			mtn_lib_current_foot = (mtn_lib_start_foot + 1 )%2;
			s = wlc_middle;
			break;
		case wlc_middle:				
			if (is_action_running()) s = wlc_middle;
				else {
					if (mtn_lib_stop==0x01){
						if (mtn_lib_current_foot==right_foot) {
							action_set_page (L_E_R);
							s = wlc_end;
						}
						else action_set_page (L_M_L); //we cant end with left foot
						action_start_page();
					}
					else { //si no paramos

						if (abs (diff)<200){//si no se ha desviado
							if (mtn_lib_current_foot==left_foot) action_set_page (L_M_L);
							else action_set_page (L_M_R);
							action_start_page();
							s = wlc_middle;
						}
						else { //se ha desviado
							//paramos el movimiento y pasamos a ajustar -> solo podemos acabar con REL
							if (mtn_lib_current_foot==right_foot) {
							
								action_set_page (L_E_R);
								action_start_page();
								s= wlc_wait_stop;
							}
							else {
								action_set_page (L_M_L);
								action_start_page();
								s= wlc_middle;
							}

						}
					
					}
					change_foot();
				}
			break;
		case wlc_wait_stop:
			if (is_action_running()) s = wlc_wait_stop;
			else s = wlc_adjust;
		case wlc_adjust:
							
				if (diff<0) //tenemos que girar a la derecha
				{
					if (turn_right()) {
						s = wlc_idle;
						mtn_lib_stop = 0;
					}
					else if (diff>-100) mtn_lib_stop_mtn();					
				}
				else {
					if (turn_left()) {
						s = wlc_idle;
						mtn_lib_stop = 0;
					}
					else if (diff<100) mtn_lib_stop_mtn();	
				}
			
			break;
		case wlc_end:
				if (is_action_running()) s = wlc_end;
				else {
					mtn_lib_stop = 0;
					done = 0x01;
					s = wlc_idle;
				}
			break;
	}
	return done;
}
uint8_t walk_right_compensating (int comp_ini, int comp_act){
	static wrc_states s = wrc_idle;
	int done =0;
	int diff = compass_deviation (comp_ini,comp_act);
	switch (s){

		case wrc_idle:
			if (mtn_lib_start_foot==left_foot) action_set_page (R_S_L);
			else action_set_page (R_S_R);
			action_start_page();
			mtn_lib_current_foot = (mtn_lib_start_foot + 1 )%2;
			s = wrc_middle;
			break;
		case wrc_middle:				
			if (is_action_running()) s = wrc_middle;
				else {
					if (mtn_lib_stop==0x01){
						if (mtn_lib_current_foot==left_foot) {
							action_set_page (R_E_L);
							s = wrc_end;
						}
						else action_set_page (R_M_R); //we cant end with Right foot
						action_start_page();
					}
					else { //si no paramos

						if (abs (diff)<200){//si no se ha desviado
							if (mtn_lib_current_foot==left_foot) action_set_page (R_M_L);
							else action_set_page (R_M_R);
							action_start_page();
							s = wrc_middle;
						}
						else { //se ha desviado
							//paramos el movimiento y pasamos a ajustar -> solo podemos acabar con REL
							if (mtn_lib_current_foot==left_foot) {
							
								action_set_page (R_E_L);
								action_start_page();
								s= wrc_wait_stop;
							}
							else {
								action_set_page (R_M_R);
								action_start_page();
								s= wrc_middle;
							}

						}
					
					}
					change_foot();
				}
			break;
		case wrc_wait_stop:
			if (is_action_running()) s = wrc_wait_stop;
			else s = wrc_adjust;
		case wrc_adjust:
							
				if (diff<0) //tenemos que girar a la derecha
				{
					if (turn_right()) {
						s = wrc_idle;
						mtn_lib_stop = 0;
					}
					else if (diff>-100) mtn_lib_stop_mtn();					
				}
				else {
					if (turn_left()) {
						s = wrc_idle;
						mtn_lib_stop = 0;
					}
					else if (diff<100) mtn_lib_stop_mtn();	
				}
			
			break;
		case wrc_end:
				if (is_action_running()) s = wrc_end;
				else {
					mtn_lib_stop = 0;
					done = 0x01;
					s = wrc_idle;
				}
			break;
	}
	return done;
}
uint8_t turn_angle (int angle){
	static turn_states s = t_init;
	static int comp_ini = 0;
	static int comp_end = 0;
	int done = 0;
	
	switch (s){
		case t_init:
			comp_ini = exp_compass_get_avg_heading();
			comp_end = add_angles (comp_ini,angle*10);
			
			s=t_middle;
			break;
		case t_middle:
			if (abs (compass_deviation (exp_compass_get_avg_heading(),comp_end))>err){
			// ("diff = %d\n",compass_deviation (exp_compass_get_avg_heading(),comp_end));
				if (compass_deviation (exp_compass_get_avg_heading(),comp_end)>err){
					s =t_right;
				}
				else if (compass_deviation (exp_compass_get_avg_heading(),comp_end)<-err){
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
				if (compass_deviation (exp_compass_get_avg_heading(),comp_end)<err){ 
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
				if (compass_deviation (exp_compass_get_avg_heading(),comp_end)>-err){
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

