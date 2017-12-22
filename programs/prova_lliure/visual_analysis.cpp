#include <iostream>
#include <math.h>

#include "visual_analysis.h"
#define DEFAULT_FACE_CASCADE "/usr/share/opencv/lbpcascades/lbpcascade_frontalface.xml"

//Llibreria bàsica de visió per computador. Documentació a https://github.com/aniolgarcia/robohead

using namespace cv;

visual_analysis::visual_analysis()
{
	camera = VideoCapture(0); //Definition of the VideoCapture object

	if ( !camera.isOpened() )  // if not success, exit program
	{
		 std::cout << "Cannot open the web cam" << std::endl;
	}
	//Values for orange color in HSV, change them if you plan to use a visual_analysis::get_ball vith a non orange object
	min_H = 0;
	max_H = 360;

	min_S = 231;
	max_S = 255;

	min_V = 37;
	max_V = 255;

	last_x = -1;
	last_y = -1;
}

//_________________________________________________________________________________________________

visual_analysis::~visual_analysis()
{
}

//_________________________________________________________________________________________________

bool visual_analysis::get_img(Mat& original_img)
{
	success = camera.read(original_img);
	return success;    
}

//_________________________________________________________________________________________________

int visual_analysis::get_color(double& x, double& y, Mat& original_img, int min_H_, int max_H_, int min_S_, int max_S_, int min_V_, int max_V_)
{
	if(!success)
	{
		std::cout << "Cannot read a frame from video stream" << std::endl;
		
	}

	Mat hsv_img; //Creation of the HSV matrix

	cvtColor(original_img, hsv_img, COLOR_BGR2HSV); //Changing RGB values to HSV

	Mat processed_img; //Creation ot the cleaned and filtered image

	inRange(hsv_img, Scalar(min_H_, min_S_, min_V_), Scalar(max_H_, max_S_, max_V_), processed_img); //Applying the filters to the image

	erode(processed_img, processed_img, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) );  //Cleaning the unnecessary particles
	dilate(processed_img, processed_img, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) ); 

	dilate(processed_img, processed_img, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) ); 
	erode(processed_img, processed_img, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) );

	Moments img_moments = moments(processed_img);  //Getting the position of the ball

	double m01 = img_moments.m01;
	double m10 = img_moments.m10;
	double area = img_moments.m00;
	x = m10/area;
	y = m01/area;

	//Printing out the results
	if(isnan(x) == false && isnan(y) == false) //Seeing if results are logical
	{
		std::cout << x << "  "  << y << std::endl;
		imshow("processed image", processed_img);
		imshow("original", original_img);
		waitKey(20);
		return 1; //That value corresponds to the happy expression of the face library
	}
	else
	{
		x = 0;
		y = 0;
		imshow("processed image", processed_img);
		imshow("original", original_img);
		waitKey(20);
		return 3; //Value of the angry face
	}
}

//_________________________________________________________________________________________________

int visual_analysis::get_ball(double& x, double& y, Mat& original_img)
{   
	if(!success)
	{
		std::cout << "Cannot read a frame from video stream" << std::endl;
		
	}

	Mat hsv_img; //Creation of the HSV matrix

	cvtColor(original_img, hsv_img, COLOR_BGR2HSV); //Changinr RGB values to HSV

	Mat processed_img; //Creation ot the cleaned and filtered image

	inRange(hsv_img, Scalar(min_H, min_S, min_V), Scalar(max_H, max_S, max_V), processed_img); //Applying the filters to the image

	erode(processed_img, processed_img, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) );  //Cleaning the unnecessary particles
	dilate(processed_img, processed_img, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) ); 

	dilate(processed_img, processed_img, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) ); 
	erode(processed_img, processed_img, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) );

	Moments img_moments = moments(processed_img);  //Getting the position of the ball

	double m01 = img_moments.m01;
	double m10 = img_moments.m10;
	double area = img_moments.m00;
	x = m10/area;
	y = m01/area;

	//Printing out the results
	if(isnan(x) == false && isnan(y) == false) //Seeing if results are logical
	{
		std::cout << x << "  "  << y << std::endl;
		imshow("processed image", processed_img);
		imshow("original", original_img);
		waitKey(20);
		return 1; //That value corresponds to the happy face of the face library
	}
	else
	{
		x = 0;
		y = 0;
		imshow("processed image", processed_img);
		imshow("original", original_img);
		waitKey(20);
		return 3; //Value of the angry face
	}
}

