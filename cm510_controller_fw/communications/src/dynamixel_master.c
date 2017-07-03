#include "cm510_cfg.h"
#include "comm_cfg.h"
#include "dynamixel_master.h"

/* private variables */
uint8_t dyn_master_tx_buffer[DYN_MASTER_MAX_TX_BUFFER_LEN];
uint8_t dyn_master_sent_bytes;
volatile uint8_t dyn_master_sent_done;
uint8_t dyn_master_rx_buffer[DYN_MASTER_MAX_RX_BUFFER_LEN];
uint8_t dyn_master_received_bytes;
volatile uint8_t dyn_master_packet_ready;
return_level_t dyn_master_return_level;
uint8_t dyn_master_rx_no_answer;
uint8_t dyn_master_rx_num_packets;
uint32_t dyn_master_baudrate;
uint8_t dyn_master_initialized=0x00;
/* private timeout variables */
uint16_t dyn_master_rx_timeout_us;

/* private timeout functions */
void dyn_master_start_timeout(void)
{
  uint32_t count;

  count=(((dyn_master_rx_timeout_us*(F_CPU/1000000))/1024));

  TCNT0=255-count;
  /* enable timer */
  TIFR0|=(1<<TOV0);// clear interrutp flag
  TCCR0B=(1<<CS02)|(1<<CS00);// set prescaler to 1024
}

void dyn_master_cancel_timeout(void)
{
  /* disable timer */
  TCCR0B=0x00;
  TIFR0|=(1<<TOV0);// clear interrutp flag
  TCNT0=0x00; 
}

uint8_t dyn_master_timeout(void)
{
  if(TIFR0&(1<<TOV0))
    return 0x01;
  else
    return 0x00;
}

/* private functions */
void dyn_master_set_tx_mode(void)
{
  PORTE&=~0x08;
  PORTE|=0x04;
}

void dyn_master_set_rx_mode(void)
{
  PORTE&=~0x04;
  PORTE|=0x08;
}

/* interrupt handlers */
ISR(USART0_TX_vect)
{
  cli();
  if(dyn_master_sent_bytes==dyn_master_tx_buffer[3]+4)
  {
    dyn_master_set_rx_mode();
    dyn_master_sent_done=1;
  }
  else
  {
    UDR0=dyn_master_tx_buffer[dyn_master_sent_bytes];
    dyn_master_sent_bytes++;
  }
  sei();
}

ISR(USART0_RX_vect)
{
  static uint8_t length;

  cli();// disable any other interrupt
  dyn_master_start_timeout();// each time a byte is received, restart the timeout
  dyn_master_rx_buffer[dyn_master_received_bytes]=UDR0;
  switch(dyn_master_received_bytes)
  {
    case 0: if(dyn_master_rx_buffer[dyn_master_received_bytes]==0xFF)
              dyn_master_received_bytes++;
            break;
    case 1: if(dyn_master_rx_buffer[dyn_master_received_bytes]==0xFF)
              dyn_master_received_bytes++;
            else
              dyn_master_received_bytes--;
            break;
    case 2: dyn_master_received_bytes++;
            break;
    case 3: length=dyn_master_rx_buffer[dyn_master_received_bytes]+3;
            dyn_master_received_bytes++;
            break;
    default: if(dyn_master_received_bytes==length)
             {
               dyn_master_received_bytes=0;
               dyn_master_packet_ready=1;
               dyn_master_cancel_timeout();// cancel timeout when reception is complete
             }
             else
               dyn_master_received_bytes++;
             break;
  }
  sei();// enable all interrutps
}

uint8_t dyn_master_wait_transmission(void)
{
  dyn_master_start_timeout();
  while(dyn_master_sent_done==0)
  {
    if(dyn_master_timeout())
    {
      dyn_master_cancel_timeout();
      dyn_master_sent_done=0x01;
      return DYN_TIMEOUT;
    }
  }
  dyn_master_cancel_timeout();
  
  return DYN_SUCCESS;
}

uint8_t dyn_master_send(void)
{
  // set the transfer
  dyn_master_sent_bytes=1;
  UDR0=dyn_master_tx_buffer[0];

  return DYN_SUCCESS;
} 

