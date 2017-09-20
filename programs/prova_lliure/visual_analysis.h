#ifndef VISUAL_ANALYSIS_H
#define VISUAL_ANALYSIS_H

#include <math.h>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"

//Llibreria bàsica de visió per computador. Documentació a https://github.com/aniolgarcia/robohead

using namespace cv;

/**
 *\ingroup robohead
 *\class visual_analysis
 *\brief The class used to get images form the camera and track objects in these images.
 * This class contains all the necessary methods to get images from the camera and search colors or faces inside them.
 * Most of this methods take as argument the variables where the position from image will be stored. 
 */



class visual_analysis
{

	public:

		//----------------- CONSTRUCTOR & DESTRUCTOR ---------------//

		/**
		 *\brief Constructor of the visual_analysis class
		 * It creates and initializes a VideoCapture object (the camera), checks if that object is able to get images and sets the HSV values to orange.
		 *
		 */
		visual_analysis();

		/**
		 *\brief Destructor of the class
		 *
		 */
		~visual_analysis();

		//----------------- METHODS --------------//

		/**
		 *\brief It fills a matrix with the data obtined form a captured frame
		 *
		 *\param opiginal_img A matrix that will contain the captured frame
		 *\return Returns true if success
		 *
		 */
		bool get_img(Mat& original_img);

		/**
		 *\brief It searches for a specific color in the image
		 * Given the image and the range of colors to search in HSV system, it gets the coordinates in the image where the color is and save it in x and y variables
		 *
		 *\param x The double where the x coordinate form image will be saved
		 *\param y The double where the y coordinate from image will be saved 
		 *\param opiginal_img A matrix that contain the frame to analize
		 *\param min_H_ Minimum hue to search
		 *\param max_H_ Maximum hue to search
		 *\param min_S_ Minimum saturatuion to search
		 *\param max_S_ Maximum saturatuion to search
		 *\param min_V_ Minimun color valur to search
		 *\param max_V_ Maximum color value to search
		 *\return It returns an integrer that can be used to set a face expression later.
		 */
		int get_color(double& x, double& y, Mat& original_img, int min_H_, int max_H_, int min_S_, int max_S_, int min_V_, int max_V_);

		/**
		 *\brief It searches for a orange ball
		 * It searches for an orange ball and return its position in the image. It can be useful to test various features.  
		 *
		 *\param x The double where the x coordinate form image will be saved
		 *\param y The double where the y coordinate from image will be saved 
		 *\param opiginal_img A matrix that contain the frame to analize
		 *\return It returns an integrer that can be used to set a face expression later.
		 */
		int get_ball(double& x, double& y, Mat& original_img);

		/**
		 *\brief It searches for a face in an image
		 * It will return the position of the biggest face that it's able to find. Depending on the camera, the face recognition can vary, so it's not very reilable.
		 * NOTE: It has some trouble with people wearing glasses... 
		 *
		 *\param x The double where the x coordinate form image will be saved
		 *\param y The double where the y coordinate from image will be saved 
		 *\param opiginal_img A matrix that contain the frame to analize
		 *\return It returns an integrer that can be used to set a face expression later.
		 */
		int get_face(double& x, double& y, Mat& original_img);

		/**
		 *\brief It searches for a face in an image
		 *
		 *\param x The double where the x coordinate form image will be saved
		 *\param y The double where the y coordinate from image will be saved 
		 *\param opiginal_img A matrix that contain the frame to analize
		 */
		void get_circle(double& x, double& y, Mat& original_img);


	private:
		VideoCapture camera; //Cosntructor of the VideoCapture object, used to get the frames.
		

		int min_H;
		int max_H;

		int min_S;
		int max_S;

		int min_V;
		int max_V;

		int last_x;
		int last_y;

		bool success;
		
		
};

#endif //VISUAL_ANALYSIS_H

