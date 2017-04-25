#pragma once

#include "ContourFinder.h"

class EllipseFinder:public ContourFinder{

public:
	
	PatternPoint<T_DOUBLE> initialPoints;
	PatternPoint<T_DOUBLE> finalPoints;

	EllipseFinder() :ContourFinder() {};
	~EllipseFinder();	
	bool VerifyContourShape(vector<Point> contour);
	RotatedRect FitContourToEllipse(vector<Point> contour);	
	bool VerifyEllipseNoEnlarged(RotatedRect box);	
	bool  VerifyEllipseShaped(RotatedRect box, Mat &cimage);
	PatternPoint<T_DOUBLE> ElegirElipsesFinales(int);
	PatternPoint<T_DOUBLE> ElegirElipsesFinalesAsimetrico(int);
	PatternPoint<T_DOUBLE> ElegirElipsesFinalesSimetrico(int);
	PatternPoint<T_DOUBLE> ElegirElipsesFinales2(PatternPoint<T_DOUBLE> boxElegidos, Point2f center, T_DOUBLE dmax);
	bool FindEllipse(cv::Mat& in, cv::Mat& out, int &frame_counter, Point2f center, double distMaxPatron, int tipo, int NroPuntos);	

};