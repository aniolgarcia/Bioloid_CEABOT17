#ifndef _DYNAMIXEL_MASTER_H
#define _DYNAMIXEL_MASTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dynamixel.h"

/* public functions */
void dyn_master_init(void);
uint8_t dyn_master_is_init(void);
void dyn_master_set_rx_timeout(uint16_t timeout_ms);
void dyn_master_set_return_level(return_level_t level);
return_level_t dyn_master_get_return_level(void);
void dyn_master_set_baudrate(uint32_t baudrate);
uint32_t dyn_master_get_baudrate(void);
void dyn_master_scan(uint8_t *num,uint8_t *ids);
uint8_t dyn_master_ping(uint8_t id);
uint8_t dyn_master_read_byte(uint8_t id,uint16_t address,uint8_t *data);
uint8_t dyn_master_read_word(uint8_t id,uint16_t address,uint16_t *data);
uint8_t dyn_master_start_read_table(uint8_t id,uint16_t address,uint16_t length,uint8_t *data);
uint8_t dyn_master_wait_read_table(uint8_t id,uint16_t address,uint16_t length,uint8_t *data);
uint8_t dyn_master_read_table(uint8_t id,uint16_t address,uint16_t length,uint8_t *data);
uint8_t dyn_master_write_byte(uint8_t id, uint16_t address, uint8_t data);
uint8_t dyn_master_write_word(uint8_t id, uint16_t address, uint16_t data);
uint8_t dyn_master_write_table(uint8_t id, uint16_t address, uint16_t length, uint8_t *data);
uint8_t dyn_master_reg_write(uint8_t id, uint16_t address, uint16_t length, uint8_t *data);
uint8_t dyn_master_action(void);
uint8_t dyn_master_sync_write(uint8_t num,uint8_t *ids,uint16_t address, uint16_t length, TWriteData *data);
uint8_t dyn_master_bulk_read(uint8_t num,uint8_t *ids,uint16_t *address, uint16_t *length, TWriteData *data);

#ifdef __cplusplus
}
#endif

#endif
