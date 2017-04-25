#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <math.h> 
#include <stdio.h>
#include <sstream>
#include <stdio.h> 
#include <type_traits> 
#include <cmath> 
#include <time.h>
#include <fstream>
#include <chrono>
#include <string>

#include <omp.h>

#include <stdio.h>
#include <math.h>
#include <assert.h>


using namespace std;
using namespace cv;

typedef double T_DOUBLE;
typedef char T_CHAR;
typedef long T_LONG;
typedef float T_FLOAT;
typedef int T_INT;
typedef unsigned char T_BYTE;
typedef long long TIPOTAM;

const long  BLOQUELINEA = 1024;
const int  NUMTHREADS = 8;


enum METHOD {
	OPENCV,
	OUR
};

enum FRAMESELECTION {
	NORMAL,
	DENSITY
};

enum OPERATIONMODE {
	CALIBRATE,
	SIZE
};

enum PATTERN {
	SIMETRICO,
	ASIMETRICO
};


static Scalar colorTab[] = {
	Scalar(0, 0, 255),
	Scalar(0, 255, 0),
	Scalar(255, 0, 0),
	Scalar(0, 255, 255),
	Scalar(255, 255, 0),
	Scalar(255, 0, 255),
	Scalar(255, 0, 128),
	Scalar(255, 128, 255),
	Scalar(128, 255, 255),
	Scalar(128, 255, 128),
	Scalar(255, 128, 128),
	Scalar(255, 255, 255)
};

static  Scalar colorTab2[] = {
	Scalar(0, 0, 50),
	Scalar(0, 50, 0),
	Scalar(0, 50, 50),
	Scalar(50, 0, 0),
	Scalar(50, 0, 50),
	Scalar(50, 50, 0),
	Scalar(50, 50, 50),
	Scalar(0, 0, 100),
	Scalar(0, 100, 0),
	Scalar(0, 100, 100),
	Scalar(100, 0, 0),
	Scalar(100, 0, 100),
	Scalar(100, 100, 0),
	Scalar(100, 100, 100),
	Scalar(0, 0, 150),
	Scalar(0, 150, 0),
	Scalar(0, 150, 150),
	Scalar(150, 0, 0),
	Scalar(150, 0, 150),
	Scalar(150, 150, 0),
	Scalar(150, 150, 150),
	Scalar(0, 0, 200),
	Scalar(0, 200, 0),
	Scalar(0, 200, 200),
	Scalar(200, 0, 0),
	Scalar(200, 0, 200),
	Scalar(200, 200, 0),
	Scalar(200, 200, 200),
	Scalar(0, 0, 250),
	Scalar(0, 250, 0),
	Scalar(0, 250, 250),
	Scalar(250, 0, 0),
	Scalar(250, 0, 250),
	Scalar(250, 250, 0),
	Scalar(250, 250, 250),	
};



static float EuclideanDist(Point3f& p, Point3f &q) {

	return sqrt((p.x - q.x)* (p.x - q.x) + (p.y - q.y)* (p.y - q.y) + (p.z - q.z)* (p.z - q.z));
}

static float EuclideanDist(Point2f& p, Point2f &q) {

	return sqrt((p.x - q.x)* (p.x - q.x) + (p.y - q.y)* (p.y - q.y));
}

