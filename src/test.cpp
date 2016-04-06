
#include <iostream>
#include <fstream>

#include <vector>

#include "Util.h"
#include "WoodhamData.h"

using Eigen::MatrixXd;
using namespace cimg_library;

struct lighting {
	float x, y, z;
	CImg<float>* image;
};

int main(int narg, char* argv[]) {
	MatrixXd m(2, 2);
	m(0, 0) = 3;
	m(1, 0) = 2.5;
	m(0, 1) = -1;
	m(1, 1) = m(1, 0) + m(0, 1);
	std::cout << m << std::endl;

	if (narg > 1) {
		CImg<unsigned char> src(argv[1]);
		int width = src.width();
		int height = src.height();
		CImg<float> dest(width, height);
		CImg<float> dest2(width, height);
		cimg_forXY(src,x,y)
		{
			float r = src(x, y, 0), g = src(x, y, 0), b = src(x, y, 0);
			dest(x, y) = r + g + b;
			//it doesn't seem to matter whether grey is scaled
			dest2(x, y) = (r + g + b) / 3.0;
		}

		CImgDisplay origin(src, "src"), d1(dest, "dest"), d2(dest2, "dest2");

		// wait until main window is closed
		while (!origin.is_closed()) {
			origin.wait();
		}
	}
	//now parse the pillows
	std::ifstream infile("pillow/lights/lights.data");
	WoodhamData woodham = WoodhamData(infile);

	//CImg<float> greyscale
	Eigen::MatrixXd reduce(2, 3);
	reduce << 1,2,3,4,5,6;
	std::cout<<"m: "<<std::endl<<reduce<<std::endl;
	rref(reduce);
	std::cout << "rref: "<<std::endl<<reduce<<std::endl;

	return 0;
}
