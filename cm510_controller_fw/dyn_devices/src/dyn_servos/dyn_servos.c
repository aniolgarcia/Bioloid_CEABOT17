/*
 *  Copyright (c) 2011 Humanoid Lab
 * 
 *  This file is part of iri-libbioloid. iri-libbioloid is free software: you
 *  can redistribute it and/or modify it under the terms of the GNU Lesser
 *  Public License as published by the Free Software Foundation, either 
 *  version 3 of the License, or (at your option) any later version.
 * 
 *  iri-libbioloid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser Public License
 *  along with iri-libbioloid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dyn_servos_reg.h"
#include "dyn_servos.h"
#include "dynamixel_master.h"

uint8_t get_angle_limits(uint8_t servo_id, uint16_t *cw, uint16_t *ccw)
{
  if(dyn_master_read_word(servo_id,CW_ANGLE_LIM,cw)==DYN_SUCCESS)
  {
    if(dyn_master_read_word(servo_id,CCW_ANGLE_LIM,ccw)==DYN_SUCCESS)
      return 0x00;// the actual value
    else
      return 0xFF;
  }
  else
    return 0xFF;//error
}

uint8_t set_angle_limits(uint8_t servo_id, uint16_t cw, uint16_t ccw)
{
  if(dyn_master_write_word(servo_id,CW_ANGLE_LIM,cw)==DYN_SUCCESS)
  {
    if(dyn_master_write_word(servo_id,CCW_ANGLE_LIM,ccw)==DYN_SUCCESS)
      return 0x00;// the actual value
    else
      return 0xFF;
  }
  else
    return 0xFF;//error
}

uint8_t get_temperature_limit(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,TEMP_LIM,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t set_temperature_limit(uint8_t servo_id, uint8_t temp)
{
  if(dyn_master_write_byte(servo_id,TEMP_LIM,temp)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t get_voltage_limits(uint8_t servo_id, uint8_t *low, uint8_t *high)
{
  if(dyn_master_read_byte(servo_id,LOW_VOLTAGE_LIM,low)==DYN_SUCCESS)
  {
    if(dyn_master_read_byte(servo_id,HIGH_VOLTAGE_LIM,high)==DYN_SUCCESS)
      return 0x00;// the actual value
    else
      return 0xFF;
  }
  else
    return 0xFF;//error
}

uint8_t set_voltage_limits(uint8_t servo_id, uint8_t low, uint8_t high)
{
  if(dyn_master_write_byte(servo_id,LOW_VOLTAGE_LIM,low)==DYN_SUCCESS)
  {
    if(dyn_master_write_byte(servo_id,HIGH_VOLTAGE_LIM,high)==DYN_SUCCESS)
      return 0x00;// the actual value
    else
      return 0xFF;
  }
  else
    return 0xFF;//error
}

uint16_t get_max_torque(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,MAX_TORQUE,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint8_t set_max_torque(uint8_t servo_id, uint16_t torque)
{
  if(dyn_master_write_word(servo_id,MAX_TORQUE,torque)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t get_led_alarm(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,LED_ALARM,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t set_led_alarm(uint8_t servo_id, uint8_t alarms)
{
  if(dyn_master_write_byte(servo_id,LED_ALARM,alarms)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t get_shd_alarm(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,SHD_ALARM,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t set_shd_alarm(uint8_t servo_id, uint8_t alarms)
{
  if(dyn_master_write_byte(servo_id,SHD_ALARM,alarms)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t set_wheel_mode(uint8_t servo_id)
{
  return set_angle_limits(servo_id,0,0);
}

uint8_t set_joint_mode(uint8_t servo_id)
{
  return set_angle_limits(servo_id,1024,0);
}

uint8_t is_joint_mode(uint8_t servo_id)
{
  uint16_t cw,ccw;

  if(get_angle_limits(servo_id,&cw,&ccw)==0xFF)
    return 0xFF;
  else
  {
    if(cw==0 && ccw==0)
      return 0x00;
    else
      return 0xFF;
  }
}

uint8_t enable_servo(uint8_t servo_id)
{
  if(dyn_master_write_byte(servo_id,TORQUE_EN,0x01)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t disable_servo(uint8_t servo_id)
{
  if(dyn_master_write_byte(servo_id,TORQUE_EN,0x00)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t is_servo_enabled(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,TORQUE_EN,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t turn_servo_led_on(uint8_t servo_id)
{
  if(dyn_master_write_byte(servo_id,SERVO_LED,0x01)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t turn_servo_led_off(uint8_t servo_id)
{
  if(dyn_master_write_byte(servo_id,SERVO_LED,0x00)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t is_servo_led_on(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,SERVO_LED,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t get_compliance_margin(uint8_t servo_id, uint8_t *cw, uint8_t *ccw)
{
  if(dyn_master_read_byte(servo_id,CW_COMP_MARGIN,cw)==DYN_SUCCESS)
  {
    if(dyn_master_read_byte(servo_id,CCW_COMP_MARGIN,ccw)==DYN_SUCCESS)
      return 0x00;// the actual value
    else
      return 0xFF;
  }
  else
    return 0xFF;//error
}

uint8_t set_compliance_margin(uint8_t servo_id, uint8_t cw, uint8_t ccw)
{
  if(dyn_master_write_byte(servo_id,CW_COMP_MARGIN,cw)==DYN_SUCCESS)
  {
    if(dyn_master_write_byte(servo_id,CCW_COMP_MARGIN,ccw)==DYN_SUCCESS)
      return 0x00;// the actual value
    else
      return 0xFF;
  }
  else
    return 0xFF;//error
}

uint8_t get_compliance_slope(uint8_t servo_id, uint8_t *cw, uint8_t *ccw)
{
  if(dyn_master_read_byte(servo_id,CW_COMP_SLOPE,cw)==DYN_SUCCESS)
  {
    if(dyn_master_read_byte(servo_id,CCW_COMP_SLOPE,ccw)==DYN_SUCCESS)
      return 0x00;// the actual value
    else
      return 0xFF;
  }
  else
    return 0xFF;//error
}

uint8_t set_compliance_slope(uint8_t servo_id, uint8_t cw, uint8_t ccw)
{
  if(dyn_master_write_byte(servo_id,CW_COMP_SLOPE,cw)==DYN_SUCCESS)
  {
    if(dyn_master_write_byte(servo_id,CCW_COMP_SLOPE,ccw)==DYN_SUCCESS)
      return 0x00;// the actual value
    else
      return 0xFF;
  }
  else
    return 0xFF;//error
}

uint8_t get_pid_params(uint8_t servo_id,uint8_t *P,uint8_t *I, uint8_t *D)
{
  return 0xFF;
}

uint8_t set_pid_params(uint8_t servo_id,uint8_t P,uint8_t I, uint8_t D)
{
  return 0xFF;
}

uint16_t get_target_position(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,GOAL_POS,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint8_t set_target_position(uint8_t servo_id, uint16_t position)
{
  if(dyn_master_write_word(servo_id,GOAL_POS,position)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint16_t get_target_speed(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,GOAL_SPEED,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint8_t set_target_speed(uint8_t servo_id, uint16_t speed)
{
  if(dyn_master_write_word(servo_id,GOAL_SPEED,speed)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint16_t get_torque_limit(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,TORQUE_LIM,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint8_t set_torque_limit(uint8_t servo_id, uint16_t torque)
{
  if(dyn_master_write_word(servo_id,TORQUE_LIM,torque)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint16_t get_current_position(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,CURRENT_POS,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint16_t get_current_speed(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,CURRENT_SPEED,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint16_t get_current_load(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,CURRENT_LOAD,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint8_t get_current_voltage(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,CURRENT_VOLTAGE,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t get_current_temperature(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,CURRENT_TEMP,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t is_moving(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,MOVING,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t is_locked(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,LOCK,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t lock(uint8_t servo_id)
{
  if(dyn_master_write_byte(servo_id,LOCK,0x01)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint16_t get_punch(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,PUNCH,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint8_t set_punch(uint8_t servo_id, uint16_t punch)
{
  if(dyn_master_write_word(servo_id,PUNCH,punch)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}
