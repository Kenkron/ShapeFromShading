/*
 * WoodhamNormalMapper.cpp
 *
 *  Created on: Mar 30, 2016
 *      Author: kenkron
 */

#include "Util.h"
#include "WoodhamData.h"
#include <fstream>
#include <vector>
#include <omp.h>

using Eigen::MatrixXd;
using namespace cimg_library;

WoodhamData::WoodhamData(std::istream &inputStream) {
	std::string line;
	float x, y, z, w;
	std::string filename;
	while (std::getline(inputStream, line)) {
		if (line[0] != '#') {
			std::istringstream iss(line);
			if (!(iss >> x >> y >> z >> w >> filename)) {
				//error
				std::cerr << "Line could not be parsed: " << line << std::endl;
				continue;
			}
			std::string path = "pillow/smallcolor/" + filename;
			CImg<float> colored(path.c_str());
			RadianceMap* newMap = new RadianceMap();
			newMap->lightingDirection = new Eigen::Vector3f(x, y, z);
			newMap->image = greyscaleFrom(colored);
			this->RadianceMaps.push_back(newMap);
		}
	}
	int imageWidth = RadianceMaps[0]->image->width();
	int imageHeight = RadianceMaps[0]->image->height();
	this->normalMap = new CImg<float>(imageWidth, imageHeight, 1, 3, 1);
}

void WoodhamData::initNormals(NormalTemplate normaltemplate) {
	int imageWidth = normalMap->width();
	int imageHeight = normalMap->height();
	switch (normaltemplate) {
	case CHECKER:
		for (int x = 0; x < imageWidth; x++) {
			for (int y = 0; y < imageHeight; y++) {

				//Eigen::Vector3f tmp = Eigen::Vector3f(x*4.0/(imageWidth+1)-2, y*4.0/(imageHeight+1)-2,1);
				Eigen::Vector3f tmp = Eigen::Vector3f(
						(x > imageWidth / 2) ? 1 : -1,
						(y > imageHeight / 2) ? -1 : 1, 0.25f);
				tmp = tmp / tmp.norm();
				(*this->normalMap)(x, y, 0, 0) = tmp.x() / 2 + 0.5;
				(*this->normalMap)(x, y, 0, 1) = tmp.y() / 2 + 0.5;
				(*this->normalMap)(x, y, 0, 2) = tmp.z() / 2 + 0.5;
			}
		}
		break;
	case SP:
		for (int x = 0; x < imageWidth; x++) {
			for (int y = 0; y < imageHeight; y++) {

				//Eigen::Vector3f tmp = Eigen::Vector3f(x*4.0/(imageWidth+1)-2, y*4.0/(imageHeight+1)-2,1);
				Eigen::Vector3f tmp = Eigen::Vector3f(2.0 * x / imageWidth - 1,
						-(2.0 * y / imageHeight - 1), 0.25f);
				tmp = tmp / tmp.norm();
				(*this->normalMap)(x, y, 0, 0) = -tmp.x() / 2 + 0.5;
				(*this->normalMap)(x, y, 0, 1) = -tmp.y() / 2 + 0.5;
				(*this->normalMap)(x, y, 0, 2) = tmp.z() / 2 + 0.5;
			}
		}
		break;
	}
}

