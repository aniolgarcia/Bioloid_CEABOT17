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

#include "cm510_cfg.h"
#include "exp_board_reg.h"
#include "exp_board.h"
#include "dynamixel_master.h"
#include <util/delay.h>

#ifndef EXP_BOARD_USE_LOOP
  #define EXP_BOARD_USE_LOOP     0
#endif

#ifndef EXP_BOARD_ID
  #define EXP_BOARD_ID           30
#endif

uint8_t exp_board_id;
uint8_t exp_board_present;
uint8_t exp_board_dac_present;
uint8_t exp_board_uart_ttl_present;
uint8_t exp_board_pwm_present;
uint8_t exp_board_uart_usb_present;
uint8_t exp_board_num_servos;

uint8_t exp_board_int_data[BLOCK_LENGTH];

/* private functions */
#if EXP_BOARD_USE_LOOP==1
void exp_board_loop_start(void)
{
  dyn_master_start_read_table(exp_board_id,BLOCK_START_ADDRESS,BLOCK_LENGTH,exp_board_int_data);
}

void exp_board_loop_read(void)
{
  dyn_master_wait_read_table(exp_board_id,BLOCK_START_ADDRESS,BLOCK_LENGTH,exp_board_int_data);
}
void exp_board_loop(void)

{
  dyn_master_read_table(exp_board_id,BLOCK_START_ADDRESS,BLOCK_LENGTH,exp_board_int_data);
}
#endif

// public functions
// expansion board initialize
uint8_t exp_board_init(uint8_t board_id)
{
  uint8_t peripherals;

  /* ping the board to check whether its is connected or not */
  if(dyn_master_ping(board_id)==DYN_SUCCESS)
  {
    exp_board_id=board_id;
    exp_board_present=0x01;
    /* read the available peripherals */ 
    if(dyn_master_read_byte(exp_board_id,Hardware_available,&peripherals)==DYN_SUCCESS)
    {
      if(peripherals&0x01)
      {
        exp_board_pwm_present=0x01;
        exp_board_uart_ttl_present=0x00;
      }
      else
      {
        exp_board_pwm_present=0x00;
        exp_board_uart_ttl_present=0x01;
      }
      if(peripherals&0x02)
      {
        exp_board_dac_present=0x01;
        exp_board_uart_usb_present=0x00;
      }
      else
      {
        exp_board_dac_present=0x00;
        exp_board_uart_usb_present=0x01;
      }
      // read the number pf PWM servos
      dyn_master_read_byte(exp_board_id,NUM_PWM_SERVOS,&exp_board_num_servos);
      return 0x00;
    } 
    else
    {
      exp_board_dac_present=0x00;
      exp_board_uart_ttl_present=0x00;
      exp_board_pwm_present=0x00;
      exp_board_uart_usb_present=0x00;
      // read the number pf PWM servos
      dyn_master_read_byte(exp_board_id,NUM_PWM_SERVOS,&exp_board_num_servos);
      return 0xFF;
    }
  }
  else
  {
    exp_board_present=0x00;
    exp_board_dac_present=0x00;
    exp_board_uart_ttl_present=0x00;
    exp_board_pwm_present=0x00;
    exp_board_uart_usb_present=0x00;
    exp_board_id=0xFF;
    exp_board_num_servos=0;
    return 0xFF;
  }
}

uint8_t exp_board_is_present(void)
{  
  return exp_board_present;
}

