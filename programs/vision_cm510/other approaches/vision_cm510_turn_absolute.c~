#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Funció per calcular el desviament del robot respecte l'orientació inicial. És com compass però amb la posició inicial parametritzada.
int compass_param(int ini, int actual)
{
	short int inc = actual - ini;
	if(inc<-1800)
	{
	  	inc+=3600;
	}
	else if(inc>1800)
	{
	  	inc-=3600;
	}
	return inc;
}


//Com que el rang és de [-1800, -250(aprox)] U [0, 2050(aprox)], quan passi de -250, simplement li restem 250.
int bno055_correction(int value)
{
	if(value > -250)
	{
	  return value -250;
	}
	else
	{
	  return value;
	}
}

int add_angles(int angle1, int angle2)
{
	int addition = angle1 + angle2;
	if(addition < -1800)
	{
		return addition + 3600;
	}
	else if(addition > 180)
	{
		return addition -3600;
	}
	else
	{
		return addition;
	}
}

#define err 100
typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;
uint8_t turn_absolute(int angle, int direction)
{
	
	return done;
}


void user_init(void)
{
  serial_console_init(9600);
  balance_init();
  balance_calibrate_gyro();
//  balance_enable_gyro(); // En aquest cas hem de tenir el gyro desactivat, ja que si no intenta compensar els propis moviments de gir
  user_time_set_period(100);
  mtn_lib_init();
  exp_adc_start();
  exp_bno055_start();
  if(is_button_pressed(BTN_UP))
  {
    exp_bno055_erase_calibration();
  }
  while(exp_bno055_is_calibrated()!=0x01)
  {
    _delay_ms(100);
  }

}


int num, target_position, turn_direction, actual_position;
unsigned char data;
int position[8];
position[0] = -1465;
position[1] = -1056;
position[2] = -602;
position[3] = -215;
position[4] = 304;
position[5] = 912;
position[6] = 1195;
position[7] = 1634;
int last_position = 0;
int next_position;
int direction;



typedef enum {wait_start, wait_ready, wait_5, read_QR, get_target_position, turn} main_states; 
main_states state = wait_start

void user_loop(void)
{

  	switch(state)
  	{
  	case wait_start:
  		if(is_button_rising_edge(BTN_START))
        {
            valor_base = bno055_correction(exp_bno055_get_heading());
            action_set_page(31);
            action_start_page();
			user_time_set_period(5000);
            state=wait_5;
        }
        else
        {
            state=wait_start;
        }
        break;


    case wait_5: 	 
    	if(user_time_is_period_done())
        {
            state = wait_ready;
        }
        else
        {
            state = wait_5;
        }
        break;

    case wait_ready: 
    	if(is_action_running())
        {
            state = wait_ready;
        }
        else
        {
        	state = read_QR;
        }
        break;

    case read_QR:
		cm510_printf("ready\n");
		_delay_ms(500);
    	num=cm510_read(&data,1);
		_delay_ms(500);

		if(num == 0)
		{
			state = read_QR;
		}
		else
		{
			state = get_target_position;
		}
		break;

	case get_target_position:
		switch(data)
		{
		case '1':
			next_position = (last_position+(-45/45))%8;
			target_position = positions[next_position];
			direction = -1;
			break;
		case '2':
			next_position = (last_position+(45/45))%8;
			target_position = positions[next_position];
			direction = 1;
			break;
		case '3':
			next_position = (last_position+(-90/45))%8;
			target_position = positions[next_position];
			direction = -1;
			break;
		case '4':
			next_position = (last_position+(90/45))%8;
			target_position = positions[next_position];
			direction = 1;
			break;
		case '5':
			next_position = (last_position+(-135/45))%8;
			target_position = positions[next_position];
			direction = -1;
			break;
		case '6':
			next_position = (last_position+(135/45))%8;
			target_position = positions[next_position];
			direction = 1;
			break;
		case '7':
			next_position = (last_position+(-180/45))%8;
			target_position = positions[next_position];
			direction = -1;
			break;
		case '8':
			next_position = (last_position+(180/45))%8;
			target_position = positions[next_position];
			direction = 1;
			break;
		}
		state = turn;
		break;

	case turn:
		actual_position = bno055_correction(exp_bno055_get_heading());
		if(actual_position <= add_angles(target_position, err) && actual_position >= add_angles(target_position, -err))
		{
			mtn_lib_stop_mtn;
		}

		if(direction == -1)
		{
			if(turn_left() == 0x01)
			{
				state = read_QR;
			}
			else
			{
				state = turn; 
			}
		}
		else if(diretion == 1)
		{
			if(turn_right() == 0x01)
			{
				state = read_QR;
			}
			else
			{
				state = turn;
			}
		}
		break;
  	}
}