//_________________________________________________________________________________________________

int visual_analysis::get_face(double& x, double& y, Mat& original_img)
{
	if(!success)
	{
		std::cout << "Cannot read a frame from video stream" << std::endl;
	}

	Mat img_gray; //Creation of the matrix for the gray image

	cvtColor(original_img, img_gray, COLOR_BGR2GRAY); //Changing RGB data to GRAY data
	equalizeHist(img_gray, img_gray); //Cleaning the image

	CascadeClassifier face_cascade_; //Creating the object for the cascade

	std::string face_cascade_file = DEFAULT_FACE_CASCADE; //Using the default Cascade file for face detection

	if (!face_cascade_.load(face_cascade_file)) //Loading the file
	{
			std::cerr << "Error loading " << face_cascade_file << "!!" << std::endl;
	}

	Rect face_region; //Creating a rectangle for the face position

	assert(img_gray.channels() == 1);
	std::vector<cv::Rect> faces;

	face_cascade_.detectMultiScale(img_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, //Looking for faces
					   cv::Size(50, 50), // Min size
					   cv::Size(400, 400));// Max size

	
	if (faces.size() > 0) 
	{
		//Getting the biggest face
		face_region = faces[0];
		int bigarea = face_region.width*face_region.height;
		for (size_t i = 1; i < faces.size(); ++i)
		{
			int area_i = faces[i].width*faces[i].height;
			if (area_i > bigarea) 
			{
				face_region = faces[i];
				bigarea = area_i;
			}
		}
		//calculating the coordinates of the face
		x = face_region.x + (face_region.width/2);
		y = face_region.y + (face_region.height/2);

		std::cout << x << "  "  << y << std::endl;
		rectangle(original_img, face_region, CV_RGB(0, 255,0), 1);
		imshow("original", original_img);
		waitKey(10);
		return 1;
	}
	else
	{
		//Printing results
		std::cout << x << "  "  << y << std::endl;
		rectangle(original_img, face_region, CV_RGB(0, 255,0), 1);
		imshow("original", original_img);
		waitKey(10);
		return 3;
	}

}

//_________________________________________________________________________________________________

void visual_analysis::get_circle(double& x, double& y, Mat& original_img)
{
	if(!success)
	{
		std::cout << "Cannot read a frame from video stream (get_circle)" << std::endl;
	}

	Mat img_gray;

	cvtColor(original_img, img_gray, COLOR_BGR2GRAY);
	//GaussianBlur(img_gray, img_gray, Size(9, 9), 2, 2 ); //May help depending on your camera. Uncomment it if you don't get any circle.

	vector<Vec3f> circles; //Creating a vector of vectors with size 3 (a 3*n matrix) to store the screen coordinates of the centre and its radius

	HoughCircles(img_gray, circles, CV_HOUGH_GRADIENT, 1, img_gray.rows/2, 150, 100, 0, 0 ); //Searching for the circles
	std::cout << "Circles found" << std::endl;
	int max_radius = circles[0][2];
	std::cout << circles.size() << std::endl;
	int max_circle_x, max_circle_y;

	//Looking for the biggest circle
	for(int i = 0; i < (int) circles.size(); i++ )
	{
   		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
   		int radius = cvRound(circles[i][2]);
   		if(circles[i][2] > max_radius)
   		{
   			max_radius = circles[i][2];
   			max_circle_x = circles[i][0];
   			max_circle_y = circles[i][1];

   		}
   
   		circle(original_img, center, 3, Scalar(0,255,0), -1, 8, 0 );
   		circle(original_img, center, radius, Scalar(0,0,255), 3, 8, 0 );
 	}
 	//Getting the coordinates df the centre of the biggest head.
 	x = max_circle_x;
 	y = max_circle_y;
 	std::cout << x << "  " << y << std::endl;

 	//namedWindow( "Circle result", CV_WINDOW_AUTOSIZE );
	imshow( "Circle result", original_img);
	waitKey(20);

}

//_________________________________________________________________________________________________
