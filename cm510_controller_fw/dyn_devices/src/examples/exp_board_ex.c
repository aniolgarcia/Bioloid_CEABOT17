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

#include "exp_board.h"
#include "dynamixel_master.h"
#include "serial_console.h"
#include <stdio.h>
#include <util/delay.h>

int main(void)
{
  uint8_t i,data;

  serial_console_init(57600);
  dyn_master_init();
  sei();

  _delay_ms(4000);
  if(exp_board_init(0xC0)==0x00)
  {
    cm510_printf("expansion baord found\n");
    /* GPIO test */
/*    if(exp_gpio_config(GPIO0,GPIO_OUT))
      cm510_printf("Error configuring the GPIO0 pin\n");
    if(exp_gpio_config(GPIO1,GPIO_IN))
      cm510_printf("Error configuring the GPIO1 pin\n");
    for(i=0;i<10;i++)
    {
      if(exp_gpio_set_value(0,1))
        cm510_printf("Error Error setting the value of the GPIO0 pin\n");
      if(exp_gpio_get_value(GPIO1)==0x01)
        cm510_printf("GPIO pin is HIGH\n");
      else if(exp_gpio_get_value(GPIO1)==0x00)
        cm510_printf("GPIO pin is LOW\n");
      else
        cm510_printf("Error getting the GPIO1 value\n");
      _delay_ms(1000);
      if(exp_gpio_set_value(0,0))
        cm510_printf("Error Error setting the value of the GPIO1 pin\n");
      if(exp_gpio_get_value(GPIO1)==0x00)
        cm510_printf("GPIO pin is LOW\n");
      else if(exp_gpio_get_value(GPIO1)==0x01)
        cm510_printf("GPIO pin is HIGH\n");
      else
        cm510_printf("Error getting the GPIO1 value\n");
      _delay_ms(1000);
    }*/
    /* LED and switches test */
/*    for(i=0;i<10;i++)
    {
      exp_gpio_set_led();
      cm510_printf("Switches value: %d\n",exp_gpio_get_switches());
      _delay_ms(1000);
      exp_gpio_clear_led();
      cm510_printf("Switches value: %d\n",exp_gpio_get_switches());
      _delay_ms(1000);
    }*/
    /* PWM test */
/*    exp_pwm_start();
    exp_pwm_set_frequency(1000);
    exp_pwm_enable_ch(PWM0);
    for(i=0;i<10;i++)
    {
      exp_pwm_set_duty_cycle(PWM0,25);
      _delay_ms(1000);
      exp_pwm_set_duty_cycle(PWM0,50);
      _delay_ms(1000);
      exp_pwm_set_duty_cycle(PWM0,75);
      _delay_ms(1000);
    }
    exp_pwm_stop();*/
    /* DAC test */
/*    exp_dac_start();
    exp_dac_enable_ch(DAC0);
    for(i=0;i<10;i++)
    {
      exp_dac_set_voltage(DAC0,1000); 
      cm510_printf("Volatge: %d mV\n",exp_dac_get_voltage(DAC0));
      _delay_ms(1000);
      exp_dac_set_voltage(DAC0,2000); 
      cm510_printf("Volatge: %d mV\n",exp_dac_get_voltage(DAC0));
      _delay_ms(1000);
      exp_dac_set_voltage(DAC0,3000); 
      cm510_printf("Volatge: %d mV\n",exp_dac_get_voltage(DAC0));
      _delay_ms(1000);
      exp_dac_set_voltage(DAC0,4000); 
      cm510_printf("Volatge: %d mV\n",exp_dac_get_voltage(DAC0));
      _delay_ms(1000);
    }
    dac_stop();*/
    /* compass test */
    exp_compass_start();
    cm510_printf("Number of samples to average: %d\n",exp_compass_get_avg_samples());
    for(i=0;i<100;i++)
    {
      cm510_printf("Current heading: %d\n",exp_compass_get_heading());
      cm510_printf("Current averaged heading: %d\n",exp_compass_get_avg_heading());
      _delay_ms(1000);
    }
    exp_compass_stop();
    /* ADC test */
/*    exp_adc_start();
    cm510_printf("Number of samples to average: %d\n",exp_adc_get_average_samples());
    cm510_printf("Sampling period: %d ms\n",exp_adc_get_sample_period());
    for(i=0;i<100;i++)
    {
      cm510_printf("Current voltage at ADC0: %d mV\n",exp_adc_get_channel(ADC0));
      cm510_printf("Current averaged voltage at ADC0: %d mV\n",exp_adc_get_avg_channel(ADC0));
      _delay_ms(1000);
    }
    exp_adc_stop();*/
    /* UART USB test */
/*    exp_uart_usb_start();
    cm510_printf("current baudrate: %d\n",exp_uart_usb_get_baudrate());
    for(i=0;i<10;i++)
    {
      while(!exp_uart_usb_is_data_available())
        _delay_ms(100);
      data=exp_uart_usb_receive_data();
      cm510_printf("Received data: %d\n",data);
      exp_uart_usb_send_byte(data);
      while(exp_uart_usb_is_sending())
        _delay_ms(100);
      cm510_printf("data sent\n");
    }
    exp_uart_usb_stop();*/
  }
  else
    cm510_printf("expansion board not found\n"); 

  while(1);
}
