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

	std::ifstream infile("pillow/lights/lights.data");
	WoodhamData woodham = WoodhamData(infile);

	int iterations = 4;
	if (narg > 2) {
		iterations = atoi(argv[2]);
		if (iterations == 0) {
			//in this case, do a raw woodham calculation
			//based on the known lighting
			woodham.GenerateNormalMap();
			std::stringstream ss2;
			ss2 << argv[1] << "KnownLightingResult.png" << std::endl;
			std::string imgfile;
			ss2 >> imgfile;
			woodham.normalMap->get_normalize(0, 255).save(imgfile.c_str());
		}
	}

	woodham.initNormals(SP);
	if (narg > 1) {
		std::stringstream ss;
		ss << argv[1] << "GivenLightingInformation.log" << std::endl;
		std::string dirsfile;
		ss >> dirsfile;
		std::ofstream dirs;
		dirs.open(dirsfile.c_str());
		for (int j = 0; j < woodham.RadianceMaps.size(); j++) {
			Eigen::Vector3f tmp = (*woodham.RadianceMaps[j]->lightingDirection)
					/ (*woodham.RadianceMaps[j]->lightingDirection).norm();
			dirs << tmp(0) << " " << tmp(1) << " " << tmp(2) << std::endl;
		}
		std::stringstream ss2;
		ss2 << argv[1] << "LightingTemplate.png" << std::endl;
		std::string imgfile;
		ss2 >> imgfile;
		woodham.normalMap->get_normalize(0, 255).save(imgfile.c_str());
	} else {
		CImgDisplay main_disp(*(woodham.normalMap), "Convex Template");
		while (!main_disp.is_closed()) {
		}
	}

	std::cout<< "Estimate Error: "<< woodham.getError() << std::endl;
	for (int i = 0; i < iterations; i++) {
		woodham.GenerateLightingDirection();
		woodham.GenerateNormalMap();
		if (narg > 1) {
			std::stringstream ss;
			ss << argv[1] << "LightingEstimate" << i << ".log" << std::endl;
			std::string dirsfile;
			ss >> dirsfile;
			std::ofstream dirs;
			dirs.open(dirsfile.c_str());
			for (int j = 0; j < woodham.RadianceMaps.size(); j++) {
				dirs
						<< (*woodham.RadianceMaps[j]->lightingDirection).transpose()
						<< std::endl;
			}
			dirs.close();
			std::stringstream ss2;
			ss2 << argv[1] << "LightingEstimate" << i << ".png" << std::endl;
			std::string imgfile;
			ss2 >> imgfile;
			woodham.normalMap->get_normalize(0, 255).save(imgfile.c_str());
		} else {
			for (int j = 0; j < woodham.RadianceMaps.size(); j++) {
				Eigen::Vector3f tmp =
						(*woodham.RadianceMaps[i]->lightingDirection);
				std::cout << tmp(0) << " " << tmp(1) << " " << tmp(2)
						<< std::endl;
			}
			CImgDisplay main_disp(*(woodham.normalMap), "Latest Iteration");
			while (!main_disp.is_closed()) {
			}
		}
		std::cout<< "Estimate Error: "<< woodham.getError() << std::endl;
	}

	Eigen::MatrixXd reduce(2, 3);
	reduce << 1, 2, 3, 4, 5, 6;
	std::cout << "m: " << std::endl << reduce << std::endl;
	rref(reduce);
	std::cout << "rref: " << std::endl;
	printMatrix(reduce);

	return 0;
}
