#ifndef _CM510_CFG_H
#define _CM510_CFG_H

#include "buzzer.h"

// controller configuration parameters
#define ADC_MAX_NUM_SAMPLES                     16
#define ADC_SAMPLE_PERIOD_MS                    40
#define ADC_VOLTAGE_ALARM_TH                    450 
#define ADC_VOLTAGE_ALARM_WINDOW                20
#define ADC_VOLTAGE_ALARM_NOTE                  NOTE_LA
#define ADC_VOLTAGE_ALARM_TIME_ON               30
#define ADC_VOLTAGE_ALARM_TIME_OFF              30

// communication configuration parameters
#define DYN_MASTER_MAX_TX_BUFFER_LEN            128
#define DYN_MASTER_MAX_RX_BUFFER_LEN            128
#define DYN_MASTER_DEFAULT_BAUDRATE             1000000
#define DYN_MASTER_DEFAULT_TIMEOUT_US           2000
#define SERIAL_CONSOLE_MAX_BUFFER_LEN           128

// motion configuration parameters
#define MANAGER_MAX_NUM_SERVOS                  32 
#define MANAGER_MISSING_SERVOS_ALARM_NOTE       NOTE_SOL
#define MANAGER_MISSING_SERVOS_ALARM_TIME_ON    10
#define MANAGER_MISSING_SERVOS_ALARM_TIME_OFF   100
#define BALANCE_GYRO_CAL_NUM_SAMPLES            10
#define BALANCE_GYRO_X_CHANNEL                  4
#define BALANCE_GYRO_Y_CHANNEL                  3
#define BALANCE_GYRO_CAL_FAILED_ALARM_NOTE      NOTE_SOL
#define BALANCE_GYRO_CAL_FAILED_ALARM_TIME_ON   10
#define BALANCE_GYRO_CAL_FAILED_ALARM_TIME_OFF  100
#define BALANCE_MAX_CAL_GYRO_ERROR              20.0
#define BALANCE_FORWARD_FALL_THD_VALUE          200
#define BALANCE_BACKWARD_FALL_THD_VALUE         (-200)
#define BALANCE_KNEE_GAIN                       (4.0/54.0)
#define BALANCE_ANKLE_PITCH_GAIN                (4.0/18.0)
#define BALANCE_ANKLE_ROLL_GAIN                 (4.0/20.0)
#define BALANCE_HIP_ROLL_GAIN                   (4.0/40.0)

// expansion board configuration parameters
#define EXP_BOARD_USE_LOOP                      1
#define EXP_BOARD_ID                            30

#endif
