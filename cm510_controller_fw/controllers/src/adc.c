#include "cm510_cfg.h"
#include "adc.h"
#include "buzzer.h"
#include <util/delay.h>

/* external functions */
void buzzer_start_alarm(note_t note, uint16_t on_time_100ms, uint16_t off_time_100ms);
void buzzer_stop_alarm(void);
uint8_t buzzer_is_playing_alarm(void);

/* private variables */
volatile uint16_t adc_ch_data[NUM_ADC][ADC_MAX_NUM_SAMPLES];
volatile uint16_t adc_values[NUM_ADC];
volatile uint16_t adc_avg_values[NUM_ADC];
volatile uint8_t adc_current_ch;
volatile uint8_t adc_current_sample;
volatile uint8_t adc_voltage_alarm;

/* private functions */
void adc_set_channel(uint8_t ch_id)
{
  ADMUX&=0xF8;
  ADMUX|=(ch_id&0x07);
  if(ch_id==0)
    PORTA|=0xFC;
  else
    PORTA=~(0x80>>(ch_id-1));// enable the desired channel
}

void adc_set_sample_period(void)
{
  uint16_t prescaler_values[7]={1,8,32,64,128,256,1024};
  uint16_t compare_value;
  uint8_t i;

  for(i=0;i<7;i++)
  {
    compare_value=(((F_CPU/1000)*ADC_SAMPLE_PERIOD_MS)/(prescaler_values[i]*NUM_ADC))-1;
    if(compare_value<256)
    {
      OCR2A=(uint8_t)compare_value;
      TCCR2B=i+1; 
      break;
    }
  }
}

void adc_start_conv(void)
{
  ADCSRA|=(1<<ADSC);
}

uint8_t adc_is_conversion_done(void)
{
  if(ADCSRA&(1<<ADIF))
    return 0x01;
  else
    return 0x00;
}

uint8_t adc_is_period_done(void)
{
  if(TIFR2&(1<<OCF2A))
    return 0x01;
  else
    return 0x00;
}

void adc_loop(void)
{
  uint8_t i;
  uint16_t data;

  if(adc_is_period_done())
  {
    TIFR2|=(1<<OCF2A)|(1<<OCF2B)|(1<<TOV2);// clear interrupt flag
    TCNT2=0x00;// start from 0
    // set the channel to sample
    adc_set_channel(adc_current_ch);      
    _delay_us(12);
    // start a new conversion
    adc_start_conv();
    // wait for the conversion to end
    while(!adc_is_conversion_done());
    ADCSRA|=(1<<ADIF);// clear interrupt flag
    data=ADC;//(ADCL | (ADCH<<8));
    adc_values[adc_current_ch]=data;
    adc_ch_data[adc_current_ch][adc_current_sample]=data;
    // compute the average for the current channel
    data=0;
    for(i=0;i<ADC_MAX_NUM_SAMPLES;i++)
      data+=adc_ch_data[adc_current_ch][i];
    data=data/ADC_MAX_NUM_SAMPLES;
    adc_avg_values[adc_current_ch]=data;
    if(adc_current_ch==NUM_ADC-1)// last channel
      adc_current_sample=(adc_current_sample+1)%ADC_MAX_NUM_SAMPLES;
    if(adc_current_ch==ADC_VCC)// monitor the Voltage supply
    {
//      voltage_mv=((uint32_t)data*(uint32_t)5000*(uint32_t)133)/((uint16_t)1024*(uint16_t)33);
      // compare with hysteresis
      if(adc_voltage_alarm==0x01)
      {
        if(data>=(ADC_VOLTAGE_ALARM_TH+ADC_VOLTAGE_ALARM_WINDOW))// voltage under 11 V
        {
          buzzer_stop_alarm();
          adc_voltage_alarm=0x00;
        }
      }
      else
      {
        if(data<(ADC_VOLTAGE_ALARM_TH-ADC_VOLTAGE_ALARM_WINDOW))// voltage under 11 V
        {
          buzzer_start_alarm(NOTE_LA,30,30);
          adc_voltage_alarm=0x01;
        }
      }
    }
    // select next channel
    adc_current_ch=(adc_current_ch+1)%NUM_ADC;
  }
}

/* public functions */
void init_adc(void)
{
  uint8_t i=0,j=0;

  ADCSRA=(1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);// enable ADC set prescaler 1/128
                                                                  // disable auto trigger, interrupts disabled
  ADCSRB=0x00;// free running mode
  ADMUX=0x00;// use external reference, right ajust the result
  DIDR0=0x00;// enable all ADC channels
  DIDR2=0x00;// enable all adc channels

  // configure the GPIO to enable the sensors
  DDRA  = 0xFC;
  PORTA = 0xFC;// all sensors disabled

  // initialize internal sample variables to 12V to avoid initial voltage alarm
  for(i=0;i<NUM_ADC;i++)
  {
    adc_values[i]=0x0266;//equivalent to 12 V
    adc_avg_values[i]=0x0266;//equivalent to 12 V
    for(j=0;j<ADC_MAX_NUM_SAMPLES;j++)
      adc_ch_data[i][j]=0x0266;
  }
  adc_current_ch=0;
  adc_current_sample=0;
  adc_voltage_alarm=0x00;

  // configure the timer 2 to perform periodic conversions (1 ms period)
  TCCR2A=(1<<WGM21);// CTC mode, no output
  TCNT2=0x00;// start from 0
  TIMSK2=0x00;// interrupts not enabled
  TIFR2=0x07;// clear any pending interrupt
  adc_set_sample_period();// set the default sample period
}

uint16_t get_adc_channel(adc_t channel_id)
{
  return adc_values[(uint8_t)channel_id];
}

uint16_t get_adc_avg_channel(adc_t channel_id)
{
  return adc_avg_values[(uint8_t)channel_id];
}
