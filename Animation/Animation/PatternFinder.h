#pragma once

#include "CentersAndConvexFinder.h"

template <class T>
class PatternFinder: public CentersAndConvexFinder<T>{

public:
	
	PatternPoint patternGenerated;
	PatternPoint patternCorners;	
	PatternPoint hullCorners;
	
	T_INT m, n;
	T_INT patternType;

	PatternFinder() :CentersAndConvexFinder() {}
	PatternFinder(vector< RotatedRect > box) :CentersAndConvexFinder(box) {}
	PatternFinder(vector< Point2f > point):CentersAndConvexFinder(point) {}
	PatternFinder(PatternPoint pPoint) :CentersAndConvexFinder(pPoint) {}

	~PatternFinder();

	void Init(T_INT, T_INT, T_INT);
	void Init(T_INT);
	void SetPatternFinder(PatternPoint &);
	vector<Point2f> FitPatternToSimetricConvexHull();
	vector<Point2f> FitPatternToAsimetricConvexHull();
	vector<Point2f> FitPatternToConvexHull();	
	vector<Point2f> GeneratePattern(T_INT, T_INT, Point2f, Point2f, Point2f, Point2f, T_INT);
	vector<Point2f> MovePattern(vector<Point2f>, Point2f,Point2f);
	Mat DrawPattern(Mat,T_INT, T_INT, vector<Point2f>, Scalar [], T_INT);
	Mat DrawConvexHull(Mat img);
	void FindPattern(cv::Mat , cv::Mat &, int &, double &);
};

#include "PatternFinder.cpp"