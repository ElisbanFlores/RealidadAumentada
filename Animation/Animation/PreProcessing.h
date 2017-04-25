#pragma once
#include "Common.h"

class PreProcessing{

public:
	PreProcessing();
	~PreProcessing();
	cv::Mat PreProcess(cv::Mat&, cv::Mat&,bool thr=false);
	void AdaptiveThreshold(cv::Mat , cv::Mat &, float T = 0.15f);
};