#include "pan_tilt.h"
#include "motion_cfg.h"
#include "motion_manager.h"

// external variables
extern uint8_t manager_num_servos;
extern TServoInfo manager_servos[MANAGER_MAX_NUM_SERVOS];

// private variables
// pan variables
uint32_t pan_target_angle;
uint32_t pan_target_speed;
uint32_t pan_current_angle;
uint8_t pan_stoping;
uint8_t pan_moving;
uint8_t pan_servo_id;
// tilt variables
uint32_t tilt_target_angle;
uint32_t tilt_target_speed;
uint32_t tilt_current_angle;
uint8_t tilt_stoping;
uint8_t tilt_moving;
uint8_t tilt_servo_id;

// public functions
void pan_tilt_init(uint8_t pan_id,uint8_t tilt_id)
{
  // set serov ID's
  pan_servo_id=pan_id;
  tilt_servo_id=tilt_id;
  /* initialize pan variables */
  pan_target_angle=manager_servos[pan_servo_id].current_value<<7;
  pan_target_speed=0;
  pan_current_angle=manager_servos[pan_servo_id].current_value<<7;
  pan_stoping=0x00;
  pan_moving=0x00;
  /* initialize tilt variables */
  tilt_target_angle=manager_servos[tilt_servo_id].current_value<<7;
  tilt_target_speed=0;
  tilt_current_angle=manager_servos[tilt_servo_id].current_value<<7;
  tilt_stoping=0x00;
  tilt_moving=0x00;
}

void pan_tilt_move_angles(int8_t pan_angle,uint8_t tilt_angle)
{
  pan_move_angle(pan_angle);
  tilt_move_angle(tilt_angle);
}

void pan_move_angle(int8_t pan_angle)
{
  if(pan_servo_id!=0xFF)
  {
    if(pan_target_speed==0)
    {
      pan_target_angle=manager_servos[pan_servo_id].current_value<<7;
      pan_current_angle=manager_servos[pan_servo_id].current_value<<7;
      pan_moving=0x00;
    }
    else
    {
      pan_target_angle=(uint32_t)((((int32_t)pan_angle*(int32_t)manager_servos[pan_servo_id].max_value)/((int32_t)manager_servos[pan_servo_id].angle)+(int32_t)manager_servos[pan_servo_id].center_value));
      if(pan_target_angle>manager_servos[pan_servo_id].ccw_limit)
        pan_target_angle=manager_servos[pan_servo_id].ccw_limit;
      else if(pan_target_angle<manager_servos[pan_servo_id].cw_limit)
        pan_target_angle=manager_servos[pan_servo_id].cw_limit;
      if(manager_servos[pan_servo_id].current_value==pan_target_angle)
        pan_moving=0x00;
      else  
        pan_moving=0x01;
      pan_target_angle=(pan_target_angle<<7);
    }
  }
}

void pan_set_speed(uint8_t pan_speed)
{
  pan_target_speed=(((uint32_t)pan_speed*(uint32_t)manager_servos[pan_servo_id].max_value)/((uint32_t)manager_servos[pan_servo_id].angle));
}

void pan_stop(void)
{
  pan_stoping=0x01;
}

uint8_t pan_is_moving(void)
{
  return pan_moving;
}

void tilt_move_angle(int8_t tilt_angle)
{
  if(tilt_servo_id!=0xFF)
  {
    if(tilt_target_speed==0)
    {
      tilt_target_angle=manager_servos[tilt_servo_id].current_value<<7;
      tilt_current_angle=manager_servos[tilt_servo_id].current_value<<7;
      tilt_moving=0x00;
    }
    else
    {
      tilt_target_angle=(int32_t)((((int32_t)tilt_angle*(int32_t)manager_servos[tilt_servo_id].max_value)/((int32_t)manager_servos[tilt_servo_id].angle)+(int32_t)manager_servos[tilt_servo_id].center_value));
      if(tilt_target_angle>manager_servos[tilt_servo_id].ccw_limit)
        tilt_target_angle=manager_servos[tilt_servo_id].ccw_limit;
      else if(tilt_target_angle<manager_servos[tilt_servo_id].cw_limit)
        tilt_target_angle=manager_servos[tilt_servo_id].cw_limit;
      if(manager_servos[tilt_servo_id].current_value==tilt_target_angle)
        tilt_moving=0x00;
      else
        tilt_moving=0x01;
      tilt_target_angle=(tilt_target_angle<<7);
    }
  }
}

void tilt_set_speed(uint8_t tilt_speed)
{
  tilt_target_speed=(((uint32_t)tilt_speed*(uint32_t)manager_servos[tilt_servo_id].max_value)/((uint32_t)manager_servos[tilt_servo_id].angle));
}

void tilt_stop(void)
{
  tilt_stoping=0x01;
}

uint8_t tilt_is_moving(void)
{
  return tilt_moving;
}

void pan_tilt_process(void)
{
  // pan control
  if(pan_stoping==0x01)
  {
    pan_target_angle=manager_servos[pan_servo_id].current_value<<7;
    pan_current_angle=manager_servos[pan_servo_id].current_value<<7;
    pan_target_speed=0;
    pan_moving=0x00;
    pan_stoping=0x00;
  }
  else
  {
    if(pan_current_angle<pan_target_angle)
    {
      pan_current_angle+=pan_target_speed;
      if(pan_current_angle>=pan_target_angle)
      {
        pan_current_angle=pan_target_angle;
        pan_moving=0x00;
      }
    }
    else if(pan_current_angle>pan_target_angle)
    {
      if(pan_current_angle<pan_target_speed)
      {
        pan_current_angle=pan_target_angle;
        pan_moving=0x00;
      }
      else
      {
        pan_current_angle-=pan_target_speed;
        if(pan_current_angle<=pan_target_angle)
        {
          pan_current_angle=pan_target_angle;
          pan_moving=0x00;
        }
      }
    }
  }
  manager_servos[pan_servo_id].current_value=(pan_current_angle>>7);
  // tilt control
  if(tilt_stoping==0x01)
  {
    tilt_target_angle=manager_servos[tilt_servo_id].current_value;
    tilt_current_angle=manager_servos[tilt_servo_id].current_value;
    tilt_target_speed=0;
    tilt_moving=0x00;
    tilt_stoping=0x00;
  }
  else
  {
    if(tilt_current_angle<tilt_target_angle)
    {
      tilt_current_angle+=tilt_target_speed;
      if(tilt_current_angle>=tilt_target_angle)
      {
        tilt_current_angle=tilt_target_angle;
        tilt_moving=0x00;
      }
    }
    else if(tilt_current_angle>tilt_target_angle)
    {
      if(tilt_current_angle<tilt_target_speed)
      { 
        tilt_current_angle=tilt_target_angle;
        tilt_moving=0x00;
      }  
      else
      {
        tilt_current_angle-=tilt_target_speed;
        if(tilt_current_angle<=tilt_target_angle)
        {
          tilt_current_angle=tilt_target_angle;
          tilt_moving=0x00;
        }  
      }
    } 
  }
  manager_servos[tilt_servo_id].current_value=(tilt_current_angle>>7);
}
