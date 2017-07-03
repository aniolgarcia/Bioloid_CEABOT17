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

#ifndef _EXP_BOARD_H
#define _EXP_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include "dyn_common.h"

/* GPIO definitions */
#define GPIO_IN                  0
#define GPIO_OUT                 1
typedef enum {GPIO0=0,GPIO1,GPIO2,GPIO3,GPIO4,GPIO5,GPIO6,GPIO7,GPIO8,GPIO9,GPIO10,GPIO11,GPIO12,GPIO13,GPIO14,GPIO15,GPIO16,GPIO17,GPIO18} gpio_id_t;

/* PWM definitions */
typedef enum {PWM0=0,PWM1,PWM2,PWM3} pwm_id_t;

/* DAC definitions */
typedef enum {DAC0=0,DAC1} dac_id_t;

/* ADC definitions */
typedef enum {ADC0=0,ADC1,ADC2,ADC3,ADC4,ADC5,ADC6,ADC7} adc_id_t;

uint8_t exp_board_init(uint8_t board_id);
uint8_t exp_board_is_present(void);

/* ADC interface */
uint8_t exp_adc_start(void);
uint8_t exp_adc_stop(void);
uint8_t exp_adc_is_running(void);
uint8_t exp_adc_set_average_samples(uint8_t num);
uint8_t exp_adc_get_average_samples(void);
uint8_t exp_adc_set_sample_period(uint8_t period_ms);
uint8_t exp_adc_get_sample_period(void);
uint16_t exp_adc_get_channel(adc_id_t channel_id);
uint16_t exp_adc_get_avg_channel(adc_id_t channel_id);
/* GPIO interface */
uint8_t exp_gpio_config(gpio_id_t gpio_id, uint8_t dir);
uint8_t exp_gpio_get_value(gpio_id_t gpio_id);
uint8_t exp_gpio_set_value(gpio_id_t gpio_id, uint8_t level);
uint8_t exp_gpio_set_led(void);
uint8_t exp_gpio_clear_led(void);
uint8_t exp_gpio_get_switches(void); 
/* PWM interface */
uint8_t exp_pwm_start(void);
uint8_t exp_pwm_stop(void);
uint8_t exp_pwm_is_running(void);
uint8_t exp_pwm_enable_ch(pwm_id_t pwm_id);
uint8_t exp_pwm_disable_ch(pwm_id_t pwm_id);
uint8_t exp_pwm_is_channel_enabled(pwm_id_t pwm_id);
uint8_t exp_pwm_set_frequency(uint16_t freq_hz);
uint16_t exp_pwm_get_frequency(void);
uint8_t exp_pwm_set_duty_cycle(pwm_id_t pwm_id, uint8_t duty);
uint8_t exp_pwm_get_duty_cycle(pwm_id_t pwm_id);
uint8_t exp_pwm_num_servos(void);
/* DAC interface */
uint8_t exp_dac_start(void);
uint8_t exp_dac_stop(void);
uint8_t exp_dac_is_running(void);
uint8_t exp_dac_enable_ch(dac_id_t dac_id);
uint8_t exp_dac_disable_ch(dac_id_t dac_id);
uint8_t exp_dac_is_channel_enabled(dac_id_t dac_id);
uint8_t exp_dac_set_voltage(dac_id_t dac_id, uint16_t voltage_mv);
uint16_t exp_dac_get_voltage(dac_id_t dac_id);
/* compass interface */
uint8_t exp_compass_start(void);
uint8_t exp_compass_stop(void);
uint8_t exp_compass_is_running(void);
uint8_t exp_compass_start_calibration(void);
uint8_t exp_compass_stop_calibration(void);
uint8_t exp_compass_is_calibrating(void);
uint8_t exp_compass_set_avg_samples(uint8_t num_samples);
uint8_t exp_compass_get_avg_samples(void);
uint16_t exp_compass_get_heading(void);
uint16_t exp_compass_get_avg_heading(void);
/* TTL UART interface */
uint8_t exp_uart_ttl_start(void);
uint8_t exp_uart_ttl_stop(void);
uint8_t exp_uart_ttl_is_running(void);
uint8_t exp_uart_ttl_set_baudrate(uint16_t baudrate);
uint16_t exp_uart_ttl_get_baudrate(void);
uint8_t exp_uart_ttl_send_byte(uint8_t data);
uint8_t exp_uart_ttl_is_sending(void);
uint8_t exp_uart_ttl_send_vector(uint8_t *data, uint16_t length);
uint8_t exp_uart_ttl_is_data_available(void);
uint8_t exp_uart_ttl_receive_data(void);
/* USB UART interface */
uint8_t exp_uart_usb_start(void);
uint8_t exp_uart_usb_stop(void);
uint8_t exp_uart_usb_is_running(void);
uint8_t exp_uart_usb_set_baudrate(uint16_t baudrate);
uint16_t exp_uart_usb_get_baudrate(void);
uint8_t exp_uart_usb_send_byte(uint8_t data);
uint8_t exp_uart_usb_is_sending(void);
uint8_t exp_uart_usb_send_vector(uint8_t *data, uint16_t length);
uint8_t exp_uart_usb_is_data_available(void);
uint8_t exp_uart_usb_receive_data(void);

#ifdef __cplusplus
}
#endif

#endif
