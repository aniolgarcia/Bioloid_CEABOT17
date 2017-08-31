//grab.c : initializes the uEye camera and grabs a predetermined number of images, measuring the time it takes to grab and store them in the hard disk drive (SD)
//The image size is previously defined
//Compile with : gcc grab.c -o grab -lueye_api 
#include "exceptions.h"
#include <stdio.h>
#include <stdlib.h>
#include <ueye.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <zbar.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "rs232.h"


std::string serial_dev="/dev/ttyUSB0";

#define NUM_IMGS  	1
#define SIZE_X		640
#define SIZE_Y		480

//activate/deactivate parts of the code
#define DEBUG true
#define SAVE false //saves the images to disk
#define SHOW true 
#define SERIAL true

using namespace std;
using namespace cv;
using namespace zbar;


int
findQR (Mat & grey)
{
  ImageScanner scanner;
  scanner.set_config (ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
  int width = grey.cols;
  int height = grey.rows;
  uchar *raw = (uchar *) grey.data;
  // wrap image data
  Image image (width, height, "Y800", raw, width * height);
  // scan the image for barcodes
  int n = scanner.scan (image);
  // extract results. The iterator is not really needed as we will use only the first one
  for (Image::SymbolIterator symbol = image.symbol_begin ();
       symbol != image.symbol_end (); ++symbol)
    {
      // do something useful with results
      if (DEBUG) 
        {
          vector < Point > vp;
          cout << "decoded " << symbol->get_type_name ();
          cout << " symbol \"" << symbol->get_data () << '"' << " " << endl;
          int n = symbol->get_location_size ();
          for (int i = 0; i < n; i++)
	        {
	          vp.push_back (Point (symbol->get_location_x (i), symbol->get_location_y (i)));
	          cout << "location " << i << "X: " << symbol->get_location_x (i);
            cout << " Y: " << symbol->get_location_y (i) << endl;
	        }
        }
        //if multiple codes we only return the first one
        std::string str1("Turn45L");
        std::string str2("Turn45R");
        std::string str3("Turn90L");
        std::string str4("Turn90R");
        std::string str5("Turn135L");
        std::string str6("Turn135R");
        std::string str7("Turn180L");
        std::string str8("Turn180R");
        if (str1.compare(symbol->get_data()) == 0) return 1;
        if (str2.compare(symbol->get_data()) == 0) return 2;
        if (str3.compare(symbol->get_data()) == 0) return 3;
        if (str4.compare(symbol->get_data()) == 0) return 4;
        if (str5.compare(symbol->get_data()) == 0) return 5;
        if (str6.compare(symbol->get_data()) == 0) return 6;
        if (str7.compare(symbol->get_data()) == 0) return 7;
        if (str8.compare(symbol->get_data()) == 0) return 8;

//         std::string str10("N1");
//         std::string str11("N2");
//         std::string str12("N3");
//         std::string str13("N4");
//         std::string str14("N5");
//         std::string str15("N6");
//         if (str10.compare(symbol->get_data()) == 0) return 10;
//         if (str11.compare(symbol->get_data()) == 0) return 11;
//         if (str12.compare(symbol->get_data()) == 0) return 12;
//         if (str13.compare(symbol->get_data()) == 0) return 13;
//         if (str14.compare(symbol->get_data()) == 0) return 14;
//         if (str15.compare(symbol->get_data()) == 0) return 15;
// 
//         std::string str20("S1");
//         std::string str21("S2");
//         std::string str22("S3");
//         std::string str23("S4");
//         std::string str24("S5");
//         std::string str25("S6");
//         if (str20.compare(symbol->get_data()) == 0) return 20;
//         if (str21.compare(symbol->get_data()) == 0) return 21;
//         if (str22.compare(symbol->get_data()) == 0) return 22;
//         if (str23.compare(symbol->get_data()) == 0) return 23;
//         if (str24.compare(symbol->get_data()) == 0) return 24;
//         if (str25.compare(symbol->get_data()) == 0) return 25;
// 
//         std::string str30("E1");
//         std::string str31("E2");
//         std::string str32("E3");
//         std::string str33("E4");
//         std::string str34("E5");
//         std::string str35("E6");
//         if (str30.compare(symbol->get_data()) == 0) return 30;
//         if (str31.compare(symbol->get_data()) == 0) return 31;
//         if (str32.compare(symbol->get_data()) == 0) return 32;
//         if (str33.compare(symbol->get_data()) == 0) return 33;
//         if (str34.compare(symbol->get_data()) == 0) return 34;
//         if (str35.compare(symbol->get_data()) == 0) return 35;
// 
//         std::string str40("W1");
//         std::string str41("W2");
//         std::string str42("W3");
//         std::string str43("W4");
//         std::string str44("W5");
//         std::string str45("W6");
//         if (str40.compare(symbol->get_data()) == 0) return 40;
//         if (str41.compare(symbol->get_data()) == 0) return 41;
//         if (str42.compare(symbol->get_data()) == 0) return 42;
//         if (str43.compare(symbol->get_data()) == 0) return 43;
//         if (str44.compare(symbol->get_data()) == 0) return 44;
//         if (str45.compare(symbol->get_data()) == 0) return 45;
// 
//         std::string str50("N25");
//         std::string str51("N75");
//         std::string str52("N125");
//         std::string str53("N175");
//         if (str50.compare(symbol->get_data()) == 0) return 50;
//         if (str51.compare(symbol->get_data()) == 0) return 51;
//         if (str52.compare(symbol->get_data()) == 0) return 52;
//         if (str53.compare(symbol->get_data()) == 0) return 53;
// 
//         std::string str60("S25");
//         std::string str61("S75");
//         std::string str62("S125");
//         std::string str63("S175");
//         if (str60.compare(symbol->get_data()) == 0) return 60;
//         if (str61.compare(symbol->get_data()) == 0) return 61;
//         if (str62.compare(symbol->get_data()) == 0) return 62;
//         if (str63.compare(symbol->get_data()) == 0) return 63;
// 
//         std::string str70("E25");
//         std::string str71("E75");
//         std::string str72("E125");
//         std::string str73("E175");
//         std::string str74("E225");
//         if (str70.compare(symbol->get_data()) == 0) return 70;
//         if (str71.compare(symbol->get_data()) == 0) return 71;
//         if (str72.compare(symbol->get_data()) == 0) return 72;
//         if (str73.compare(symbol->get_data()) == 0) return 73;
//         if (str74.compare(symbol->get_data()) == 0) return 74;
// 
//         std::string str80("W25");
//         std::string str81("W75");
//         std::string str82("W125");
//         std::string str83("W175");
//         std::string str84("W225");
//         if (str80.compare(symbol->get_data()) == 0) return 80;
//         if (str81.compare(symbol->get_data()) == 0) return 81;
//         if (str82.compare(symbol->get_data()) == 0) return 82;
//         if (str83.compare(symbol->get_data()) == 0) return 83;
//         if (str84.compare(symbol->get_data()) == 0) return 84;


        //Code not recognized
        cout << "NOT RECOGNOZED: decoded " << symbol->get_type_name ();
        cout << " symbol \"" << symbol->get_data () << '"' << " " << endl;
        return -1; 
      /*       RotatedRect r = minAreaRect(vp);
         Point2f pts[4];
         r.points(pts);
         for(int i=0;i<4;i++){
         line(frame,pts[i],pts[(i+1)%4],Scalar(255,0,0),3);
         }
         cout<<"Angle: "<<r.angle<<endl;
       */ 
    }
    return 0;
}

int
main ()
{

  //Vars
  int ret;
  int numcam;
  HCAM m_hCam = 0;
  int m_nSizeX = SIZE_X;
  int m_nSizeY = SIZE_Y;
  char *m_pcImageMemory;
  int m_nMemoryId;
  int m_nBitsPerPixel = 24;
  int nImages=0;
  char filename[100];
  struct timeval t1, t2;
  int numims = NUM_IMGS;
  double ellapsedtime;

  int imX, imY, imBits;

  try{
  CRS232 serial("Serial_port");
  //Init serial
  if (SERIAL) {
    if (DEBUG) cout <<"Opening serial port"<<endl;
    TRS232_config serial_config;
    unsigned char data,answer;

    serial_config.baud=9600;
// 	serial_config.baud = 57600;
    serial_config.num_bits=8;
    serial_config.parity=none;
    serial_config.stop_bits=1;
    try{
      serial.open((void *)&serial_dev);
      serial.config(&serial_config);
      if (DEBUG) cout <<"Serial port active"<<endl;
    }catch(CException &e){
      std::cout << e.what() << std::endl;
    }
  }
  //Init camera
  //Query information     
  ret = is_GetNumberOfCameras (&numcam);
  if (ret != IS_SUCCESS)
  {
      cout << "Error querying number of cameras"<<endl;
      return -1;
  } else {
    if (DEBUG) cout << "Number of cameras: "<< numcam<<endl;
  }

  //Open camera
  gettimeofday (&t1, NULL);
  ret = is_InitCamera (&m_hCam, NULL);
  if (ret != IS_SUCCESS)
  {
      cout << "Error opening camera"<<endl;
      return -1;
  }
  gettimeofday (&t2, NULL);
  ellapsedtime = (t2.tv_sec - t1.tv_sec) * 1000.0;
  ellapsedtime += (t2.tv_usec - t1.tv_usec) / 1000.0;
  if (DEBUG) cout << "Initialization took "<< ellapsedtime / 1000<<"seconds"<<endl;

  //configura 640x480             
  IMAGE_FORMAT_INFO formatInfo;
  formatInfo.nFormatID = 13;
  ret = is_ImageFormat (m_hCam, IMGFRMT_CMD_SET_FORMAT, &formatInfo.nFormatID, 4);
  if (ret != IS_SUCCESS)
  {
      cout <<"is_ImageFormat failed with "<< ret<<endl;
  }
  //Allocate and set image memory
  ret = is_AllocImageMem (m_hCam, m_nSizeX, m_nSizeY, m_nBitsPerPixel,
		      &m_pcImageMemory, &m_nMemoryId);
  if (ret != IS_SUCCESS)
  {
      cout << "Error allocating image memory"<<endl;
  }

  if (m_pcImageMemory != NULL)
  {
      ret = is_SetImageMem (m_hCam, m_pcImageMemory, m_nMemoryId);
      if (ret != IS_SUCCESS)
	    {
	      cout <<"Error setting image mem"<<endl;
	    }
  }
  //XS seems only to allow rgb16 on raspberry
  //this change (or any other) has not worked for me
  /*   ret=is_SetColorMode(m_hCam,IS_CM_MONO8);
     if(ret != IS_SUCCESS){
     printf("Error setting color mode\n");
     }
   */
  /////////////////////////////////////////////////////// 
  if (DEBUG) cout << "Starting time measurement..."<<endl;
  gettimeofday (&t1, NULL);
  bool rgbOk = false;
  int QRcode;
  //get one image to initialize data structures
  ret =	is_InquireImageMem (m_hCam, m_pcImageMemory, m_nMemoryId, &imX, &imY,
			    &imBits, NULL);

     Mat gray;

  if (SHOW)  cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );
  //for (n = 0; n < NUM_IMGS; n++)
  while (1)
    {
// 		unsigned char data, num;
// 		num = serial.read(&data, 1);
// 		if(num == '1')
// 		{
			
// 		sleep(0.5);
      if (DEBUG) cout << "Capture"<<endl;
      ret = is_FreezeVideo (m_hCam, IS_WAIT);
      if (ret != IS_SUCCESS)
	    {
	      cout << "Error during capture process "<<ret<<endl;
	    }
	    else
		{
      //get image and convert to opencv
      if (DEBUG) cout << "Inquire"<<endl;
      ret =	is_InquireImageMem (m_hCam, m_pcImageMemory, m_nMemoryId, &imX, &imY,
			    &imBits, NULL);
      if (ret != IS_SUCCESS)
	    {
	      cout << "Error during inquire. Error = "<< ret<<endl;
	    }
      if (DEBUG) cout <<"Image is: "<< imX<<" "<< imY<<" "<< imBits<<endl;

      //-----------------ZBAR scanner ----------------
      // Create a RBG image from the camera pointer
      IplImage *rgb888Image =  cvCreateImageHeader (cvSize (imX, imY), IPL_DEPTH_8U, 3);
      rgb888Image->imageData = m_pcImageMemory;
      //create a grey image from RGB
 
      Mat rgb(rgb888Image);
      cvtColor(rgb,gray,CV_BGR2GRAY);
      //find QR codes
      QRcode = findQR(gray);
      cout << "DECODED "<<QRcode<<endl;;
	  
	  
	  

// send command to robot
	if (SERIAL) {
		if (QRcode>0) {
		unsigned char aChar = '0' + QRcode; //convert int to char
		serial.write(&aChar,1);
		sleep(2);
		
		}
	}
	 
      

      //Save images if required
      if (DEBUG) cout << "Save"<<endl;

      if (SAVE) {
        IMAGE_FILE_PARAMS ImageFileParams;
          //sprintf (filename, "testimage%d.bmp", n);
        ImageFileParams.pwchFileName = L"test.jpg";
        ImageFileParams.nFileType = IS_IMG_JPG;
        ImageFileParams.nQuality = 80;
        ImageFileParams.pnImageID = NULL;
        ImageFileParams.ppcImageMem = NULL;
        ret = is_ImageFile (m_hCam, IS_IMAGE_FILE_CMD_SAVE, (void*)&ImageFileParams,
                           sizeof(ImageFileParams));
        if (ret != IS_SUCCESS)
        {
          cout <<"Error saving image in iteration "<< nImages<<endl;
        }
      }

      //show images if required
      if (SHOW) {
        cvShowImage( "mywindow", rgb888Image);
        if ( (cvWaitKey(10) & 255) == 27 ) break;
      }
      
      cvReleaseImageHeader(&rgb888Image);
      nImages++;
// 		}
   }
	}
      //}
  gettimeofday (&t2, NULL);
  //////////////////////////////////////////////////////
  ellapsedtime = (t2.tv_sec - t1.tv_sec) * 1000.0;
  ellapsedtime += (t2.tv_usec - t1.tv_usec) / 1000.0;
  if (DEBUG) cout << "Ellapsed time in seconds: "<< ellapsedtime / 1000<<endl;
  if (DEBUG) cout << "Captured "<<nImages<<" images in "<< ellapsedtime / 1000<<" seconds"<<endl;
  if (DEBUG) cout << "Frame rate = "<<(float) numims / (ellapsedtime / 1000)<<" fps" <<endl;

  ret = is_FreeImageMem (m_hCam, m_pcImageMemory, m_nMemoryId);
  if (ret != IS_SUCCESS)
  {
      cout << "Error freeing image memory"<<endl;
  } else {
      if (DEBUG) cout << "Success in freeing mem"<<endl;
  }
  ret = is_ExitCamera (m_hCam);
  }catch(CException &e){
	  printf("%s\n",e.what().c_str());
  }
}
