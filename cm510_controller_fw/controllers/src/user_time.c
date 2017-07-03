#include "user_time.h"

/* private variables */
uint8_t user_time_single;
uint16_t user_time_time;
uint16_t user_time_period;
volatile uint8_t user_time_done;
volatile uint8_t user_time_period_done;

/* private funcitons */
void user_time_loop(void)
{
  static uint16_t count=0;

  if(TIFR5&(1<<OCF5A))
  {
    TIFR5|=(1<<OCF5A);// clear any interrupt
    TCNT5H=0x00;                   
    TCNT5L=0x00;
    if(user_time_single==0x01)
    {
      if(count==user_time_time)
      {
        count=0;
        user_time_done=0x01;
        TCCR5B&=0xF8;// disable timer 
      }
      else
        count++;
    }
    else
    {
      if(count==user_time_period)
      {
        count=0;
        user_time_period_done=0x01;
      }
      else
        count++;
    }
  }
}

/* public functions */
void init_user_time(void)
{
  // initialize timer3 for buzzer time control (100ms period)
  TIMSK5=0x00;// disable all interrupts
  TIFR5=0x2F;// clear any pending interrupt
  TCNT5H=0x00;                   
  TCNT5L=0x00;
  // set PWM mode with ICR top-count (CTC mode)
  TCCR5A=0x00;
  TCCR5A&=~(1<<WGM50);
  TCCR5A&=~(1<<WGM51);
  TCCR5B=0x00;
  TCCR5B|=(1<<WGM52);
  TCCR5B&=~(1<<WGM53);
  // clear output compare value A
  OCR5AH=0x18;
  OCR5AL=0x6A;
  TCCR5B&=0xF8;// disable timer by default

  /* initialize internal variables */
  user_time_single=0x00;
  user_time_time=0x00;
  user_time_period=0x00;
  user_time_done=0x00;
  user_time_period_done=0x00;
}

void user_time_set_period(uint16_t period_ms)
{
  user_time_single=0x00;
  user_time_period=period_ms/100;
  user_time_period_done=0x00;
  TCNT5H=0x00;                   
  TCNT5L=0x00;
  TIFR5=0x2F;// clear any pending interrupt
  TCCR5B&=0xF8;// disable timer by default
  TCCR5B|=0x04;
}

void user_time_set_one_time(uint16_t time_ms)
{
  user_time_single=0x01;
  user_time_time=time_ms/100;
  user_time_done=0x00;
  TCNT5H=0x00;                   
  TCNT5L=0x00;
  TIFR5=0x2F;// clear any pending interrupt
  TCCR5B&=0xF8;// disable timer by default
  TCCR5B|=0x04;
}

uint8_t user_time_is_period_done(void)
{
  if(user_time_period_done==0x01)
  {
    user_time_period_done=0x00;
    return 0x01;
  }
  else
    return 0x00;
}

uint8_t user_time_is_done(void)
{
  if(user_time_done==0x01)
  {
    user_time_done=0x00;
    return 0x01;
  }
  else
    return 0x00;
}

void user_time_stop(void)
{
  TCCR5B&=0xF8;// disable timer 
  TIFR5|=(1<<OCF5A);// clear any interrupt
  TCNT5H=0x00;                   
  TCNT5L=0x00;
  user_time_period_done=0x00;  
  user_time_done=0x00;  
}