CImg<float>* WoodhamData::GenerateNormalMap() {
	int rows = this->RadianceMaps.size();
	MatrixXd m(rows, 3);
	MatrixXd mt(3, rows);
	for (int i = 0; i < rows; i++) {
		//
		m(i, 0) = (*RadianceMaps[i]->lightingDirection)(0);
		m(i, 1) = (*RadianceMaps[i]->lightingDirection)(1);
		m(i, 2) = (*RadianceMaps[i]->lightingDirection)(2);

		//
		mt(0, i) = (*RadianceMaps[i]->lightingDirection)(0);
		mt(1, i) = (*RadianceMaps[i]->lightingDirection)(1);
		mt(2, i) = (*RadianceMaps[i]->lightingDirection)(2);
	}

	Eigen::MatrixXd MTM = mt * m;

	int imageWidth = RadianceMaps[0]->image->width();
	int imageHeight = RadianceMaps[0]->image->height();
	
#pragma omp parallel
	{
		#pragma omp for
	for (int x = 0; x < imageWidth; x++) {
		for (int y = 0; y < imageHeight; y++) {
			MatrixXd I(rows, 1);
			for (int img = 0; img < rows; img++) {
				I(img) = (*RadianceMaps[img]->image)(x, y);
			}
			Eigen::MatrixXd MTI = mt * I;
			Eigen::MatrixXd linalg(3, 4);
			linalg << MTM, MTI;
			rref(linalg);
			//normalize column 3
			linalg.col(3) = linalg.col(3) / linalg.col(3).norm();
			for (int i = 0; i < 3; i++) {
				(*this->normalMap)(x, y, 0, i) = linalg(i, 3) / 2.0 + 0.5;
			}
		}
	}
	}
	return this->normalMap;
}

void WoodhamData::GenerateLightingDirection() {
	int iterations = RadianceMaps.size();
#pragma omp parallel
	{
		#pragma omp for
		for (int r = 0; r < iterations; r++) {
			int imageWidth = (this->normalMap)->width();
			int imageHeight = (this->normalMap)->height();
			MatrixXd NTN(3, 3);
			NTN << 0, 0, 0, 0, 0, 0, 0, 0, 0;
			MatrixXd Rn(3, 1);
			Rn << 0, 0, 0;
			MatrixXd n(3, 1);
			MatrixXd ntn(3, 3);
			for (int x = 0; x < imageWidth; x++) {
				for (int y = 0; y < imageHeight; y++) {
					for (int i = 0; i < 3; i++) {
						n(i, 0) = (*this->normalMap)(x, y, 0, i) * 2.0 - 1;
					}
					//ntn = n * n.transpose();

					//faster version of n*n.transpose() (no allocation):
					ntn(0, 0) = n(0) * n(0);
					ntn(0, 1) = n(1) * n(0);
					ntn(0, 2) = n(2) * n(0);
					ntn(1, 0) = n(0) * n(1);
					ntn(1, 1) = n(1) * n(1);
					ntn(1, 2) = n(2) * n(1);
					ntn(2, 0) = n(0) * n(2);
					ntn(2, 1) = n(1) * n(2);
					ntn(2, 2) = n(2) * n(2);
					NTN += ntn;
					Rn += n * ((*RadianceMaps[r]->image)(x, y));
				}
			}
			Eigen::MatrixXd linalg(3, 4);
			linalg << NTN, Rn;
			rref(linalg);
			//normalize column 3
			linalg.col(3) = linalg.col(3) / linalg.col(3).norm();
//		std::cout << "Direction Generation: %" << r * 100 / RadianceMaps.size()
//				<< std::endl;
			for (int i = 0; i < 3; i++) {
				(*RadianceMaps[r]->lightingDirection)(i) = linalg(i, 3);
			}
		}
	}
}

float WoodhamData::getError() {
	float error;
	int iterations = RadianceMaps.size();
	#pragma omp parallel reduction( + : error )
	{
		#pragma omp for
		for (int r = 0; r < iterations; r++) {
			int imageWidth = (this->normalMap)->width();
			int imageHeight = (this->normalMap)->height();
			Eigen::Vector3f n;
			float localerr=0;
			for (int x = 0; x < imageWidth; x++) {
				for (int y = 0; y < imageHeight; y++) {
					for (int i = 0; i < 3; i++) {
						n(i) = (*this->normalMap)(x, y, 0, i) * 2.0 - 1;
					}
					float tmp = n.dot(*RadianceMaps[r]->lightingDirection)-(*RadianceMaps[r]->image)(x,y);
					localerr += (tmp*tmp);
				}
			}
			error += localerr;
		}
	}
	return error;
}

WoodhamData::~WoodhamData() {
	// TODO Auto-generated destructor stub
}

