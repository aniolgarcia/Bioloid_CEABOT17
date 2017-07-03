#include "dynamixel.h"

// private functions
void dyn_set_checksum(uint8_t *packet)
{
  uint8_t checksum=0x00;
  uint8_t i;

  checksum+=packet[DYN_ID_OFF];
  checksum+=packet[DYN_LENGTH_OFF];
  checksum+=packet[DYN_INST_OFF];
  for(i=0;i<packet[DYN_LENGTH_OFF]-2;i++)
    checksum+=packet[DYN_DATA_OFF+i];
  packet[DYN_DATA_OFF+i]=~checksum;
}

uint8_t dyn_check_checksum(uint8_t *packet)
{
  uint8_t checksum=0x00;
  uint8_t i;

  checksum+=packet[DYN_ID_OFF];
  checksum+=packet[DYN_LENGTH_OFF];
  checksum+=packet[DYN_INST_OFF];
  for(i=0;i<packet[DYN_LENGTH_OFF]-2;i++)
    checksum+=packet[DYN_DATA_OFF + i];
  checksum+=packet[DYN_DATA_OFF+i];

  return checksum;
}

// public functions
void dyn_copy_packet(uint8_t *source, uint8_t *destination)
{
  uint8_t i,length;

  length=source[DYN_LENGTH_OFF]+4;
  for(i=0;i<length;i++)
    destination[i]=source[i];
}

uint8_t dyn_get_id(uint8_t *packet)
{
  return packet[DYN_ID_OFF];
}

uint8_t dyn_get_length(uint8_t *packet)
{
  return packet[DYN_LENGTH_OFF];
}

TDynInstruction dyn_get_instruction(uint8_t *packet)
{
  return packet[DYN_INST_OFF];
}
/* ping instruction*/
void dyn_init_ping_packet(uint8_t *packet,uint8_t id)
{
  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=id;
  packet[DYN_LENGTH_OFF]=0x02;
  packet[DYN_INST_OFF]=DYN_PING;
  dyn_set_checksum(packet);
}
/* read instruction */
void dyn_init_read_packet(uint8_t *packet,uint8_t id,uint8_t address,uint8_t length)
{
  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=id;
  packet[DYN_LENGTH_OFF]=0x04;
  packet[DYN_INST_OFF]=DYN_READ;
  packet[DYN_DATA_OFF]=address;
  packet[DYN_DATA_OFF+1]=length;
  dyn_set_checksum(packet);
}

uint8_t dyn_get_read_length(uint8_t *packet)
{
  return packet[DYN_DATA_OFF+1];
}

uint8_t dyn_get_read_address(uint8_t *packet)
{
  return packet[DYN_DATA_OFF];
}

/* write instruction */
void dyn_init_write_packet(uint8_t *packet,uint8_t id,uint8_t address,uint8_t length,uint8_t *data)
{
  uint8_t i;

  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=id;
  packet[DYN_LENGTH_OFF]=0x03+length;
  packet[DYN_INST_OFF]=DYN_WRITE;
  packet[DYN_DATA_OFF]=address;
  for(i=0;i<length;i++)
    packet[DYN_DATA_OFF+i+1]=data[i];
  dyn_set_checksum(packet);
}

uint8_t dyn_get_write_address(uint8_t *packet)
{
  return packet[DYN_DATA_OFF];
}

uint8_t dyn_get_write_length(uint8_t *packet)
{
  return packet[DYN_LENGTH_OFF]-3;
}

uint8_t dyn_get_write_data(uint8_t *packet,uint8_t *data)
{
  uint8_t i;

  for(i=0;i<packet[DYN_LENGTH_OFF]-3;i++)
    data[i]=packet[DYN_DATA_OFF+i+1];

  return packet[DYN_LENGTH_OFF]-3;
}

/* registered write instruction */
void dyn_init_reg_write_packet(uint8_t *packet,uint8_t id,uint8_t address,uint8_t length,uint8_t *data)
{
  uint8_t i;

  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=id;
  packet[DYN_LENGTH_OFF]=0x03+length;
  packet[DYN_INST_OFF]=DYN_REG_WRITE;
  packet[DYN_DATA_OFF]=address;
  for(i=0;i<length;i++)
    packet[DYN_DATA_OFF+i+1]=data[i];
  dyn_set_checksum(packet);
}

uint8_t dyn_get_reg_write_address(uint8_t *packet)
{
  return packet[DYN_DATA_OFF];
}

uint8_t dyn_get_reg_write_length(uint8_t *packet)
{
  return packet[DYN_LENGTH_OFF]-3;
}

uint8_t dyn_get_reg_write_data(uint8_t *packet,uint8_t *data)
{
  uint8_t i;

  for(i=0;i<packet[DYN_LENGTH_OFF]-3;i++)
    data[i]=packet[DYN_DATA_OFF+i+1];

  return packet[DYN_LENGTH_OFF]-3;
}

