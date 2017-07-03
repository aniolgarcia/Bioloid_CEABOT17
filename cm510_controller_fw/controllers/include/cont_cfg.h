#ifndef _CONT_CFG_H
#define _CONT_CFG_H

#include "buzzer.h"

// number of samples to average
#ifndef ADC_MAX_NUM_SAMPLES
  #define ADC_MAX_NUM_SAMPLES         16
#endif

#ifndef ADC_SAMPLE_PERIOD_MS
  #define ADC_SAMPLE_PERIOD_MS        10
#endif 

#ifndef ADC_VOLTAGE_ALARM_TH
  #define ADC_VOLTAGE_ALARM_TH        450
#endif

#ifndef ADC_VOLTAGE_ALARM_WINDOW      
  #define ADC_VOLTAGE_ALARM_WINDOW    20
#endif

#ifndef ADC_VOLTAGE_ALARM_NOTE
  #define ADC_VOLTAGE_ALARM_NOTE      NOTE_LA
#endif

#ifndef ADC_VOLTAGE_ALARM_TIME_ON
  #define ADC_VOLTAGE_ALARM_TIME_ON   30
#endif

#ifndef ADC_VOLTAGE_ALARM_TIME_OFF
  #define ADC_VOLTAGE_ALARM_TIME_OFF  30
#endif

#endif
