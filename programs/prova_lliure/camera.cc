#include <iostream>
#include <math.h>

#include "visual_analysis.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"

int main()
{

    cv::Mat img;
    
    visual_analysis camera;
    while(1)
    { 
            double x, y;
            int state;
            camera.get_img(img);
            state = camera.get_face(x, y, img);
            //neck.follow(x, y);
            cv::waitKey(30);
    }

}