/* action instruction */
void dyn_init_action_packet(uint8_t *packet)
{
  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=0xFE;
  packet[DYN_LENGTH_OFF]=0x02;
  packet[DYN_INST_OFF]=DYN_ACTION;
  dyn_set_checksum(packet);
}

/* reset instruction */
void dyn_init_reset_packet(uint8_t *packet,uint8_t id)
{
  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=id;
  packet[DYN_LENGTH_OFF]=0x02;
  packet[DYN_INST_OFF]=DYN_RESET;
  dyn_set_checksum(packet);
}

/* sync write instruction */
void dyn_init_sync_write_packet(uint8_t *packet,uint8_t num_servos,uint8_t *servo_ids,uint8_t address,uint8_t length,TWriteData *data)
{
  uint8_t i,j;

  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=0xFE;
  packet[DYN_LENGTH_OFF]=0x04+(length+1)*num_servos;
  packet[DYN_INST_OFF]=DYN_SYNC_WRITE;
  packet[DYN_DATA_OFF]=address;
  packet[DYN_DATA_OFF+1]=length;
  for(i=0;i<num_servos;i++)
  {
    packet[DYN_DATA_OFF+2+(length+1)*i]=servo_ids[i];
    for(j=0;j<length;j++)
      packet[DYN_DATA_OFF+2+(length+1)*i+j+1]=data[i].data_addr[j];
  }
  dyn_set_checksum(packet);
}

uint8_t dyn_sync_write_id_present(uint8_t *packet,uint8_t id,uint8_t *address,uint8_t *length,uint8_t *data)
{
  uint8_t num_servos,i,j;

  num_servos=(packet[DYN_LENGTH_OFF]-0x04)/(packet[DYN_DATA_OFF+1]+1);
  for(i=0;i<num_servos;i++)
    if(packet[DYN_DATA_OFF+2+(packet[DYN_DATA_OFF+1]+1)*i]==id)
    {
      (*address)=packet[DYN_DATA_OFF];
      (*length)=packet[DYN_DATA_OFF+1];
      for(j=0;j<packet[DYN_DATA_OFF+1];j++)
        data[j]=packet[DYN_DATA_OFF+2+(packet[DYN_DATA_OFF+1]+1)*i+j+1];
      return 0x01;
    }
  return 0x00;
}

/* bulk read instruction */
void dyn_init_bulk_read_packet(uint8_t *packet,uint8_t num_servos,uint8_t *servo_ids,uint8_t *address,uint8_t *length)
{
  uint8_t i;

  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=0xFE;
  packet[DYN_LENGTH_OFF]=0x03;
  packet[DYN_INST_OFF]=DYN_BULK_READ;
  packet[DYN_DATA_OFF]=0x00;
  for(i=0;i<num_servos;i++)
  {
    packet[DYN_DATA_OFF+1+i*3]=length[i];
    packet[DYN_DATA_OFF+1+i*3+1]=servo_ids[i];
    packet[DYN_DATA_OFF+1+i*3+2]=address[i];
    packet[DYN_LENGTH_OFF]+=0x03;
  }
  dyn_set_checksum(packet);
}

uint8_t dyn_bulk_read_id_present(uint8_t *packet,uint8_t id,uint8_t *address,uint8_t *length)
{
  uint8_t num_servos,i,prev_id=0x00;

  num_servos=(packet[DYN_LENGTH_OFF]-0x03)/3;
  for(i=0;i<num_servos;i++)
  {
    if(packet[DYN_DATA_OFF+1+i*3+1]==id)
    {
      (*address)=packet[DYN_DATA_OFF+1+i*3+2];
      (*length)=packet[DYN_DATA_OFF+1+i*3];
      return prev_id;
    }
    else
      prev_id=packet[DYN_DATA_OFF+1+i*3+1];
  }
  return 0xFF;
}

void dyn_init_status_packet(uint8_t *packet,uint8_t id,TDynError error,uint8_t length,uint8_t *data)
{
  uint8_t i;

  packet[DYN_HEADER_OFF]=0xFF;
  packet[DYN_HEADER_OFF+1]=0xFF;
  packet[DYN_ID_OFF]=id;
  packet[DYN_LENGTH_OFF]=0x02+length;
  packet[DYN_ERROR_OFF]=error;
  for(i=0;i<length;i++)
    packet[DYN_DATA_OFF+i]=data[i];
  dyn_set_checksum(packet); 
}

TDynError dyn_get_status_error(uint8_t *packet)
{
  return packet[DYN_ERROR_OFF];
}

/* read instruction status packet */
uint8_t dyn_get_read_status_data(uint8_t *packet,uint8_t *data)
{
  uint8_t i;

  for(i=0;i<packet[DYN_LENGTH_OFF]-0x02;i++)
    data[i]=packet[DYN_DATA_OFF+i];

  return packet[DYN_LENGTH_OFF]-0x02;
}
