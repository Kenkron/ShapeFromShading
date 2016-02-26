#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <vector>
#include "CImg.h"

using Eigen::MatrixXd;
using namespace cimg_library;

struct lighting{
    float x,y,z;
    CImg<float>* image;
};

CImg<float>* greyscaleFrom(CImg<float> &src){
    int width = src.width();
    int height = src.height();
    CImg<float>* dest=new CImg<float>(width,height);
    cimg_forXY(src,x,y){
	float r = src(x,y,0), g=src(x,y,0),b=src(x,y,0);
	//it doesn't seem to matter whether grey is scaled
	(*dest)(x,y) = (r+g+b)/3.0;
    }
    return dest;
}

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
    //now parse the pillows
    std::vector<lighting> sources;
    
    
    std::ifstream infile("pillow/lights/lights.data");
    std::string line;
    while (std::getline(infile, line))
	{
	    if (line.find(" ")!=line.find("\n")){
		lighting newlight;
		char *l = new char[line.length() + 1];
		std::strcpy(l, line.c_str());
		char *s = " ";
		
		newlight.x = atof(strtok(l,s));
		newlight.y = atof(strtok(NULL,s));
		newlight.z = atof(strtok(NULL,s));
		//std::cout << newlight.x<< std::endl;
		atof(strtok(NULL,s));//intensity
		/*
		  index = line.find(" ");
		  param = line.substr(0,index);
		  line = line.substr(index);
		  newlight.x = stof(param);
		  
		  index = line.find(" ");
		  param = line.substr(0,index);
		  line = line.substr(index);
		  newlight.y = stof(param);
		  
		  index = line.find(" ");
		  param = line.substr(0,index);
		  line = line.substr(index);
		  newlight.z = stof(param);
		  
		  index = line.find(" ");
		  param = line.substr(0,index);
		  line = line.substr(index);//intensity
		*/
		char* filename = strtok(NULL,s);
		char* dir = "pillow/smallcolor/";
		char* path = new char[strlen(dir)+strlen(filename)];
		*path = 0;
		strcat(path, dir);
		strcat(path, filename);
		CImg<float> colored(path);
		newlight.image = greyscaleFrom(colored);
		sources.push_back(newlight);
		/*std::cout << colored(5,5,0,1);*/
		/*
		CImgDisplay origin(*(newlight.image),"img");
		while (!origin.is_closed()){
		    origin.wait();
		    }*/
		delete(l);
		delete(path);
	    }
	    // process pair (a,b)
	    // std::cout << "done" << std::endl;
	}

    CImg<float> maxes(640,480,1,3,1);
    cimg_forXY(maxes,x,y){
	float max = 0;
	for (int i = 0; i < sources.size(); i++){
	    if ((*(sources[i].image))(x,y)>max){
		Eigen::Vector3d dir(sources[i].x, sources[i].y, sources[i].z);
		//normalize
		dir = dir/dir.norm();
		Eigen::Vector3d offset(0.5,0.5,0.5);
		dir = dir/2+offset;
		maxes(x,y,0,0) = dir(0); 
		maxes(x,y,0,1) = dir(1);
		maxes(x,y,0,2) = dir(2);
		max = (*(sources[i].image))(x,y);
	    }
	}
	//std::cout << x <<","<<y<<":"<<max<<" "<<maxes(x,y,0,0)<<","<<maxes(x,y,0,1)<<","<<maxes(x,y,0,2)<<std::endl; 
    }
    CImgDisplay norm1(maxes,"img");
    while (!norm1.is_closed()){
	norm1.wait();
    }
    return 0;
}
