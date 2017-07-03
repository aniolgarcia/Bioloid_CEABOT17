#ifndef _MOTION_PAGES_H
#define _MOTION_PAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#define PAGE_BASE_ADDR               (0x1E000UL)

#define MAX_PAGES                     256 
#define PAGE_MAX_NUM_SERVOS           31
#define POSE_NUMBER_OF_POSES_PER_PAGE 7

typedef struct // Header Structure (total 64unsigned char)
{
  uint8_t name[14];         // Name             0~13
  uint8_t reserved1;        // Reserved1        14
  uint8_t repeat;           // Repeat count     15
  uint8_t schedule;         // schedule         16
  uint8_t reserved2[3];     // reserved2        17~19
  uint8_t stepnum;          // Number of step   20
  uint8_t reserved3;        // reserved3        21
  uint8_t speed;            // Speed            22
  uint8_t reserved4;        // reserved4        23
  uint8_t accel;            // Acceleration time 24
  uint8_t next;             // Link to next     25
  uint8_t exit;             // Link to exit     26
  uint8_t reserved5[4];     // reserved5        27~30
  uint8_t checksum;         // checksum         31
  uint8_t slope[31];        // CW/CCW compliance slope  32~62
  uint8_t reserved6;        // reserved6        63
} TActionPageHeader;

typedef struct // Step Structure (total 64unsigned char)
{
  int16_t position[PAGE_MAX_NUM_SERVOS];    // Joint position   0~61
  uint8_t pause;            // Pause time       62
  uint8_t time;             // Time             63
} TActionStep;

typedef struct // Page Structure (total 512unsigned char)
{
  TActionPageHeader header;       // Page header  0~64
  TActionStep steps[POSE_NUMBER_OF_POSES_PER_PAGE];           // Page step    65~511
} TActionPage;

// public functions
uint8_t pages_check_checksum(TActionPage *page);
void pages_clear_page(TActionPage *page);
void pages_copy_page(TActionPage *src,TActionPage *dst);

uint8_t load_page_info(uint8_t page_id,TActionPage *page);

#ifdef __cplusplus
}
#endif

#endif
