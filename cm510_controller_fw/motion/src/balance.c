#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "motion_cfg.h"
#include "balance.h"
#include "adc.h"
#include "buzzer.h"
#include "user_time.h"

// external functions
extern void buzzer_start_alarm(note_t note,uint16_t on_time_100ms,uint16_t off_time_100ms);
extern void buzzer_stop_alarm(void);

extern void adc_loop(void);
extern void user_time_loop(void);

// private variables
uint16_t balance_x_gyro_center;
uint16_t balance_y_gyro_center;
uint8_t balance_gyro_calibrated;
uint8_t balance_enabled;
uint8_t balance_robot_fallen_state;
int16_t balance_offsets[MANAGER_MAX_NUM_SERVOS];

// private functions
void balance_loop(void)
{
  int16_t gyro_x=0;
  int16_t gyro_y=0;

  if(balance_enabled == 0x01 && balance_gyro_calibrated == 0x01)
  {
    // get x,y gyroscope
    gyro_x=get_adc_channel(BALANCE_GYRO_X_CHANNEL)-balance_x_gyro_center;
    gyro_y=get_adc_channel(BALANCE_GYRO_Y_CHANNEL)-balance_y_gyro_center;

    if(gyro_x < BALANCE_BACKWARD_FALL_THD_VALUE)
      balance_robot_fallen_state = robot_face_up;
    else if(gyro_x > BALANCE_FORWARD_FALL_THD_VALUE)
      balance_robot_fallen_state = robot_face_down;

    float x_error1,x_error2,y_error1,y_error2;
    x_error1=gyro_x*BALANCE_KNEE_GAIN;//4.0/54.0;
    x_error2=gyro_x*BALANCE_ANKLE_PITCH_GAIN;//4.0/18.0;
    y_error1=gyro_y*BALANCE_ANKLE_ROLL_GAIN;//4.0/20.0;
    y_error2=gyro_y*BALANCE_HIP_ROLL_GAIN;//4.0/40.0;

    balance_offsets[8] = (uint16_t)y_error1;
    balance_offsets[9] = (uint16_t)y_error1;
    balance_offsets[12] = (uint16_t)x_error1;
    balance_offsets[13] = (uint16_t)-x_error1;
    balance_offsets[14] = (uint16_t)x_error2;
    balance_offsets[15] = (uint16_t)-x_error2;
    balance_offsets[16] = (uint16_t)-y_error2;
    balance_offsets[17] = (uint16_t)-y_error2;
  }
}

int16_t balance_get_offset(uint8_t index)
{
  return balance_offsets[index];
}

void balance_get_all_offsets(int16_t **offsets)
{
  *offsets=&balance_offsets[0];
}

// public functions
void balance_init(void)
{
  uint8_t i;

  balance_x_gyro_center=0;
  balance_y_gyro_center=0;
  balance_gyro_calibrated=0x00;
  balance_enabled=0x00;
  balance_robot_fallen_state=robot_standing;
  for(i=0;i<MANAGER_MAX_NUM_SERVOS;i++)
    balance_offsets[i]=0;
}

fallen_t balance_robot_has_fallen(void)
{
  uint8_t state = balance_robot_fallen_state;
  balance_robot_fallen_state = robot_standing;
  return state;
}

void balance_reset_fall_state(void)
{
  balance_robot_fallen_state = robot_standing;
}

uint8_t balance_calibrate_gyro(void)
{
  uint16_t x_gyro_values[BALANCE_GYRO_CAL_NUM_SAMPLES];
  uint16_t y_gyro_values[BALANCE_GYRO_CAL_NUM_SAMPLES];
  float x_gyro_average=0.0,y_gyro_average=0.0,std_x=0.0,std_y=0.0;
  uint8_t i=0;

  balance_x_gyro_center=0;
  balance_y_gyro_center=0;
  user_time_set_period(50);
  for(i=0;i<BALANCE_GYRO_CAL_NUM_SAMPLES;i++)
  {
    while(!user_time_is_period_done())
    {
      user_time_loop();
      adc_loop();
    }
    x_gyro_values[i]=get_adc_channel(BALANCE_GYRO_X_CHANNEL); 
    y_gyro_values[i]=get_adc_channel(BALANCE_GYRO_Y_CHANNEL); 
    x_gyro_average+=x_gyro_values[i];
    y_gyro_average+=y_gyro_values[i];
  }  
  user_time_stop();
  x_gyro_average/=(float)BALANCE_GYRO_CAL_NUM_SAMPLES;
  y_gyro_average/=(float)BALANCE_GYRO_CAL_NUM_SAMPLES;
  // compute the standard deviation
  for(i=0;i<BALANCE_GYRO_CAL_NUM_SAMPLES;i++)
  {
    std_x+=(x_gyro_values[i]-x_gyro_average)*(x_gyro_values[i]-x_gyro_average);
    std_y+=(y_gyro_values[i]-y_gyro_average)*(y_gyro_values[i]-y_gyro_average);
  }
  std_x=sqrt(std_x/BALANCE_GYRO_CAL_NUM_SAMPLES);
  std_y=sqrt(std_y/BALANCE_GYRO_CAL_NUM_SAMPLES);
  if(std_x<BALANCE_MAX_CAL_GYRO_ERROR && std_y<BALANCE_MAX_CAL_GYRO_ERROR)
  {
    balance_gyro_calibrated=0x01;
    balance_x_gyro_center=(uint16_t)x_gyro_average;
    balance_y_gyro_center=(uint16_t)y_gyro_average;
  }
  else
  {
    buzzer_start_alarm(BALANCE_GYRO_CAL_FAILED_ALARM_NOTE,BALANCE_GYRO_CAL_FAILED_ALARM_TIME_ON,BALANCE_GYRO_CAL_FAILED_ALARM_TIME_OFF);
    return 0x00;
  }
  buzzer_stop_alarm();
  return 0x01;
}

void balance_enable_gyro(void)
{
  balance_enabled=0x01;
}

void balance_disable_gyro(void)
{
  balance_enabled=0x00;
}

uint8_t balance_is_gyro_enabled(void)
{
  return balance_enabled;
}
