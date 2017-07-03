#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "cm510_cfg.h"
#include "motion_cfg.h"
#include "motion_manager.h"
#include "motion_pages.h"
#include "dynamixel_master.h"
#include "dyn_servos.h"
#include "dyn_servos_reg.h"
#include "action.h"
#include "balance.h"
#include "buzzer.h"
#include "exp_board.h"
#include "pan_tilt.h"

// external functions
extern void buzzer_start_alarm(note_t note,uint16_t on_time_100ms,uint16_t off_time_100ms);
extern void buzzer_stop_alarm(void);
extern void balance_loop(void);
extern void balance_get_all_offsets(int16_t **offsets);
extern void action_init(void);
extern void action_process(void);
extern void exp_board_loop(void);
extern void exp_board_loop_start(void);
extern void exp_board_loop_read(void);

// private variables
uint8_t manager_num_servos;
TServoInfo manager_servos[MANAGER_MAX_NUM_SERVOS];
extern TActionPage action_next_page;
extern TActionPage action_current_page;

typedef struct
{
  uint8_t cw_slope;
  uint8_t ccw_slope;
  uint16_t position;
}dyn_send_data;

uint8_t servo_ids[MANAGER_MAX_NUM_SERVOS];
dyn_send_data data[MANAGER_MAX_NUM_SERVOS];
TWriteData packets[MANAGER_MAX_NUM_SERVOS];

// private functions
void manager_timer_init(void)
{
  // initialize timer4 for buzzer time control (7.8ms period)
  TIMSK4=0x00;// disable all interrupts
  TIFR4=0x2F;// clear any pending interrupt
  TCNT4H=0x00;                   
  TCNT4L=0x00;
  // set PWM mode with ICR top-count (CTC mode)
  TCCR4A=0x00;
  TCCR4A&=~(1<<WGM40);
  TCCR4A&=~(1<<WGM41);
  TCCR4B=0x00;
  TCCR4B|=(1<<WGM42);
  TCCR4B&=~(1<<WGM43);
  // clear output compare value A
  OCR4AH=0x3D;
  OCR4AL=0x08;
  TCCR4B&=0xF8;// set the prescaler to 8
  TCCR4B|=0x02;
}

uint8_t manager_period_done(void)
{
  if(TIFR4&(1<<OCF4A))
  {
    TIFR4|=(1<<OCF4A);// clear any interrupt
    TCNT4H=0x00;                   
    TCNT4L=0x00;
    return 0x01;
  }
  else
    return 0x00;
}

void manager_send_motion_command(void) 
{
  uint8_t *pdata;
  uint8_t i,num=0;
  uint16_t target_pos;
  int16_t *offsets;

  pdata = (uint8_t *)data;
  
  balance_get_all_offsets(&offsets);
  for(i=0;i<MANAGER_MAX_NUM_SERVOS;i++) 
  {
    if(manager_servos[i].module!=MM_NONE)
    {
      target_pos = manager_servos[i].current_value + offsets[i];
      pdata[num*4+2] = target_pos&0xFF;
      pdata[num*4+3] = (target_pos>>8)&0xFF;

      if(manager_servos[i].slope == 0) 
      {
        pdata[num*4] = 5;
        pdata[num*4+1] = 5;
      }
      else 
      {
        pdata[num*4] = 1<<(0x0F&manager_servos[i].slope);
        pdata[num*4+1] = 1<<(0x0F&manager_servos[i].slope);
      }

      packets[num].data_addr=(uint8_t *)&data[num];
      num++;
    }
  }
  if(num>0)
    dyn_master_sync_write(manager_num_servos,servo_ids,CW_COMP_SLOPE,4,packets); 
}

void manager_set_servo_slope(uint8_t servo_id, uint8_t slope)
{
  manager_servos[servo_id].slope=slope;
}

uint8_t manager_get_servo_slope(uint8_t servo_id)
{ 
  return manager_servos[servo_id].slope;
}

