#include <util/delay.h>
#include <stdio.h>
#include "cm510.h"
#include "balance.h"
#include "exp_board.h"
#include "mtn_library.h"
#include <stdlib.h>

//Programa de la banda del cm510 per a la prova de visió. La beaglebone/raspberrypi decodifica la camera i envia un número per serial segons el contingut del QR. El cas 0 de la màquina d'estats llegeix el serial (que mai hauria d'enviar 0 o valors negatiu) i els altres casos són cadascun dels moviments. Cal posar altres sistemes de seguretat, ja que ara mateix estem confiant plenament amb el bno055, i no es contemplen casos com per exemple que es vegin 2 QR, que no en trobi cap, que trobi el codi abans de l'esperat o que no el trobi...
//COm que hi havia problemes de precisió, hem fet un programa que emmagatzema els valors de la bruixola en un array.
// Cada posició de l'array correspon al valor que dona la brúixola al encarar el robot a un dels obstacles amb QR. No està completament acabat.

//El principi de funcionament era el següent:
// Si s'apreta botó avall quan la cm510 s'inicialita, entrarà en el mode per recollir dades. Encareu el robot cap a la 
// posició que voleu guardar i premeu el botó dreta. S'encenrdà una llumeta i, quan s'acabi, apunteu el robot cap al seguent
// objectiu i premeu botó dret altre cop. Aquest procés s'ha de repetir 8 vegades, una per a cada obstacle.



int position[8];
unsigned char state = 'a';
unsigned char num;
int valor_base;
int last_pos = 0;

int compass_param(int ini, int actual)
{
	short int inc = actual - ini;
	if(inc < -1800)
	{
	  inc += 3600;
	}
	else if(inc > 1800)
	{
	  inc -= 3600;
	}
	return inc;
}



int bno055_correction(int value)
{
	if(value > -240)
	{
	  return value - 240;
	}
	else
	{
	  return value;
	}
}

//Funció que suma angles deixant-los sempre en un rang de [0, 3600]
int suma_angles(int a, int b)
{
	int res = a + b;
	if(res < 0 )
	{
	  res += 3600;
	}
	else if(res > 3600)
	{
	  res -= 3600;
	}
	return res;
}

int compass(int valor_base)
{

	int nou_valor, desviament;

// 	nou_valor = exp_bno055_get_heading();
	nou_valor = bno055_correction(exp_bno055_get_heading());
	desviament = nou_valor - valor_base;
	if(desviament > 1800)
	{
		desviament = desviament - 3600;
	}
	else if(desviament < -1800)
	{
		desviament = desviament + 3600;
	}

	desviament = desviament/10;

	return desviament;
}

//La funció gira() és una còpia exacta de turn_angle() de la mtn_library, però utilitzant la bno055 en lloc del compass. Cal arreglar-ho a la llibreria de  moviments.

//S'ha canviat el comp_end per anar a una posició concreta de la brúixola, no per sumar un nombre de graus a la posició actual!

typedef enum {t_init,t_middle,t_left,t_right,t_wait_end} turn_states;
#define err 100 //Màxim error permès a turn.
uint8_t gira(int angle, int dir){
	static turn_states s = t_init;
	static int comp_ini = 0;
	static int comp_end = 0;
	int done = 0;

	switch (s){
		case t_init:
			comp_ini = bno055_correction(exp_bno055_get_heading());
//			comp_end = suma_angles (comp_ini,angle*10);
 			comp_end = angle;

			s=t_middle;
			break;
		case t_middle:
			if (abs (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end))>err){
			// ("diff = %d\n",compass_param (bno055_correction(exp_bno055_get_heading()),comp_end));
//				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)>err){
//					s =t_right;
//				}
//				else if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)<-err){
//					s=t_left;
//				}
				if(dir == -1)
				{
					s = t_left;					
				}
				if(dir == 1)
				{
					s = t_right;
				}
			
			}
			else {
				s = t_wait_end;
			}
			break;
		case t_right:
			if (turn_right()){
				s = t_wait_end;
			}
			else {
				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)<err){
						mtn_lib_stop_mtn();
				}
				else s = t_right;

			}
			break;

		case t_left:
			if (turn_left()){
				s = t_wait_end;
			}
			else {
				if (compass_param (bno055_correction(exp_bno055_get_heading()),comp_end)>-err){
						mtn_lib_stop_mtn();
				}
				else s = t_left;

			}
			break;
		case t_wait_end:
			done =0x01;
			s = t_init;
			break;

	}
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
  //cm510_printf("Init");

  if(is_button_pressed(BTN_DOWN)) //Entrem en mode de lectura
  {
	  int cont = 0;
	  while(cont < 8)
	  {
			toggle_led(LED_RxD);
			//cm510_printf("Wait reading");
		  if(is_button_rising_edge(BTN_RIGHT)) //Quan es premi el botó, fem lectura i la guardem
		  {
				//cm510_printf("Reading compass");
			  toggle_led(LED_AUX);
			  position[cont] = bno055_correction(exp_bno055_get_heading());
			  cont += 1;
				toggle_led(LED_AUX);
		  }
	  }
  }
  else //Si no s'ha entrat en mode lectura, es posen uns valors per defecte (que funcionaven per la posició concreta d'on féiem les proves, en qualsevol altre lloc són totalment erronis)
  {
	toggle_led(LED_TxD);
	  position[0] = 1184;
	  position[1] = 1591;
	  position[2] = -1636;
	  position[3] = -611;
	  position[4] = 105;
	  position[5] = 406;
	  position[6] = 758;
	  position[7] = 1134;
  }

}




