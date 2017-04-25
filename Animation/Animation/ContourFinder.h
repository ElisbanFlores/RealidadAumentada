#pragma once
#include "PatternPoint.h"

class ContourFinder{

public:
	vector< vector<Point> > contours;

	ContourFinder();
	~ContourFinder();
	vector< vector<Point> > FindContours(cv::Mat , cv::Mat &);
};