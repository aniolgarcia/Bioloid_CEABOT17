#ifndef _DYNAMIXEL_H
#define _DYNAMIXEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dynamixel_common.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define     DYN_HEADER_OFF      0
#define     DYN_ID_OFF          2
#define     DYN_LENGTH_OFF      3
#define     DYN_INST_OFF        4
#define     DYN_ERROR_OFF       4
#define     DYN_DATA_OFF        5

void dyn_copy_packet(uint8_t *source, uint8_t *destination);
uint8_t dyn_get_id(uint8_t *packet);
uint8_t dyn_get_length(uint8_t *packet);
TDynInstruction dyn_get_instruction(uint8_t *packet);
uint8_t dyn_check_checksum(uint8_t *packet);

// instruction packet
/*
typedef struct{
  uint8_t header[2];
  uint8_t id;
  uint8_t length;
  uint8_t instruction;
  uint8_t data[MAX_DATA_LENGTH];
  uint8_t checksum;
}TDynInst;
*/
/* ping instruction */
void dyn_init_ping_packet(uint8_t *packet,uint8_t id);
/* read instruction */
void dyn_init_read_packet(uint8_t *packet,uint8_t id,uint8_t address,uint8_t length);
uint8_t dyn_get_read_length(uint8_t *packet);
uint8_t dyn_get_read_address(uint8_t *packet);
/* write instruction */
void dyn_init_write_packet(uint8_t *packet,uint8_t id,uint8_t address,uint8_t length,uint8_t *data);
uint8_t dyn_get_write_address(uint8_t *packet);
uint8_t dyn_get_write_length(uint8_t *packet);
uint8_t dyn_get_write_data(uint8_t *packet,uint8_t *data);
/* registered write instruction */
void dyn_init_reg_write_packet(uint8_t *packet,uint8_t id,uint8_t address,uint8_t length,uint8_t *data);
uint8_t dyn_get_reg_write_address(uint8_t *packet);
uint8_t dyn_get_reg_write_length(uint8_t *packet);
uint8_t dyn_get_reg_write_data(uint8_t *packet,uint8_t *data);
/* action instruction */
void dyn_init_action_packet(uint8_t *packet);
/* reset instruction */
void dyn_init_reset_packet(uint8_t *packet,uint8_t id);
/* sync write instruction */
void dyn_init_sync_write_packet(uint8_t *packet,uint8_t num_servos,uint8_t *servo_ids,uint8_t address,uint8_t length,TWriteData *data);
uint8_t dyn_sync_write_id_present(uint8_t *packet,uint8_t id,uint8_t *address,uint8_t *length,uint8_t *data);
/* bulk read instruction */
void dyn_init_bulk_read_packet(uint8_t *packet,uint8_t num_servos,uint8_t *servo_ids,uint8_t *address,uint8_t *length);
uint8_t dyn_bulk_read_id_present(uint8_t *packet,uint8_t id,uint8_t *address,uint8_t *length);

// status packet
/*
typedef struct{
  uint8_t header[2];
  uint8_t id;
  uint8_t length;
  uint8_t error;
  uint8_t data[MAX_DATA_LENGTH];
  uint8_t checksum;
}TDynStatus;
*/

void dyn_init_status_packet(uint8_t *packet,uint8_t id,TDynError error,uint8_t length,uint8_t *data);
TDynError dyn_get_status_error(uint8_t *packet);
/* read instruction status packet */
uint8_t dyn_get_read_status_data(uint8_t *packet,uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif
