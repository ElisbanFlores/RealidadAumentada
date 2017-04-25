#pragma once

#include "PreProcessing.h"
#include "ContourFinder.h"
#include "EllipseFinder.h"
#include "CentersAndConvexFinder.h"
#include "PatternFinder.h"
#include "FramePattern.h"


class PatternTracker
{

public:
	int width;
	int height;
	int m;
	int n;
	int nPoints;
	int nCircles;

	///////////////////////////////////////
	// lo que debe ir en una clase aparte
	///////////////////////////////////////

	PreProcessing preProcessing;
	EllipseFinder ellipseFinder;
	PatternFinder<T_DOUBLE> patternFinder;

	int frame_counter = 1;
	double lost_frames = 0.0;
	double maximun_distance = 0.0;
	double maximun_pattern_distance = 0.0; // include in execute parameters to delete
	Point2f new_centroid; // include in execute parameters to delete
	Point2f actual_centroid; // include in execute parameters to delete	
	PatternPoint<T_DOUBLE> boxFinal;
	vector<Point3f> boardPoint;
	vector<Point2f> foundBoardPoint;

public:
	PatternTracker();
	PatternTracker(int, int, int, int, int);
	~PatternTracker();
	vector<Point3f> GenerarObjectPoint(int, int, int, int, Size);
	bool execute(Mat& );
	double GetRotTra(Mat, Mat, Mat, Mat &, Mat &, Mat &);
};


