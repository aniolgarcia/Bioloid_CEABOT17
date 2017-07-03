#include "motion_pages.h"
#include <avr/pgmspace.h>

uint8_t pages_check_checksum(TActionPage *page)
{
  uint8_t checksum=0x00;
  uint16_t i=0;

  for(i=0;i<sizeof(TActionPage);i++)
    checksum+=((uint8_t *)page)[i];
  if(checksum==0x00)
    return 0x01;
  else
    return 0x00;
}

void pages_clear_page(TActionPage *page)
{
  uint16_t i=0;

  for(i=0;i<sizeof(TActionPage);i++)
    ((uint8_t *)page)[i]=0x00;
}

void pages_copy_page(TActionPage *src,TActionPage *dst)
{
  uint16_t i=0;

  for(i=0;i<sizeof(TActionPage);i++)
    ((uint8_t *)dst)[i]=((volatile uint8_t *)src)[i];
}

uint8_t load_page_info(uint8_t page_id,TActionPage *page) 
{
  uint32_t i=0;
  uint8_t *ppage = (uint8_t *)page;

  for(i=0;i<sizeof(TActionPage);i++) 
    ppage[i]= pgm_read_byte_far((uint32_t)PAGE_BASE_ADDR+((uint32_t)sizeof(TActionPage)*(uint32_t)page_id)+i);
  
  if(pages_check_checksum(page)==0) 
    return 0;
  else 
    return -1;
}


