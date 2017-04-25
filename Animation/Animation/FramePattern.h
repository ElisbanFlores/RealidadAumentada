#pragma once

#include "Common.h"

class FramePattern {
	public:
		vector<Point2f> pattern_points;
		Point2f 	mean;
		double 		min_dist;
		string		path;
		string 		imageFileName;
		string 		dataFileName;
		Mat 		frame;

		FramePattern (void);

		friend bool operator < (FramePattern fp1, FramePattern fp2){
			return fp1.min_dist < fp2.min_dist;
		}
};
