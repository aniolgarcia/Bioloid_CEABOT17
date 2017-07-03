#include "buzzer.h"
#include "adc.h"
#include "gpio.h"

/* private variables */
uint8_t buzzer_note_freq[NUM_NOTES];
uint8_t buzzer_playing;
uint8_t buzzer_playing_alarm;
uint16_t buzzer_time_on_100ms;
uint16_t buzzer_time_off_100ms;
uint16_t buzzer_note;

/* private functions */
void buzzer_loop(void)
{
  static uint8_t playing=0x00;
  static uint16_t current_time_on=0,current_time_off=0;

  if(TIFR3&(1<<OCF3A))
  {
    TIFR3|=(1<<OCF3A);// clear any interrupt
    TCNT3H=0x00;                   
    TCNT3L=0x00;
    if(playing==0x00)// currently not playing
    {
      if(buzzer_playing==0x01 || buzzer_playing_alarm==0x01)
      {
        playing=0x01;
        current_time_on=buzzer_time_on_100ms-1;
        current_time_off=buzzer_time_off_100ms;
        if(current_time_on==0)
        {
          // disable PWM output
          TCCR1A&=(~((1<<COM1A1)|(1<<COM1A0)));
        }
      }
    }  
    else// currently playing
    {
      if(buzzer_playing==0x01 || buzzer_playing_alarm==0x01)
      {
        if(current_time_on>0)
        {
          current_time_on--;
          if(current_time_on==0)
          {
            // disable PWM output
            TCCR1A&=(~((1<<COM1A1)|(1<<COM1A0)));
          }
        }
        else
        {
          if(current_time_off>0)
            current_time_off--;
          else
          {
            // enable the output
            TCCR1A&=(~(1<<COM1A0));
            TCCR1A|=(1<<COM1A1);
            current_time_on=buzzer_time_on_100ms;
            current_time_off=buzzer_time_off_100ms;
          }
        }
      }
      else
        playing=0x00;
    }
  }
}

void buzzer_stop_alarm(void)
{
  if(buzzer_playing_alarm==0x01)
  {
    // disable PWM output
    TCCR1A&=(~((1<<COM1A1)|(1<<COM1A0)));
    buzzer_playing_alarm=0x00;
  }
}

void buzzer_start_alarm(note_t note, uint16_t on_time_100ms,uint16_t off_time_100ms)
{
  if(buzzer_playing_alarm)// the alram is not playing
    buzzer_stop_alarm();
  else if(buzzer_playing)// if the normal buzzer is playing, stop it.
    buzzer_stop();
  // set top count value
  ICR1H = 0x00;
  ICR1L = buzzer_note_freq[note];
  // Set the compare value to half the period
  OCR1AH = 0x00;
  OCR1AL = buzzer_note_freq[note]>>1;
  buzzer_time_on_100ms=on_time_100ms;
  buzzer_time_off_100ms=off_time_100ms;
  buzzer_note=note;
  // enable PWM output
  TCCR1A&=(~(1<<COM1A0));
  TCCR1A|=(1<<COM1A1);
  buzzer_playing_alarm=0x01;
}

/* public functions */
void init_buzzer(void)
{
  // configure the GPIO
  DDRB  |= 0x20;// configure bit 5 as output
  PORTB &= 0xDF;// clear the ouput

  // initialize timer 1 to work as PWM (Fast PWM mode)
  TIMSK1=0x00;// disable all interrupts
  TIFR1=0x2F;// clear any pending interrupt
  TCNT1H=0x00;                   
  TCNT1L=0x00;
  // set PWM mode with ICR top-count
  TCCR1A=0x00;
  TCCR1A&=~(1<<WGM10);
  TCCR1A|=(1<<WGM11);
  TCCR1B=0x00;
  TCCR1B|=(1<<WGM12);
  TCCR1B|=(1<<WGM13);
  // clear output compare value A
  OCR1AH=0x00;
  OCR1AL=0x00;
  TCCR1B&=0xF8;// set the prescaler to 1024
  TCCR1B|=0x05;// set the prescaler to 1024

  // initialize timer3 for buzzer time control (100ms period)
  TIMSK3=0x00;// disable all interrupts
  TIFR3=0x2F;// clear any pending interrupt
  TCNT3H=0x00;                   
  TCNT3L=0x00;
  // set PWM mode with ICR top-count (CTC mode)
  TCCR3A=0x00;
  TCCR3A&=~(1<<WGM30);
  TCCR3A&=~(1<<WGM31);
  TCCR3B=0x00;
  TCCR3B|=(1<<WGM32);
  TCCR3B&=~(1<<WGM33);
  // clear output compare value A
  OCR3AH=0x18;
  OCR3AL=0x6A;
  TCCR3B&=0xF8;// set the prescaler to 256
  TCCR3B|=0x04;

  // initialize the note frequencies
  buzzer_note_freq[0]=29;
  buzzer_note_freq[1]=26;
  buzzer_note_freq[2]=24;
  buzzer_note_freq[3]=22;
  buzzer_note_freq[4]=20;
  buzzer_note_freq[5]=18;
  buzzer_note_freq[6]=16;
  buzzer_playing=0x00;
  buzzer_playing_alarm=0x00;
  buzzer_time_on_100ms=0x01;
  buzzer_time_off_100ms=0x01;
  buzzer_note=NOTE_DO;
}

void buzzer_start(note_t note,uint16_t on_time_100ms,uint16_t off_time_100ms)
{
  if(buzzer_playing_alarm==0x00)
  {
    if(buzzer_playing==0x00)
    {
      // set top count value
      ICR1H = 0x00;
      ICR1L = buzzer_note_freq[note];
      // Set the compare value to half the period
      OCR1AH = 0x00;
      OCR1AL = buzzer_note_freq[note]>>1;
      buzzer_note=note;
      buzzer_time_on_100ms=on_time_100ms;
      buzzer_time_off_100ms=off_time_100ms;
      // enable PWM output
      TCCR1A&=(~(1<<COM1A0));
      TCCR1A|=(1<<COM1A1);
      buzzer_playing=0x01;
    }
  }
}  

void buzzer_change_note(note_t note)
{
  if(buzzer_playing_alarm==0x00)
  {
    if(buzzer_playing==0x01)
    {
      // set top count value
      ICR1H = 0x00;
      ICR1L = buzzer_note_freq[note];
      // Set the compare value to half the period
      OCR1AH = 0x00;
      OCR1AL = buzzer_note_freq[note]>>1;
      buzzer_note=note;
    }
  }
}

void buzzer_change_on_time(uint16_t time_100ms)
{
  if(buzzer_playing_alarm==0x00)
  {
    if(buzzer_playing==0x01)
      buzzer_time_on_100ms=time_100ms;
  }
}

void buzzer_change_off_time(uint16_t time_100ms)
{
  if(buzzer_playing_alarm==0x00)
  {
    if(buzzer_playing==0x01)
      buzzer_time_off_100ms=time_100ms;
  }
}

void buzzer_stop(void)
{
  if(buzzer_playing_alarm==0x00)
  {
    if(buzzer_playing==0x01)
    {
      // disable PWM output
      TCCR1A&=(~((1<<COM1A1)|(1<<COM1A0)));
      buzzer_playing=0x00;
    }
  }
}

uint8_t buzzer_is_playing(void)
{
  return buzzer_playing;
}

uint8_t buzzer_is_playing_alarm(void)
{
  return buzzer_playing_alarm;
}
