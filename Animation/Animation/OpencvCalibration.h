#pragma once
#include "Common.h";
#include "opencv2/calib3d.hpp"

class OpencvCalibration{
private:

public:
	~OpencvCalibration() { };

	bool processFrame(Mat& view, Mat& out, int nroFrame) {
		
		//if (nroFrame == 1085) {
			Size boardSize;
			boardSize.width  = 4;
			boardSize.height = 11;
			vector<Point2f> pointbuf;
		
			bool found = findCirclesGrid(view, boardSize, pointbuf, CALIB_CB_ASYMMETRIC_GRID);
		
			/*if (pointbuf.size() != 44) {
				cout << "frame:" << nroFrame
					 << "," << pointbuf.size() << endl;
			}*/
			//cout << "\t found: " << found << endl;
			if (found) {
				drawChessboardCorners(view, boardSize, Mat(pointbuf), found);
			}
			cout << nroFrame << ","
				<< found << ","
				<< pointbuf.size() << endl;
		
			cout << pointbuf<<endl;
		//}
		out = view.clone();
		//imwrite("1085.jpg", view);
		
		return found;
		//cin.get();
	}
};