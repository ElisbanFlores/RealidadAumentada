#pragma once
#include "PatternPoint.h"

template<class T>
class CentersAndConvexFinder: public PatternPoint<T>{
	
public:
	
	vector<T_INT> hull;
	vector<Point2f> hullPoints;
	
	CentersAndConvexFinder() :PatternPoint() {};
	CentersAndConvexFinder(vector< RotatedRect > box) : PatternPoint(box) {};
	CentersAndConvexFinder(vector< Point2f > point) : PatternPoint(point) {};
	CentersAndConvexFinder(PatternPoint pPoint) : PatternPoint(pPoint) {};
	~CentersAndConvexFinder() {};	
	void FindCentersAndConvex(cv::Mat& , cv::Mat& );
};

#include "CentersAndConvexFinder.cpp"