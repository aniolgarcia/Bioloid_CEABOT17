/*
 *  Copyright (c) 2011 Humanoid Lab
 * 
 *  This file is part of iri-libbioloid. iri-libbioloid is free software: you
 *  can redistribute it and/or modify it under the terms of the GNU Lesser
 *  Public License as published by the Free Software Foundation, either 
 *  version 3 of the License, or (at your option) any later version.
 * 
 *  iri-libbioloid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser Public License
 *  along with iri-libbioloid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dyn_servos.h"
#include "dynamixel_master.h"
#include "serial_console.h"
#include <stdio.h>
#include <util/delay.h>

int main(void)
{
  unsigned char num;
  unsigned char ids[32];
  unsigned short int model,position;

  serial_console_init(57600);
  dyn_master_init();
  sei();

  dyn_master_scan(&num,ids);
  if(num>0)
  {
    model=get_model_number(ids[0]);
    cm510_printf("%d\n",model);
    set_target_speed(ids[0],100);
    for(;;)
    {
      set_target_position(ids[0],0);
      do{
        position=get_current_position(ids[0]);
      }while(position>24);
      set_target_position(ids[0],1023);
      do{
        position=get_current_position(ids[0]);
      }while(position<1000);
    }
  }
  else
    cm510_printf("No device found");

  while(1);
}
