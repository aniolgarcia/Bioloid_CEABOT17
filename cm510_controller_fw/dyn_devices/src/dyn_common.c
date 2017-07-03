#include "dyn_common.h"
#include "dynamixel_master.h"

uint16_t get_model_number(uint8_t servo_id)
{
  uint16_t value;

  if(dyn_master_read_word(servo_id,MODEL_NUM,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFFFF;//error
}

uint8_t get_firmware_version(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,FIRMWARE_VER,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t set_servo_id(uint8_t old_id, uint8_t new_id)
{
  if(dyn_master_write_byte(old_id,ID,new_id)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}

uint8_t get_baudrate(uint8_t servo_id)
{
  uint8_t value;

  if(dyn_master_read_byte(servo_id,BAUD_RATE,&value)==DYN_SUCCESS)
    return value;// the actual value
  else
    return 0xFF;//error
}

uint8_t set_baudrate(uint8_t servo_id, uint8_t baudrate)
{
  if(dyn_master_write_byte(servo_id,BAUD_RATE,baudrate)==DYN_SUCCESS)
    return 0x00;// the actual value
  else
    return 0xFF;//error
}