void user_loop(void)
{
  //Hi havia un error al fer el cm510_read de la mateixa vairable sobre la qual s'executa el switch, per tant es fa lectura de "data" i al acabar el switch s'iguala "state" a "data". Dada es torna a declarar a cada iteració del user_loop per assegurar que no agafa qualsevol altre valor. Inicialment "state" = -2
  unsigned char data = 'a';
  switch(state)
  {
    case 'a':
		toggle_led(LED_AUX);
		if(is_button_rising_edge(BTN_START))
	    {
			action_set_page(31);
			action_start_page();
			data = 'b';
			valor_base = bno055_correction(exp_bno055_get_heading());
	    }
	    else
		{
			data = 'a';
		}
	    break;

    case 'b':
		if(is_action_running())
	    {
	        data = 'b';
	    }
	    else
	    {
			data = 'c';
	    }
	    break;

    case 'c':
		//cm510_printf("Llegint");
	    do{
			num=cm510_read(&data,1);
			_delay_ms(500);
	    }while(num == 0);
	    break;

    case '1': toggle_led(LED_RxD);
		if(gira(position[(last_pos-1)%8], -1) == 0x01)
	    {
			last_pos = (last_pos-1)%8;
			data = 'c';
	    }
	    else
	    {
			//cm510_printf("Desviament: %d\n",compass(valor_base));
			//cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			data = '1';
	    }
//	    //cm510_write(&data,1);
	    break;

    case '2':
		if(gira(position[(last_pos+1)%8], 1) == 0x01)
	    {
			last_pos = (last_pos+1)%8;
			data = 'c';
	    }
	    else
	    {
			//cm510_printf("Desviament: %d\n",compass(valor_base));
			//cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			data = '2';
	    }
//	    //cm510_write(&data,1);
	    break;

    case '3':
		if(gira(position[(last_pos-2)%8], -1) == 0x01)
	    {
			last_pos = (last_pos-2)%8;
			data = 'c';
	    }
	    else
	    {
			//cm510_printf("Desviament: %d\n",compass(valor_base));
			//cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			data = '3';
	    }
	    //cm510_write(&data,1);
	    break;

    case '4':
		if(gira(position[(last_pos+2)%8], 1) == 0x01)
	    {
			last_pos = (last_pos+2)%8;
			data = 'c';
	    }
	    else
	    {
			//cm510_printf("Desviament: %d\n",compass(valor_base));
			//cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			data = '4';
	    }
	    ////cm510_write(&data,1);
	    break;

    case '5':
		if(gira(position[(last_pos-3)%8], -1) == 0x01)
	    {
			last_pos = (last_pos-3)%8;
			data = 'c';
	    }
	    else
	    {
			//cm510_printf("Desviament: %d\n",compass(valor_base));
			//cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			data = '5';
	    }
	    ////cm510_write(&data,1);
	    break;

    case '6':
		if(gira(position[(last_pos+3)%8], 1) == 0x01)
	    {
			last_pos = (last_pos+3)%8;
			data = 'c';
	    }
	    else
		{
			//cm510_printf("Desviament: %d\n",compass(valor_base));
			//cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			data = '6';
	    }
	    ////cm510_write(&data,1);
	    break;

    case '7':
		if(gira(position[(last_pos-4)%8], -1) == 0x01)
	    {
			last_pos = (last_pos-4)%8;
			data = 'c';
	    }
	    else
	    {
			//cm510_printf("Desviament: %d\n",compass(valor_base));
			//cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			data = '7';
	    }
	    //cm510_write(&data,1);
	    break;

    case '8':
		if(gira(position[(last_pos+4)%8], 1) == 0x01)
	    {
			last_pos = (last_pos+4)%8;
			data = 'c';
	    }
	    else
	    {
			//cm510_printf("Desviament: %d\n",compass(valor_base));
			//cm510_printf("Rang corregit: %d  ", bno055_correction(exp_bno055_get_heading()));
			data = '8';
	    }
	    //cm510_write(&data,1);
	    break;
  }
	state = data;
}