/* ADC interface */
uint8_t exp_adc_start(void)
{
  if(dyn_master_write_byte(exp_board_id,ADC_CONTROL,0x01)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_adc_stop(void)
{
  if(dyn_master_write_byte(exp_board_id,ADC_CONTROL,0x02)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_adc_is_running(void)
{
  uint8_t status;

  if(dyn_master_read_byte(exp_board_id,ADC_CONTROL,&status)==DYN_SUCCESS)
  {
    if(status&0x04)
      return 0x01;
    else
      return 0x00;
  }
  else
    return 0xFF;
}

uint8_t exp_adc_set_average_samples(uint8_t num)
{
  if(dyn_master_write_byte(exp_board_id,ADC_NUM_SAMPLES,num)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_adc_get_average_samples(void)
{
  uint8_t num;

  if(dyn_master_read_byte(exp_board_id,ADC_NUM_SAMPLES,&num)==DYN_SUCCESS)
    return num;
  else
    return 0xFF;
}

uint8_t exp_adc_set_sample_period(uint8_t period_ms)
{
  if(dyn_master_write_byte(exp_board_id,ADC_SAMPLE_PERIOD,period_ms)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_adc_get_sample_period(void)
{
  uint8_t period;

  if(dyn_master_read_byte(exp_board_id,ADC_SAMPLE_PERIOD,&period)==DYN_SUCCESS)
    return period;
  else
    return 0xFF;
}

uint16_t exp_adc_get_channel(adc_id_t channel_id)
{
  #if EXP_BOARD_USE_LOOP==1
    return exp_board_int_data[ADC_CHANNEL0_L_block+channel_id*2-BLOCK_START_ADDRESS]+exp_board_int_data[ADC_CHANNEL0_H_block+channel_id*2-BLOCK_START_ADDRESS]*256;
  #else
  uint16_t voltage;

  if(dyn_master_read_word(exp_board_id,ADC_CHANNEL0_L+channel_id*2,&voltage)==DYN_SUCCESS)
    return ((uint32_t)voltage*5000)/1024;
  else
    return 0xFF;
  #endif
}

uint16_t exp_adc_get_avg_channel(adc_id_t channel_id)
{
  #if EXP_BOARD_USE_LOOP==1
    return exp_board_int_data[ADC_AVG_CHANNEL0_L_block+channel_id*2-BLOCK_START_ADDRESS]+exp_board_int_data[ADC_AVG_CHANNEL0_H_block+channel_id*2-BLOCK_START_ADDRESS]*256;
  #else
  uint16_t voltage;
 
  if(dyn_master_read_word(exp_board_id,ADC_AVG_CHANNEL0_L+channel_id*2,&voltage)==DYN_SUCCESS)
    return ((uint32_t)voltage*5000)/1024;
  else
    return 0xFF;
  #endif
}

/* GPIO interface */
uint8_t exp_gpio_config(gpio_id_t gpio_id, uint8_t dir)
{
  if(gpio_id<GPIO0 || gpio_id>GPIO18)
    return 0xFF;//no gpio_id valid
  if(dyn_master_write_byte(exp_board_id,(GPIO0_config+gpio_id*2),dir)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_gpio_get_value(gpio_id_t gpio_id)
{
  #if EXP_BOARD_USE_LOOP==1
    return exp_board_int_data[GPIO0_data_block+gpio_id-BLOCK_START_ADDRESS];
  #else
  uint8_t level;

  if(gpio_id<GPIO0 || gpio_id>GPIO18)
    return 0xFF;//no gpio_id valid
  if(dyn_master_read_byte(exp_board_id,(GPIO0_data+gpio_id*2),&level)==DYN_SUCCESS)
    return level;
  else
    return 0xFF;
  #endif
}

uint8_t exp_gpio_set_value(gpio_id_t gpio_id, uint8_t level)
{
  if(gpio_id<GPIO0 || gpio_id>GPIO18)
    return 0xFF;//no gpio_id valid
  if(dyn_master_write_byte(exp_board_id,(GPIO0_data+gpio_id*2),level)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_gpio_set_led(void)
{
  if(dyn_master_write_byte(exp_board_id,LED,0x01)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_gpio_clear_led(void)
{
  if(dyn_master_write_byte(exp_board_id,LED,0x00)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_gpio_get_switches(void)
{
  uint8_t switches;

  if(dyn_master_read_byte(exp_board_id,SWITCHES,&switches)==DYN_SUCCESS)
    return switches;
  else
    return 0xFF;
}

/* PWM interface */
uint8_t exp_pwm_start(void)
{
  if(exp_board_pwm_present)
  {
    if(dyn_master_write_byte(exp_board_id,PWM_CONTROL,0x01)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_pwm_stop(void)
{
  if(exp_board_pwm_present)
  {
    if(dyn_master_write_byte(exp_board_id,PWM_CONTROL,0x02)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_pwm_is_running(void)
{
  uint8_t status;

  if(exp_board_pwm_present)
  {
    if(dyn_master_read_byte(exp_board_id,PWM_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&0x04)
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_pwm_enable_ch(pwm_id_t pwm_id)
{
  if(pwm_id<PWM0 || pwm_id>PWM3)
    return 0xFF;
  if(exp_board_pwm_present)
  {
    if(dyn_master_write_byte(exp_board_id,PWM_ENABLE,0x01<<pwm_id)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_pwm_disable_ch(pwm_id_t pwm_id)
{
  if(pwm_id<PWM0 || pwm_id>PWM3)
    return 0xFF;
  if(exp_board_pwm_present)
  {
    if(dyn_master_write_byte(exp_board_id,PWM_ENABLE,0x10<<pwm_id)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_pwm_is_channel_enabled(pwm_id_t pwm_id)
{
  uint8_t status;

  if(pwm_id<PWM0 || pwm_id>PWM3)
    return 0xFF;
  if(exp_board_pwm_present)
  {
    if(dyn_master_read_byte(exp_board_id,PWM_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&(0x10<<pwm_id))
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_pwm_set_frequency(uint16_t freq_hz)
{
  if(exp_board_pwm_present)
  {
    if(dyn_master_write_word(exp_board_id,PWM_FREQ_L,freq_hz)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint16_t exp_pwm_get_frequency(void)
{
  uint16_t value;

  if(exp_board_pwm_present)
  {
    if(dyn_master_read_word(exp_board_id,PWM_FREQ_L,&value)==DYN_SUCCESS)
      return value;
    else
      return 0xFFFF;
  }
  else
    return 0xFFFF;
}

uint8_t exp_pwm_set_duty_cycle(pwm_id_t pwm_id, uint8_t duty)
{
  if(pwm_id<PWM0 || pwm_id>PWM3)
    return 0xFF;
  if(exp_board_pwm_present)
  {
    if(dyn_master_write_byte(exp_board_id,PWM_CH0_DUTY+pwm_id,duty)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_pwm_get_duty_cycle(pwm_id_t pwm_id)
{
  uint8_t value;

  if(pwm_id<PWM0 || pwm_id>PWM3)
    return 0xFF;
  if(exp_board_pwm_present)
  {
    if(dyn_master_read_byte(exp_board_id,PWM_CH0_DUTY+pwm_id,&value)==DYN_SUCCESS)
      return value;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_pwm_num_servos(void)
{
  return exp_board_num_servos;
}

/* DAC interface */
uint8_t exp_dac_start(void)
{
  if(exp_board_dac_present)
  {
    if(dyn_master_write_byte(exp_board_id,DAC_CONTROL,0x01)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_dac_stop(void)
{
  if(exp_board_dac_present)
  {
    if(dyn_master_write_byte(exp_board_id,DAC_CONTROL,0x02)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_dac_is_running(void)
{
  uint8_t status;

  if(exp_board_dac_present)
  {
    if(dyn_master_read_byte(exp_board_id,DAC_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&0x10)
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_dac_enable_ch(dac_id_t dac_id)
{
  if(dac_id<DAC0 || dac_id>DAC1)
    return 0xFF;
  if(exp_board_dac_present)
  {
    if(dyn_master_write_byte(exp_board_id,DAC_ENABLE,0x01<<dac_id)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_dac_disable_ch(dac_id_t dac_id)
{
  if(dac_id<DAC0 || dac_id>DAC1)
    return 0xFF;
  if(exp_board_dac_present)
  {
    if(dyn_master_write_byte(exp_board_id,DAC_ENABLE,0x04<<dac_id)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_dac_is_channel_enabled(dac_id_t dac_id)
{
  uint8_t status;

  if(dac_id<DAC0 || dac_id>DAC1)
    return 0xFF;
  if(exp_board_dac_present)
  {
    if(dyn_master_read_byte(exp_board_id,DAC_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&(0x40<<dac_id))
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_dac_set_voltage(dac_id_t dac_id, uint16_t voltage_mv)
{
  uint32_t value;

  if(dac_id<DAC0 || dac_id>DAC1)
    return 0xFF;
  if(exp_board_dac_present)
  {
    value=((uint32_t)voltage_mv*8192)/1000;
    if(dyn_master_write_word(exp_board_id,DAC_CH0_VOLTAGE_L+dac_id*2,(uint16_t)value)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint16_t exp_dac_get_voltage(dac_id_t dac_id)
{
  uint16_t value;

  if(dac_id<DAC0 || dac_id>DAC1)
    return 0xFFFF;
  if(exp_board_dac_present)
  {
    if(dyn_master_read_word(exp_board_id,DAC_CH0_VOLTAGE_L+dac_id*2,&value)==DYN_SUCCESS)
      return ((uint32_t)value*1000)/8192;
    else
      return 0xFFFF;
  }
  else
    return 0xFFFF;
}

/* compass interface */
uint8_t exp_compass_start(void)
{
  if(dyn_master_write_byte(exp_board_id,COMPASS_CONTROL,0x01)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_compass_stop(void)
{
  if(dyn_master_write_byte(exp_board_id,COMPASS_CONTROL,0x02)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_compass_is_running(void)
{
  uint8_t status;

  if(dyn_master_read_byte(exp_board_id,COMPASS_CONTROL,&status)==DYN_SUCCESS)
  {
    if(status&0x40)
      return 0x01;
    else
      return 0x00;
  }
  else
    return 0xFF;
}

uint8_t exp_compass_start_calibration(void)
{
  if(dyn_master_write_byte(exp_board_id,COMPASS_CONTROL,0x04)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_compass_stop_calibration(void)
{
  if(dyn_master_write_byte(exp_board_id,COMPASS_CONTROL,0x08)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_compass_is_calibrating(void)
{
  uint8_t status;

  if(dyn_master_read_byte(exp_board_id,COMPASS_CONTROL,&status)==DYN_SUCCESS)
  {
    if(status&0x80)
      return 0x01;
    else
      return 0x00;
  }
  else
    return 0xFF;
}

uint16_t exp_compass_get_heading(void)
{
  #if EXP_BOARD_USE_LOOP==1
    return exp_board_int_data[COMPASS_HEADING_VAL_L_block-BLOCK_START_ADDRESS]+exp_board_int_data[COMPASS_HEADING_VAL_H_block-BLOCK_START_ADDRESS]*256;
  #else
  uint16_t heading;

  if(dyn_master_read_word(exp_board_id,COMPASS_HEADING_VAL_L,&heading)==DYN_SUCCESS)
    return heading;
  else
    return 0xFFFF;
  #endif
}

uint16_t exp_compass_get_avg_heading(void)
{
  #if EXP_BOARD_USE_LOOP==1
    return exp_board_int_data[COMPASS_AVG_HEADING_VAL_L_block-BLOCK_START_ADDRESS]+exp_board_int_data[COMPASS_AVG_HEADING_VAL_H_block-BLOCK_START_ADDRESS]*256;
  #else
  uint16_t heading;

  if(dyn_master_read_word(exp_board_id,COMPASS_AVG_HEADING_VAL_L,&heading)==DYN_SUCCESS)
    return heading;
  else
    return 0xFFFF;
  #endif
}

uint8_t exp_compass_set_avg_samples(uint8_t num_samples)
{
  if(dyn_master_write_byte(exp_board_id,COMPASS_NUM_SAMPLES,num_samples)==DYN_SUCCESS)
    return 0x00;
  else
    return 0xFF;
}

uint8_t exp_compass_get_avg_samples(void)
{
  uint8_t samples;

  if(dyn_master_read_byte(exp_board_id,COMPASS_NUM_SAMPLES,&samples)==DYN_SUCCESS)
    return samples;
  else
    return 0xFF;
}

/* TTL UART interface */
uint8_t exp_uart_ttl_start(void)
{
  if(exp_board_uart_ttl_present)
  {
    if(dyn_master_write_byte(exp_board_id,UART_TTL_CONTROL,0x01)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_ttl_stop(void)
{
  if(exp_board_uart_ttl_present)
  {
    if(dyn_master_write_byte(exp_board_id,UART_TTL_CONTROL,0x02)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_ttl_is_running(void)
{
  uint8_t status;

  if(exp_board_uart_ttl_present)
  {
    if(dyn_master_read_byte(exp_board_id,UART_TTL_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&0x20)
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_ttl_set_baudrate(uint16_t baudrate)
{
  if(exp_board_uart_ttl_present)
  {
    if(dyn_master_write_word(exp_board_id,UART_TTL_BAUDRATE_L,baudrate)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint16_t exp_uart_ttl_get_baudrate(void)
{
  uint16_t baudrate;

  if(exp_board_uart_ttl_present)
  {
    if(dyn_master_read_word(exp_board_id,UART_TTL_BAUDRATE_L,&baudrate)==DYN_SUCCESS)
      return baudrate;
    else
      return 0xFFFF;
  }
  else
    return 0xFFFF;
}

uint8_t exp_uart_ttl_send_byte(uint8_t data)
{
  if(exp_board_uart_ttl_present)
  {
    /* write the data to be sent */
    if(dyn_master_write_byte(exp_board_id,UART_TTL_TX_DATA,data)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
    /* start sending the byte */
    if(dyn_master_write_byte(exp_board_id,UART_TTL_CONTROL,0x04)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_ttl_is_sending(void)
{
  uint8_t status;

  if(exp_board_uart_ttl_present)
  {
    if(dyn_master_read_byte(exp_board_id,UART_TTL_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&0x40)
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_ttl_send_vector(uint8_t *data, uint16_t length)
{
  uint16_t i;
 
  if(exp_board_uart_ttl_present)
  {
    for(i=0;i<length;i++)
    {
      exp_uart_ttl_send_byte(data[i]);
      while(exp_uart_ttl_is_sending())
        _delay_ms(10);
    }  
    return 0x00;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_ttl_is_data_available(void)
{
  uint8_t status;

  if(exp_board_uart_ttl_present)
  {
    if(dyn_master_read_byte(exp_board_id,UART_TTL_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&0x80)
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_ttl_receive_data(void)
{
  uint8_t data;

  if(exp_board_uart_ttl_present)
  {
    if(dyn_master_read_byte(exp_board_id,UART_TTL_RX_DATA,&data)==DYN_SUCCESS)
      return data;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

/* USB UART interface */
uint8_t exp_uart_usb_start(void)
{
  if(exp_board_uart_usb_present)
  {
    if(dyn_master_write_byte(exp_board_id,UART_USB_CONTROL,0x01)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_usb_stop(void)
{
  if(exp_board_uart_usb_present)
  {
    if(dyn_master_write_byte(exp_board_id,UART_USB_CONTROL,0x02)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_usb_is_running(void)
{
  uint8_t status;

  if(exp_board_uart_usb_present)
  {
    if(dyn_master_read_byte(exp_board_id,UART_USB_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&0x20)
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_usb_set_baudrate(uint16_t baudrate)
{
  if(exp_board_uart_usb_present)
  {
    if(dyn_master_write_word(exp_board_id,UART_USB_BAUDRATE_L,baudrate)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint16_t exp_uart_usb_get_baudrate(void)
{
  uint16_t baudrate;

  if(exp_board_uart_usb_present)
  {
    if(dyn_master_read_word(exp_board_id,UART_USB_BAUDRATE_L,&baudrate)==DYN_SUCCESS)
      return baudrate;
    else
      return 0xFFFF;
  }
  else
    return 0xFFFF;
}

uint8_t exp_uart_usb_send_byte(uint8_t data)
{
  if(exp_board_uart_usb_present)
  {
    /* write the data to be sent */
    if(dyn_master_write_byte(exp_board_id,UART_USB_TX_DATA,data)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
    /* start sending the byte */
    if(dyn_master_write_byte(exp_board_id,UART_USB_CONTROL,0x04)==DYN_SUCCESS)
      return 0x00;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_usb_is_sending(void)
{
  uint8_t status;

  if(exp_board_uart_usb_present)
  {
    if(dyn_master_read_byte(exp_board_id,UART_USB_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&0x40)
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_usb_send_vector(uint8_t *data, uint16_t length)
{
  uint16_t i;
 
  if(exp_board_uart_usb_present)
  {
    for(i=0;i<length;i++)
    {
      exp_uart_usb_send_byte(data[i]);
      while(exp_uart_usb_is_sending())
        _delay_ms(10);
    }  
    return 0x00;
  }
  return 0xFF;
}

uint8_t exp_uart_usb_is_data_available(void)
{
  uint8_t status;

  if(exp_board_uart_usb_present)
  {
    if(dyn_master_read_byte(exp_board_id,UART_USB_CONTROL,&status)==DYN_SUCCESS)
    {
      if(status&0x80)
        return 0x01;
      else
        return 0x00;
    }
    else
      return 0xFF;
  }
  else
    return 0xFF;
}

uint8_t exp_uart_usb_receive_data(void)
{
  uint8_t data;

  if(exp_board_uart_usb_present)
  {
    if(dyn_master_read_byte(exp_board_id,UART_USB_RX_DATA,&data)==DYN_SUCCESS)
      return data;
    else
      return 0xFF;
  }
  else
    return 0xFF;
}
