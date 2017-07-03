#include "gpio.h"

typedef struct{
  volatile uint8_t pressed;
  volatile uint8_t rising_edge;
  volatile uint8_t falling_edge;
  volatile uint8_t old_state;
  volatile uint8_t state_count;
}TButtonState;

/* private variables */
TButtonState button_up;
TButtonState button_down;
TButtonState button_left;
TButtonState button_right;
TButtonState button_start;

/* private functions */
void pushbuttons_loop(void)
{
  if((~PINE & BTN_UP)!=0x00)  
  {
    button_up.pressed=0x01;
    if(button_up.old_state==0x00)
    {
      button_up.state_count++;
      if(button_up.state_count==REBOUND_COUNT)
      {
        button_up.rising_edge=0x01; 
        button_up.old_state=0x01;
        button_up.state_count=0;
      }
    }
    else
      button_up.state_count=0;
  }
  else
  {
    if(button_up.old_state==0x01)
    {
      button_up.state_count++;
      if(button_up.state_count==REBOUND_COUNT)
      {
        button_up.falling_edge=0x01;
        button_up.old_state=0x00;
        button_up.state_count=0;
      }
    }
    else
      button_up.state_count=0;
  }
  if((~PINE & BTN_DOWN)!=0x00)  
  {
    button_down.pressed=0x01;
    if(button_down.old_state==0x00)
    {
      button_down.state_count++;
      if(button_down.state_count==REBOUND_COUNT)
      {
        button_down.rising_edge=0x01; 
        button_down.old_state=0x01;
        button_down.state_count=0;
      }
    }
    else
      button_down.state_count=0;
  }
  else
  {
    if(button_down.old_state==0x01)
    {
      button_down.state_count++;
      if(button_down.state_count==REBOUND_COUNT)
      {
        button_down.falling_edge=0x01;
        button_down.old_state=0x00;
        button_down.state_count=0;
      }
    }
    else
      button_down.state_count=0;
  }
  if((~PINE & BTN_LEFT)!=0x00)  
  {
    button_left.pressed=0x01;
    if(button_left.old_state==0x00)
    {
      button_left.state_count++;
      if(button_left.state_count==REBOUND_COUNT)
      {
        button_left.rising_edge=0x01; 
        button_left.old_state=0x01;
        button_left.state_count=0;
      }
    }
    else
      button_left.state_count=0;
  }
  else
  {
    if(button_left.old_state==0x01)
    {
      button_left.state_count++;
      if(button_left.state_count==REBOUND_COUNT)
      {
        button_left.falling_edge=0x01;
        button_left.old_state=0x00;
        button_left.state_count=0;
      }
    }
    else
      button_left.state_count=0;
  }
  if((~PINE & BTN_RIGHT)!=0x00)  
  {
    button_right.pressed=0x01;
    if(button_right.old_state==0x00)
    {
      button_right.state_count++;
      if(button_right.state_count==REBOUND_COUNT)
      {
        button_right.rising_edge=0x01; 
        button_right.old_state=0x01;
        button_right.state_count=0;
      }
    }
    else
      button_right.state_count=0;
  }
  else
  {
    if(button_right.old_state==0x01)
    {
      button_right.state_count++;
      if(button_right.state_count==REBOUND_COUNT)
      {
        button_right.falling_edge=0x01;
        button_right.old_state=0x00;
        button_right.state_count=0;
      }
    }
    else
      button_right.state_count=0;
  }
  if((~PIND & BTN_START)!=0x00)  
  {
    button_start.pressed=0x01;
    if(button_start.old_state==0x00)
    {
      button_start.state_count++;
      if(button_start.state_count==REBOUND_COUNT)
      {
        button_start.rising_edge=0x01; 
        button_start.old_state=0x01;
        button_start.state_count=0;
      }
    }
    else
      button_start.state_count=0;
  }
  else
  {
    if(button_start.old_state==0x01)
    {
      button_start.state_count++;
      if(button_start.state_count==REBOUND_COUNT)
      {
        button_start.falling_edge=0x01;
        button_start.old_state=0x00;
        button_start.state_count=0;
      }
    }
    else
      button_start.state_count=0;
  }
}

/* public functions */
void init_leds(void)
{
  // configure the PORTC as outputs for the LED's
  DDRC  = 0x7F;
  // all LED's off except for the Battery LED
  PORTC = 0x7E;

  /* initialize internal variables */
  button_up.pressed=0x00;
  button_up.rising_edge=0x00;
  button_up.falling_edge=0x00;
  button_up.old_state=0x00;
  button_up.state_count=0x00;
  button_down.pressed=0x00;
  button_down.rising_edge=0x00;
  button_down.falling_edge=0x00;
  button_down.old_state=0x00;
  button_down.state_count=0x00;
  button_left.pressed=0x00;
  button_left.rising_edge=0x00;
  button_left.falling_edge=0x00;
  button_left.old_state=0x00;
  button_left.state_count=0x00;
  button_right.pressed=0x00;
  button_right.rising_edge=0x00;
  button_right.falling_edge=0x00;
  button_right.old_state=0x00;
  button_right.state_count=0x00;
  button_start.pressed=0x00;
  button_start.rising_edge=0x00;
  button_start.falling_edge=0x00;
  button_start.old_state=0x00;
  button_start.state_count=0x00;
}

