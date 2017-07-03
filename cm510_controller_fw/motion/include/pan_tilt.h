#ifndef PAN_TILT_H
#define PAN_TILT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

// public functions
void pan_tilt_init(uint8_t pan_id,uint8_t tilt_id);
void pan_tilt_move_angles(int8_t pan_angle,uint8_t titl_angle);
void pan_move_angle(int8_t pan_angle);
void pan_set_speed(uint8_t pan_speed);
void pan_stop(void);
uint8_t pan_is_moving(void);
void tilt_move_angle(int8_t tilt_angle);
void tilt_set_speed(uint8_t tilt_speed);
void tilt_stop(void);
uint8_t tilt_is_moving(void);

void pan_tilt_process(void);

#ifdef __cplusplus
}
#endif

#endif
