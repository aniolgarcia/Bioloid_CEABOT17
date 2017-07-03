#ifndef _MOTION_MANAGER_H
#define _MOTION_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

typedef enum {MM_NONE=0,MM_ACTION=1,MM_JOINTS=2} mm_module_t;

// servo information structure
typedef struct{
  uint16_t angle;
  uint16_t max_value;
  uint16_t min_value;
  uint16_t center_value;
  uint16_t current_value;
  uint16_t cw_limit;
  uint16_t ccw_limit;
  uint8_t slope;
  mm_module_t module;
}TServoInfo;

// public functions
uint8_t manager_init(uint8_t num_servos);

#ifdef __cplusplus
}
#endif

#endif