uint8_t dyn_master_wait_reception(void)
{
  // set timeout 
  // wait for the status packet
  if(dyn_master_rx_no_answer)
  {
    dyn_master_rx_no_answer=0x00;
    return DYN_SUCCESS;
  }
  else
  {
    dyn_master_start_timeout();
    while(dyn_master_packet_ready==0x00)
    {
      if(dyn_master_timeout())
      {
        dyn_master_cancel_timeout();
        return DYN_TIMEOUT;
      }
    }
    dyn_master_rx_num_packets--;
    if(dyn_master_rx_num_packets==0x00)
      dyn_master_cancel_timeout();
    dyn_master_packet_ready=0x00;
    // check the input packet checksum
    if(dyn_check_checksum(dyn_master_rx_buffer)==0xFF)
      return dyn_get_status_error(dyn_master_rx_buffer);
    else
      return DYN_CHECKSUM_ERROR;
  }
}

/* public functions */
void dyn_master_init(void)
{
  /* initialize internal variables */
  dyn_master_sent_done=0x01;
  dyn_master_packet_ready=0x00;
  dyn_master_sent_bytes=0x00;
  dyn_master_received_bytes=0x00;
  dyn_master_rx_no_answer=0x00;
  dyn_master_rx_num_packets=0x00;
  dyn_master_return_level=return_all;

  /* configure the IO ports */
  DDRE|=0x0E;
  DDRE&=0xFE;

  DDRC=0x7F;
  PORTC=0x7F;

  /* initialize the serial port */
  // set UART register A
  //Bit 7: USART Receive Complete
  //Bit 6: USART Transmit Complete
  //Bit 5: USART Data Resigter Empty 
  //Bit 4: Frame Error
  //Bit 3: Data OverRun
  //Bit 2: Parity Error
  //Bit 1: Double The USART Transmission Speed
  //Bit 0: Multi-Processor Communication Mode
  UCSR0A = 0b01000010;

  // set UART register B
  // bit7: enable rx interrupt
  // bit6: enable tx interrupt
  // bit4: enable rx
  // bit3: enable tx
  // bit2: set sendding size(0 = 8bit)
  UCSR0B = 0b11011000;

  // set UART register C
  // bit6: communication mode (1 = synchronize, 0 = asynchronize)
  // bit5,bit4: parity bit(00 = no parity) 
  // bit3: stop bit(0 = stop bit 1, 1 = stop bit 2)
  // bit2,bit1: data size(11 = 8bit)
  UCSR0C = 0b00000110;
  dyn_master_set_baudrate((uint32_t)DYN_MASTER_DEFAULT_BAUDRATE);

  /* initialize the timeout timer */
  dyn_master_rx_timeout_us=DYN_MASTER_DEFAULT_TIMEOUT_US;
  TCCR0A=0x00;// normal mode, output disabled on both channels
  TCCR0B=0x00;// normal mode, prescaler 0 (timer disabled)
  TIMSK0=0x00;// all interrupts disabled
  TCNT0=0x00;

  dyn_master_set_rx_mode();
  dyn_master_initialized=0x01;
}

uint8_t dyn_master_is_init(void)
{
  return dyn_master_initialized;
}

void dyn_master_set_rx_timeout(uint16_t timeout_us)
{
  if(timeout_us>16000)
    dyn_master_rx_timeout_us=16000;
  else
    dyn_master_rx_timeout_us=timeout_us;
}

void dyn_master_set_return_level(return_level_t level)
{
  dyn_master_return_level=level;
}

return_level_t dyn_master_get_return_level(void)
{
  return dyn_master_return_level;
}

void dyn_master_set_baudrate(uint32_t baudrate)
{
  uint16_t divisor;

  divisor = (uint16_t)((uint32_t)2000000 / baudrate) - 1;
  UBRR0H = (uint8_t)((divisor & 0xFF00) >> 8);
  UBRR0L = (uint8_t)(divisor & 0x00FF);
}

uint32_t dyn_master_get_baudrate(void)
{
  return dyn_master_baudrate;
}

void dyn_master_scan(uint8_t *num,uint8_t *ids)
{
  uint8_t i;

  *num=0;
  for(i=0;i<254;i++)
  {
    if(dyn_master_ping(i)==DYN_SUCCESS)// the device exists
    {
      ids[*num]=i;
      (*num)++;
    }
  }
}

uint8_t dyn_master_ping(uint8_t id)
{
  uint8_t error;

  // wait until any previous transmissions ends
  if(dyn_master_wait_transmission()==DYN_TIMEOUT)
    return DYN_TIMEOUT;
  // generate the ping packet for the desired device
  dyn_init_ping_packet(dyn_master_tx_buffer,id);
  dyn_master_rx_num_packets=0x01;
  dyn_master_rx_no_answer=0x00;
  // enable transmission
  dyn_master_set_tx_mode();
  // send the data
  if((error=dyn_master_send())!=DYN_SUCCESS)
  {
    dyn_master_set_rx_mode();
    return error;
  }
  // wait for the replay within the given timeout
  error=dyn_master_wait_reception();

  return error;
}

