#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "detectqrcode.h"
#include "rs232.h"
#include "exceptions.h"
#ifdef __REAL__
#include "ueyeinterface.h"
#endif

CRS232 serial_console("bioloid_serial_port");
TRS232_config serial_console_config;



void InitSerial()
{
#ifdef __REAL__
  std::string serial_console_device="/dev/ttyUSB0";
#else
  std::string serial_console_device="/tmp/ttyV1";
#endif
  try{
    serial_console_config.baud=57600;
    serial_console_config.num_bits=8;
    serial_console_config.parity=none;
    serial_console_config.stop_bits=1;
    serial_console.open((void *)&serial_console_device);
    serial_console.config(&serial_console_config);
  }catch(CException &e){
    std::cout << e.what() << std::endl;
    exit(-1);
  }
}

int WaitResponse(CRS232& serial, char* resp, size_t len, int sec=15)
{
  int read = 0;
  int tries = sec*1000000/20000;
  while ((read=serial.read((unsigned char*)resp,len))<=0 and tries>0)
  {
    usleep(20000);
    --tries;
  }
  resp[read] = '\0';
  //DEBUG(std::cerr << "Str response: " << resp;)
  //DEBUG(if (read==0 or resp[read-1]!='\n') std::cerr << '\n';)
  return read;
}

void Ignore(CRS232& serial, int ignore)
{
  unsigned char* buffer = new unsigned char[ignore];
  serial.read(buffer, ignore);
  delete[] buffer;
}



int Get(CRS232& serial, const char* arg)
{
  char buffer[80];
  std::string cmd = std::string("Get") + arg + "\n";
  //DEBUG(std::cerr << cmd << '\n';)
  while (WaitResponse(serial, buffer, 79, 2) == 0)
  {
//    DEBUG(std::cerr << "No response to " << cmd << ". Requesting again...\n";)
    sleep(1);
    serial.write((unsigned char*)cmd.c_str(), cmd.length());
  }
  int retval = atoi(buffer);
//  DEBUG(std::cerr << "Get" << arg << "->" << retval << '\n';)
  return atoi(buffer);
}

int DeltaOrientation(CRS232& serial, int goal)
{
  int current = Get(serial, "Compass");
  /* Compute *positive* increment angle int the range [0,3600) */
  int delta = ((goal-current)%3600+3600)%3600;
  /* Take delta to the range (-1800, 1800) */
  if (delta > 1800) delta -= 3600;
  return delta;
}



bool WaitReady(CRS232& serial)
{
  char buffer[16];
  while (serial.read((unsigned char*)buffer,16)<=0)
    usleep(100000);
  //DEBUG(std::cerr << "Str response: " << buffer << '\n';)
  return strncmp("ready",buffer,5)==0;
}


int main()
{
	while(1)
	{
		InitSerial();
		if(WaitReady(serial_console))
		{
			std::cout << "He llegit el serial del cm510" << std::endl;
		}
	}
}