void manager_set_servo_value(uint8_t servo_id, uint16_t value)
{
  manager_servos[servo_id].current_value=value;
}

uint16_t manager_get_servo_value(uint8_t servo_id)
{
  return manager_servos[servo_id].current_value;
}

mm_module_t manager_get_servo_module(uint8_t servo_id)
{
  return manager_servos[servo_id].module;
}

uint8_t manager_get_cw_servo_limit(uint8_t servo_id)
{
  return manager_servos[servo_id].cw_limit;
}

uint8_t manager_get_ccw_servo_limit(uint8_t servo_id)
{
  return manager_servos[servo_id].ccw_limit;
}
void manager_loop(void)
{
  if(manager_period_done()==0x01)
  {
/*    #if EXP_BOARD_USE_LOOP==1
      if(exp_board_is_present())
        exp_board_loop(); 
    #endif*/
    #if EXP_BOARD_USE_LOOP==1
      if(exp_board_is_present())
        exp_board_loop_start(); 
    #endif
    // call the action process
    action_process(); //action_ready
    // balance the robot
    balance_loop();
    // update the pan&tilt angles
    pan_tilt_process();
    #if EXP_BOARD_USE_LOOP==1
      if(exp_board_is_present())
        exp_board_loop_read(); 
    #endif
    // send the motion commands to the servos
    manager_send_motion_command();
  }
}

// public functions
uint8_t manager_init(uint8_t num_servos)
{
  uint8_t i,num=0,id;
  uint16_t model;

  // enable power to the servos
  if(dyn_master_is_init()==0x00)
    dyn_master_init();
  _delay_ms(500);

  /* initialize by default the information for all servos */
  for(i=0;i<num;i++)
  {
    manager_servos[i].angle=0;
    manager_servos[i].max_value=0;
    manager_servos[i].min_value=0;
    manager_servos[i].center_value=0;
    manager_servos[i].current_value=0;
    manager_servos[i].module=MM_NONE;
    manager_servos[i].slope=5;
    manager_servos[i].cw_limit=0;
    manager_servos[i].ccw_limit=0;
  }

  /* scan the bus for all available servos */
  dyn_master_scan(&num,servo_ids);
  manager_num_servos=0;
  for(i=0;i<num;i++)
  {
    id=servo_ids[i];
    model=get_model_number(id);
    if(model==0x000C || model==0x012C)// standard AX-12 or AX12W
    {
      enable_servo(id);
      manager_servos[id].angle=300;
      manager_servos[id].max_value=1023;
      manager_servos[id].min_value=0;
      manager_servos[id].center_value=512;
      manager_servos[id].current_value=get_current_position(id);
      get_angle_limits(id,&manager_servos[id].cw_limit,&manager_servos[id].ccw_limit);
      manager_servos[id].module=MM_ACTION;
      manager_servos[id].slope=5;
      manager_num_servos++;
    }
    else if(model==0x0C00)// Emmulated servos from the expansion board pan&tilt servos
    {
      enable_servo(id);
      manager_servos[id].angle=180;
      manager_servos[id].max_value=1023;
      manager_servos[id].min_value=0;
      manager_servos[id].center_value=512;
      manager_servos[id].current_value=get_current_position(id);
      get_angle_limits(id,&manager_servos[id].cw_limit,&manager_servos[id].ccw_limit);
      manager_servos[id].module=MM_JOINTS;
      manager_servos[id].slope=5;
      manager_num_servos++;
    }
  } 

  if(num_servos != manager_num_servos)
    buzzer_start_alarm(MANAGER_MISSING_SERVOS_ALARM_NOTE,MANAGER_MISSING_SERVOS_ALARM_TIME_ON,MANAGER_MISSING_SERVOS_ALARM_TIME_OFF);
  else
    buzzer_stop_alarm();

  /* initialize the period timer */
  manager_timer_init();
  /* initialize the action module */
  action_init();

  return num;
}