uint8_t dyn_master_read_byte(uint8_t id,uint16_t address,uint8_t *data)
{
  return dyn_master_read_table(id,address,1,data);
}

uint8_t dyn_master_read_word(uint8_t id,uint16_t address,uint16_t *data)
{
  uint8_t error;
  uint8_t data_int[2];

  // generate the ping packet for the desired device
  error=dyn_master_read_table(id,address,2,data_int);
  (*data)=data_int[0]+data_int[1]*256;

  return error;
}


uint8_t dyn_master_start_read_table(uint8_t id,uint16_t address,uint16_t length,uint8_t *data)
{
  uint8_t error;

  // wait until any previous transmissions ends
  if(dyn_master_wait_transmission()==DYN_TIMEOUT)
    return DYN_TIMEOUT;
  // generate the read packet for the desired device
  dyn_init_read_packet(dyn_master_tx_buffer,id,address,length);
  dyn_master_rx_num_packets=0x01;
  if(dyn_master_return_level==no_return || id==DYN_BROADCAST_ID)
    dyn_master_rx_no_answer=0x01;
  else
    dyn_master_rx_no_answer=0x00;
  // enable transmission
  dyn_master_set_tx_mode();
  // send the data
  if((error=dyn_master_send())!=DYN_SUCCESS)
  {
    dyn_master_set_rx_mode();
    return error;
  }
  return error;
}

uint8_t dyn_master_wait_read_table(uint8_t id,uint16_t address,uint16_t length,uint8_t *data)
{
  uint8_t error=DYN_SUCCESS;

  // wait for the replay within the given timeout
  if(dyn_master_return_level!=no_return && id!=DYN_BROADCAST_ID)
  {
    if((error=dyn_master_wait_reception())==DYN_SUCCESS)
    {
      if(dyn_get_read_status_data(dyn_master_rx_buffer,data)!=length)// not enough data
        error=DYN_INST_ERROR;
    }
  }

  return error;
}

uint8_t dyn_master_read_table(uint8_t id,uint16_t address,uint16_t length,uint8_t *data)
{
  uint8_t error;

  // wait until any previous transmissions ends
  if(dyn_master_wait_transmission()==DYN_TIMEOUT)
    return DYN_TIMEOUT;
  // generate the read packet for the desired device
  dyn_init_read_packet(dyn_master_tx_buffer,id,address,length);
  dyn_master_rx_num_packets=0x01;
  if(dyn_master_return_level==no_return || id==DYN_BROADCAST_ID)
    dyn_master_rx_no_answer=0x01;
  else
    dyn_master_rx_no_answer=0x00;
  // enable transmission
  dyn_master_set_tx_mode();
  // send the data
  if((error=dyn_master_send())!=DYN_SUCCESS)
  {
    dyn_master_set_rx_mode();
    return error;
  }
  // wait for the replay within the given timeout
  if(dyn_master_return_level!=no_return && id!=DYN_BROADCAST_ID)
  {
    if((error=dyn_master_wait_reception())==DYN_SUCCESS)
    {
      if(dyn_get_read_status_data(dyn_master_rx_buffer,data)!=length)// not enough data
        error=DYN_INST_ERROR;
    }
  } 

  return error;
}

uint8_t dyn_master_write_byte(uint8_t id, uint16_t address, uint8_t data)
{
  return dyn_master_write_table(id,address,1,&data);
}

uint8_t dyn_master_write_word(uint8_t id, uint16_t address, uint16_t data)
{
  uint8_t data_int[2];

  data_int[0]=data%256;
  data_int[1]=data/256;
  return dyn_master_write_table(id,address,2,data_int);
}

uint8_t dyn_master_write_table(uint8_t id, uint16_t address, uint16_t length, uint8_t *data)
{
  uint8_t error;

  // wait until any previous transmissions ends
  if(dyn_master_wait_transmission()==DYN_TIMEOUT)
    return DYN_TIMEOUT;
  // generate the write packet for the desired device
  dyn_init_write_packet(dyn_master_tx_buffer,id,address,length,data);
  dyn_master_rx_num_packets=0x01;
  if(dyn_master_return_level==return_all && id!=DYN_BROADCAST_ID)
    dyn_master_rx_no_answer=0x00;
  else
    dyn_master_rx_no_answer=0x01;
  // enable transmission
  dyn_master_set_tx_mode();
  // send the data
  if((error=dyn_master_send())!=DYN_SUCCESS)
  {
    dyn_master_set_rx_mode();
    return error;
  }
  // wait for the replay within the given timeout
  if(dyn_master_return_level==return_all && id!=DYN_BROADCAST_ID)
    error=dyn_master_wait_reception();

  return error;
}

