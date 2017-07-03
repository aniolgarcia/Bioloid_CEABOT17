#ifndef _USER_TIME_H
#define _USER_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

/* public functions */
void init_user_time(void);
void user_time_set_period(uint16_t period_ms);
void user_time_set_one_time(uint16_t time_ms);
uint8_t user_time_is_period_done(void);
uint8_t user_time_is_done(void);
void user_time_stop(void);

#ifdef __cplusplus
}
#endif

#endif
