
#include "ContourFinder.h"

ContourFinder::ContourFinder() {
}

ContourFinder::~ContourFinder() {
}

vector< vector<Point> > ContourFinder::FindContours(cv::Mat in, cv::Mat &out) {
	
	cv::Canny(in, out, 100, 200);	// encontrar los contornos genrados por Canny		
	contours.clear();
	findContours(
		out,
		contours,
		//CV_RETR_EXTERNAL,
		CV_RETR_TREE,
		CV_CHAIN_APPROX_NONE
	);//CV_RETR_TREE 3ms, CV_RETR_EXTERNAL 1 ms	
	
	return contours;
}
