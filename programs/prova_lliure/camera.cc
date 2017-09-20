#include <iostream>
#include <math.h>

#include "visual_analysis.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"

//Codi de la prova lliure de CEABOT 2017. Bàsicament obre una càmera i busca la cara més gran. La major part de la feina la fa visual_analysis.

int main()
{

    cv::Mat img; //Creem una matriu que contindrà la imatge
    
    visual_analysis camera; //Creem un objecte visual_analysis
    while(1)
    { 
            double x, y;
            int state;
            camera.get_img(img); //Cridem un mètode de visual_analysis per capturar una imatge
            state = camera.get_face(x, y, img); //Cridem un altre mètode per trobar la cara i la seva posició dins la imatge
            cv::waitKey(30);
    }

}
