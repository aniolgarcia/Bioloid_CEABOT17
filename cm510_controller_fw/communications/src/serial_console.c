#include "comm_cfg.h"
#include "serial_console.h"
#include <stdio.h>
#include <stdarg.h>

/* private variables */
volatile uint8_t serial_console_rx_buffer[SERIAL_CONSOLE_MAX_BUFFER_LEN];
volatile uint8_t serial_console_rx_read_ptr;
volatile uint8_t serial_console_rx_write_ptr;
volatile uint8_t serial_console_rx_num_data;
volatile uint8_t serial_console_tx_buffer[SERIAL_CONSOLE_MAX_BUFFER_LEN];
volatile uint8_t serial_console_tx_read_ptr;
volatile uint8_t serial_console_tx_write_ptr;
volatile uint8_t serial_console_tx_num_data;
volatile uint8_t serial_console_sending;
static FILE *device;

/* interrupt handlers */
SIGNAL(USART1_RX_vect)
{
  serial_console_rx_buffer[serial_console_rx_write_ptr]=UDR1;
  serial_console_rx_write_ptr=(serial_console_rx_write_ptr+1)%SERIAL_CONSOLE_MAX_BUFFER_LEN;
  if(serial_console_rx_num_data<SERIAL_CONSOLE_MAX_BUFFER_LEN)
    serial_console_rx_num_data++;
}

SIGNAL(USART1_TX_vect)
{
  if(serial_console_tx_num_data>0)
  {
    UDR1=serial_console_tx_buffer[serial_console_tx_read_ptr];
    serial_console_tx_read_ptr=(serial_console_tx_read_ptr+1)%SERIAL_CONSOLE_MAX_BUFFER_LEN;
    serial_console_tx_num_data--;
  }
  else
    serial_console_sending=0x00;
}

/* private functions */
void serial_console_set_baudrate(uint32_t baudrate)
{
  uint16_t divisor;

  divisor = (uint16_t)((uint32_t)2000000 / baudrate) - 1;
  UBRR1H = (uint8_t)((divisor & 0xFF00) >> 8);
  UBRR1L = (uint8_t)(divisor & 0x00FF);
}

int serial_console_putchar(char c,FILE *dev)
{
  if(c=='\n')
  {
    serial_console_tx_buffer[serial_console_tx_write_ptr]='\r';
    serial_console_tx_write_ptr=(serial_console_tx_write_ptr+1)%SERIAL_CONSOLE_MAX_BUFFER_LEN;
    serial_console_tx_num_data++;
  }
  serial_console_tx_buffer[serial_console_tx_write_ptr]=c;
  serial_console_tx_write_ptr=(serial_console_tx_write_ptr+1)%SERIAL_CONSOLE_MAX_BUFFER_LEN;
  serial_console_tx_num_data++;
  if(serial_console_sending==0x00)
  {
    UDR1=serial_console_tx_buffer[serial_console_tx_read_ptr];
    serial_console_tx_read_ptr=(serial_console_tx_read_ptr+1)%SERIAL_CONSOLE_MAX_BUFFER_LEN;
    serial_console_tx_num_data--;
    serial_console_sending=0x01;
  }

  return 0;
}

int serial_console_getchar(FILE *dev)
{
  int8_t rx;

  if(serial_console_rx_num_data>0)
  {
    rx=serial_console_rx_buffer[serial_console_rx_read_ptr];
    serial_console_rx_read_ptr=(serial_console_rx_read_ptr+1)%SERIAL_CONSOLE_MAX_BUFFER_LEN;
    serial_console_rx_num_data--;
    if(rx=='\r')
      rx='\n';
    return rx;
  }
  else
    return -1;
}

/* pubic functions */
void serial_console_init(uint32_t baudrate)
{
  // set UART register A
  //Bit 7: USART Receive Complete
  //Bit 6: USART Transmit Complete
  //Bit 5: USART Data Resigter Empty 
  //Bit 4: Frame Error
  //Bit 3: Data OverRun
  //Bit 2: Parity Error
  //Bit 1: Double The USART Transmission Speed
  //Bit 0: Multi-Processor Communication Mode
  UCSR1A = 0b01000010;

  // set UART register B
  // bit7: enable rx interrupt
  // bit6: enable tx interrupt
  // bit4: enable rx
  // bit3: enable tx
  // bit2: set sendding size(0 = 8bit)
  UCSR1B = 0b11011000;

  // set UART register C
  // bit6: communication mode (1 = synchronize, 0 = asynchronize)
  // bit5,bit4: parity bit(00 = no parity) 
  // bit3: stop bit(0 = stop bit 1, 1 = stop bit 2)
  // bit2,bit1: data size(11 = 8bit)
  UCSR1C = 0b00000110;

  // initialize
  serial_console_rx_read_ptr=0;
  serial_console_rx_write_ptr=0;
  serial_console_rx_num_data=0;
  serial_console_tx_read_ptr=0;
  serial_console_tx_write_ptr=0;
  serial_console_tx_num_data=0;
  serial_console_sending=0x00;

  // set baudrate
  serial_console_set_baudrate(baudrate);
  device=fdevopen(serial_console_putchar,serial_console_getchar);
}

uint8_t serial_console_get_num_data(void)
{  
  return serial_console_rx_num_data; 
}

int cm510_printf(const char *fmt, ...)
{
  va_list ap;
  int i;

  va_start(ap,fmt);
  i = vfprintf(stdout, fmt, ap);
  va_end(ap);
  return i;
}

int cm510_scanf(const char *fmt, ...)
{
  va_list arg;
  int done;
 
  va_start (arg, fmt);
  done = vfscanf (stdin, fmt, arg);
  va_end (arg);
  
  return done;
}

uint8_t cm510_read(uint8_t *data,uint8_t len)
{
  uint8_t i=0;

  for(i=0;i<len;i++)
  {
    if(serial_console_rx_num_data>0)
    {
      data[i]=serial_console_rx_buffer[serial_console_rx_read_ptr];
      serial_console_rx_read_ptr=(serial_console_rx_read_ptr+1)%SERIAL_CONSOLE_MAX_BUFFER_LEN;
      serial_console_rx_num_data--;
    }
    else
      break;
  }

  return i;
}

uint8_t cm510_write(uint8_t *data,uint8_t len)
{
  uint8_t i;

  for(i=0;i<len;i++)
  {
    if(serial_console_sending==0x00)
    {
      UDR1=data[i];
      serial_console_sending=0x01;
    }
    else
    {
      if(serial_console_tx_num_data<SERIAL_CONSOLE_MAX_BUFFER_LEN)
      {
        serial_console_tx_buffer[serial_console_tx_write_ptr]=data[i];
        serial_console_tx_write_ptr=(serial_console_tx_write_ptr+1)%SERIAL_CONSOLE_MAX_BUFFER_LEN;
        serial_console_tx_num_data++;
      }
      else 
        break;
    } 
  }

  return i;
}
