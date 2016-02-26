#include <iostream>
#include <Eigen/Dense>
#include "CImg.h"

using Eigen::MatrixXd;
using namespace cimg_library;

int main(int narg, char* argv[]){
    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    std::cout << m << std::endl;

    if (narg>1){
	CImg<unsigned char> src(argv[1]);
	int width = src.width();
	int height = src.height();
	CImg<float> dest(width,height);
	CImg<float> dest2(width,height);
	cimg_forXY(src,x,y){
	    float r = src(x,y,0), g=src(x,y,0),b=src(x,y,0);
	    dest(x,y) = r+g+b;
	    //it doesn't seem to matter whether grey is scaled
	    dest2(x,y) = (r+g+b)/3.0;
	}

	CImgDisplay origin(src,"src"), d1(dest,"dest"), d2(dest2,"dest2");

	// wait until main window is closed
	while (!origin.is_closed()){
	    origin.wait();
	}
    }
    return 0;
}