uint8_t dyn_master_reg_write(uint8_t id, uint16_t address, uint16_t length, uint8_t *data)
{
  uint8_t error;

  // wait until any previous transmissions ends
  if(dyn_master_wait_transmission()==DYN_TIMEOUT)
    return DYN_TIMEOUT;
  // generate the registered write packet for the desired device
  dyn_init_reg_write_packet(dyn_master_tx_buffer,id,address,length,data);
  dyn_master_rx_num_packets=0x01;
  if(dyn_master_return_level==return_all && id!=DYN_BROADCAST_ID)
    dyn_master_rx_no_answer=0x00;
  else
    dyn_master_rx_no_answer=0x01;
  // enable transmission
  dyn_master_set_tx_mode();
  // send the data
  if((error=dyn_master_send())!=DYN_SUCCESS)
  {
    dyn_master_set_rx_mode();
    return error;
  }
  // wait for the replay within the given timeout
  if(dyn_master_return_level==return_all && id!=DYN_BROADCAST_ID)
    error=dyn_master_wait_reception();

  return error;
}

uint8_t dyn_master_action(void)
{
  uint8_t error;

  // wait until any previous transmissions ends
  if(dyn_master_wait_transmission()==DYN_TIMEOUT)
    return DYN_TIMEOUT;
  // generate the action packet for the desired device
  dyn_init_action_packet(dyn_master_tx_buffer);
  dyn_master_rx_num_packets=0x01;
  dyn_master_rx_no_answer=0x01;
  // enable transmission
  dyn_master_set_tx_mode();
  // send the data
  if((error=dyn_master_send())!=DYN_SUCCESS)
  {
    dyn_master_set_rx_mode();
    return error;
  }

  return error;
}

uint8_t dyn_master_sync_write(uint8_t num,uint8_t *ids,uint16_t address, uint16_t length, TWriteData *data)
{
  uint8_t error;

  // wait until any previous transmissions ends
  if(dyn_master_wait_transmission()==DYN_TIMEOUT)
    return DYN_TIMEOUT;
  // generate the write packet for the desired device
  dyn_init_sync_write_packet(dyn_master_tx_buffer,num,ids,address,length,data);
  dyn_master_rx_num_packets=0x01;
  dyn_master_rx_no_answer=0x01;
  // enable transmission
  dyn_master_set_tx_mode();
  // send the data
  if((error=dyn_master_send())!=DYN_SUCCESS)
  {
    dyn_master_set_rx_mode();
    return error;
  }

  return error;
}

uint8_t dyn_master_bulk_read(uint8_t num,uint8_t *ids,uint16_t *address, uint16_t *length, TWriteData *data)
{
  uint8_t error,i,ver1_address[255],ver1_length[255];

  // wait until any previous transmissions ends
  if(dyn_master_wait_transmission()==DYN_TIMEOUT)
    return DYN_TIMEOUT;
  // generate the read packet for the desired device
  for(i=0;i<num;i++)
  {
    ver1_address[i]=address[i];
    ver1_length[i]=length[i];
  }
  dyn_init_bulk_read_packet(dyn_master_tx_buffer,num,ids,ver1_address,ver1_length);
  dyn_master_rx_num_packets=num;
  if(dyn_master_return_level==no_return)
    dyn_master_rx_no_answer=0x01;
  else
    dyn_master_rx_no_answer=0x00;
  // enable transmission
  dyn_master_set_tx_mode();
  // send the data
  if((error=dyn_master_send())!=DYN_SUCCESS)
  {
    dyn_master_set_rx_mode();
    return error;
  }
  // wait for the replay within the given timeout
  if(dyn_master_return_level!=no_return)
  {
    for(i=0;i<num;i++)
    {
      if((error=dyn_master_wait_reception())==DYN_SUCCESS)
      {
        if(dyn_get_read_status_data(dyn_master_rx_buffer,data[i].data_addr)!=length[i])// not enough data
          error=DYN_INST_ERROR;
      }
    }
  }

  return error;
}