void turn_led_on(leds_t led_id)
{
  switch(led_id)
  {
    case LED_BAT: PORTC &= ~LED_BAT;
                  break;
    case LED_TxD: PORTC &= ~LED_TxD;
                  break;
    case LED_RxD: PORTC &= ~LED_RxD;
                  break;
    case LED_AUX: PORTC &= ~LED_AUX;
                  break;
    case LED_MANAGE: PORTC &= ~LED_MANAGE;
                     break;
    case LED_PROGRAM: PORTC &= ~LED_PROGRAM;
                      break;
    case LED_PLAY: PORTC &= ~LED_PLAY;
                   break;
  }
}

void turn_led_off(leds_t led_id)
{
  switch(led_id)
  {
    case LED_BAT: PORTC |= LED_BAT;
                  break;
    case LED_TxD: PORTC |= LED_TxD;
                  break;
    case LED_RxD: PORTC |= LED_RxD;
                  break;
    case LED_AUX: PORTC |= LED_AUX;
                  break;
    case LED_MANAGE: PORTC |= LED_MANAGE;
                     break;
    case LED_PROGRAM: PORTC |= LED_PROGRAM;
                      break;
    case LED_PLAY: PORTC |= LED_PLAY;
                   break;
  }
}

void toggle_led(leds_t led_id)
{
  switch(led_id)
  {
    case LED_BAT: PORTC ^= LED_BAT;
                  break;
    case LED_TxD: PORTC ^= LED_TxD;
                  break;
    case LED_RxD: PORTC ^= LED_RxD;
                  break;
    case LED_AUX: PORTC ^= LED_AUX;
                  break;
    case LED_MANAGE: PORTC ^= LED_MANAGE;
                     break;
    case LED_PROGRAM: PORTC ^= LED_PROGRAM;
                      break;
    case LED_PLAY: PORTC ^= LED_PLAY;
                   break;
  }
}

uint8_t is_led_on(leds_t led_id)
{
  uint8_t value=0x00;

  switch(led_id)
  {
    case LED_BAT: value = (PINC & LED_BAT)>>6;
                  break;
    case LED_TxD: value = (PINC & LED_TxD)>>5;
                  break;
    case LED_RxD: value = (PINC & LED_RxD)>>4;
                  break;
    case LED_AUX: value = (PINC & LED_AUX)>>3;
                  break;
    case LED_MANAGE: value = (PINC & LED_MANAGE)>>2;
                     break;
    case LED_PROGRAM: value = (PINC & LED_PROGRAM)>>1;
                      break;
    case LED_PLAY: value = (PINC & LED_PLAY);
                   break;
  }

  return value;  
}

void init_buttons(void)
{
  DDRD  &= 0xFE;
  PORTD |= 0x01;
	
  DDRE  &= 0x0F;
  PORTE |= 0xF0;
}

uint8_t is_button_pressed(pushbuttons_t button_id)
{
  unsigned char value=0x00;

  switch(button_id)
  {
    case BTN_UP: if(button_up.pressed)
                 {
                   value=0x01;
                   button_up.pressed=0x00;
                 }
                 break;
    case BTN_DOWN: if(button_down.pressed)
                   {
                     value=0x01;
                     button_down.pressed=0x00;
                   }
                   break;
    case BTN_LEFT: if(button_left.pressed)
                   {
                     value=0x01;
                     button_left.pressed=0x00;
                   }
                   break;
    case BTN_RIGHT: if(button_right.pressed)
                    {
                      value=0x01;
                      button_right.pressed=0x00;
                    }
                    break;
    case BTN_START: if(button_start.pressed)
                    {
                      value=0x01;
                      button_start.pressed=0x00;
                    }
                    break;
  }

  return value;
}

uint8_t is_button_rising_edge(pushbuttons_t button_id)
{
  unsigned char value=0x00;

  switch(button_id)
  {
    case BTN_UP: if(button_up.rising_edge)
                 {
                   value=0x01;
                   button_up.rising_edge=0x00;
                 }
                 break;
    case BTN_DOWN: if(button_down.rising_edge)
                   {
                     value=0x01;
                     button_down.rising_edge=0x00;
                   }
                   break;
    case BTN_LEFT: if(button_left.rising_edge)
                   {
                     value=0x01;
                     button_left.rising_edge=0x00;
                   }
                   break;
    case BTN_RIGHT: if(button_right.rising_edge)
                    {
                      value=0x01;
                      button_right.rising_edge=0x00;
                    }
                    break;
    case BTN_START: if(button_start.rising_edge)
                    {
                      value=0x01;
                      button_start.rising_edge=0x00;
                    }
                    break;
  }

  return value;
}

uint8_t is_button_falling_edge(pushbuttons_t button_id)
{
  unsigned char value=0x00;

  switch(button_id)
  {
    case BTN_UP: if(button_up.falling_edge)
                 {
                   value=0x01;
                   button_up.falling_edge=0x00;
                 }
                 break;
    case BTN_DOWN: if(button_down.falling_edge)
                   {
                     value=0x01;
                     button_down.falling_edge=0x00;
                   }
                   break;
    case BTN_LEFT: if(button_left.falling_edge)
                   {
                     value=0x01;
                     button_left.falling_edge=0x00;
                   }
                   break;
    case BTN_RIGHT: if(button_right.falling_edge)
                    {
                      value=0x01;
                      button_right.falling_edge=0x00;
                    }
                    break;
    case BTN_START: if(button_start.falling_edge)
                    {
                      value=0x01;
                      button_start.falling_edge=0x00;
                    }
                    break;
  }

  return value;
}

