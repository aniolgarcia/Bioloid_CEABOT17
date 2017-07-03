#ifndef _GPIO_H
#define _GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

// leds interface
typedef enum {LED_BAT=0x01,LED_TxD=0x02,LED_RxD=0x04,LED_AUX=0x08,LED_MANAGE=0x10,LED_PROGRAM=0x20,LED_PLAY=0x40} leds_t;
#define NUM_LEDS      7

// buttons interface
typedef enum {BTN_UP=0x10,BTN_DOWN=0x20,BTN_LEFT=0x40,BTN_RIGHT=0x80,BTN_START=0x01} pushbuttons_t;
#define NUM_BUTTONS 5

#define REBOUND_COUNT 20

/**
 * \brief Function to initialize the LED's module
 *
 * This function initializes the GPIO pins associated to the LED's as outputs.
 * The LED's are connected to the 7 lower bits of port C.
 *
 * \return this function always returns 0
 */
void init_leds(void);
/**
 * \brief Function to turn on a given LED
 *
 * This function turns on a given LED, what ever its current state. The LED
 * is identified by one of the macros defined above.
 *
 * \param led_id identifier of the LED to turn on
 * 
 * \return 0 if the function is successfull, 0xFF otherwise
 */
void turn_led_on(leds_t led_id);
/**
 * \brief Function to turn off a given LED
 *
 * This function turns off a given LED, what ever its current state. The LED
 * is identified by one of the macros defined above.
 *
 * \param led_id identifier of the LED to turn off
 * 
 * \return 0 if the function is successfull, 0xFF otherwise
 */
void turn_led_off(leds_t led_id);
/**
 * \brief Function to toggle the current state of a LED
 *
 * This function changes the current state of a given LED. If it is on, it is turned
 * off, and viceversa. The LED is identified by one of the macros defined above.
 * 
 * \param led_id identifier of the LED to toggle
 * 
 * \return 0 if the function is successfull, 0xFF otherwise
 */
void toggle_led(leds_t led_id);
/**
 * \brief Function to check wether the LED is on or not
 *
 * This function checks if a given LED is turned on or off. The LED is identified 
 * by one of the macros defined above.
 * 
 * \param led_id identifier of the LED to check
 * 
 * \return 0 if the LED is turned off, 1 if the led is turned on and 0xFF in 
 *         case of any error.
 */
uint8_t is_led_on(leds_t led_id);

/**
 * \brief Function to initialize the buttons module
 *
 * This function initializes the GPIO pins associated to the buttons as inputs.
 * The buttons are connected to the four most significant bits of port E and the 
 * least significant bit of port D.
 *
 * This function also sets to NULL the callback functions for all the buttons and
 * initializes the current state of the buttons.
 * 
 * \return this function always returns 0
 */
void init_buttons(void);
/**
 * \brief Function to check whether a button is pressed or not
 *
 * This functions checks if a given value is pressed or not. The desired button 
 * is identified with one of the macros defined above.
 * 
 * \param button_id identifier of the button to assign a callback. This must be any of 
 *                  the macros defined above.
 * 
 * \return 0 if the button is not pressed, 1 if it is pressed and 0xFF in 
 *         case of any error.
 */
uint8_t is_button_pressed(pushbuttons_t button_id);

uint8_t is_button_rising_edge(pushbuttons_t button_id);

uint8_t is_button_falling_edge(pushbuttons_t button_id);

#ifdef __cplusplus
}
#endif

#endif
