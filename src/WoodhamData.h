/*
 * InputParser.h
 *
 *  Created on: Mar 30, 2016
 *      Author: kenkron
 */

#ifndef SRC_WOODHAMDATA_H_
#define SRC_WOODHAMDATA_H_

#include "Util.h"
#include <vector>

using namespace cimg_library;

typedef struct RadianceMap{
    Eigen::Vector3f* lightingDirection;
    CImg<float>* image;
}RadianceMap;

enum NormalTemplate {CHECKER, SP};

class WoodhamData {
public:
	std::vector<RadianceMap*> RadianceMaps;
    CImg<float>* normalMap;
	WoodhamData(std::istream &inputStream);
	void initNormals(NormalTemplate t);
	~WoodhamData();
	CImg<float>* GenerateNormalMap();
	void GenerateLightingDirection();
};

#endif /* SRC_WOODHAMDATA_H_ */
