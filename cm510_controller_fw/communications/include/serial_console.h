#ifndef _SERIAL_CONSOLE_H
#define _SERIAL_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

void serial_console_init(uint32_t baudrate);
uint8_t serial_console_get_num_data(void);
int cm510_printf(const char *fmt, ...);
int cm510_scanf(const char *fmt, ... );
uint8_t cm510_read(uint8_t *data,uint8_t len);
uint8_t cm510_write(uint8_t *data,uint8_t len);

#ifdef __cplusplus
}
#endif

#endif
