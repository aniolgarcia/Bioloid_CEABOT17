#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "detectqrcode.h"
#include "rs232.h"
#include "exceptions.h"

using namespace cv;
using namespace std;

int main() 
{
  cv::Mat gray;
  cv::Mat frame;
  CDetectqrcode detector;
  std::vector<std::vector<cv::Point> > squares;
  CRS232 serial_console("bioloid_serial_port");
  std::string serial_console_device="/dev/pts/27";
  TRS232_config serial_console_config;
  std::string cmd;

  cv::VideoCapture cap(0);   //0 is the id of video device.0 if you have only one camera.

  if (!cap.isOpened()) 
  { //check if video device has been initialised
    cout << "cannot open camera";
    return 0;
  }

  detector.init(721.977446f,717.128980f,334.800583f,242.691968f,0.0475f,0.0475f);

  try{
    serial_console_config.baud=57600;
    serial_console_config.num_bits=8;
    serial_console_config.parity=none;
    serial_console_config.stop_bits=1;
    serial_console.open((void *)&serial_console_device);
    serial_console.config(&serial_console_config);
  }catch(CException &e){
    std::cout << e.what() << std::endl;
    return 0;
  }

  cv::namedWindow("QRcode",1);
  //unconditional loop

  for(unsigned int i=0;i<2;i++) 
  { 
    std::cout << "move left" << std::endl;
    cmd="l\n";
    serial_console.write((unsigned char *)cmd.c_str(),2);
    sleep(2);
    std::cout << "move right" << std::endl;
    cmd="r\n";
    serial_console.write((unsigned char *)cmd.c_str(),2);
    sleep(2);
  }

  while (true) 
  {
    cap.read(frame);
    std::vector<TQRInfo> tags;
    cv::cvtColor(frame,gray,CV_BGR2GRAY);
    detector.findSquares(gray, squares);
    detector.findQR(gray,tags);
    for(unsigned int i=0;i<tags.size();i++)
    {
      std::cout << "Tag ID: " << tags[i].tag_id << std::endl;
      std::cout << "Tag position: X:" << tags[i].tvec[0] << " Y: " << tags[i].tvec[1] << " Z: " << tags[i].tvec[2] << std::endl;
    }
    detector.drawSquares(frame, squares);
    cv::imshow("QRcode",frame);
    if (waitKey(30) >= 0)
      break;
  }

  return 0;
}
