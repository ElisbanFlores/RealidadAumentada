#include "PreProcessing.h"

PreProcessing::PreProcessing() {
}

PreProcessing::~PreProcessing() {
}

cv::Mat PreProcessing::PreProcess(cv::Mat& in, cv::Mat& out,bool thr) {
	if(!(in.type() == CV_8UC1))
		cvtColor(in, out, CV_BGR2GRAY);
	
	//imshow("preprocessssss", in);
	//waitKey(1);
	
	if (thr == true)
	{	
		//adaptiveThreshold(out, out, 255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,25,10);
		GaussianBlur(out, out, Size(9,9), 0, 0);
		//medianBlur(out, out, 9);
		adaptiveThreshold(out, out, 255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,75,25);
		//AdaptiveThreshold(out, out,0.5f);
		//adaptiveThreshold(out, out, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 75, 40);
	}
	else
	{
		GaussianBlur(out, out, Size(5, 5), 0, 0);
		// solo con Circulos
		//adaptiveThreshold(out, out, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 25, 10);		
	}
	
	//imshow("preprocessssss",out);

	return out;
}


//Adaptive Thresholding Using the Integral Image
void PreProcessing::AdaptiveThreshold(cv::Mat input, cv::Mat &bin, float T)
{
	bin = cv::Mat(input.rows, input.cols, CV_8UC1);
		
	unsigned long* integralImg;
	int i, j;
	long sum = 0;
	int count = 0;
	int index;
	int x1, y1, x2, y2;
	int cols = input.cols;
	int rows = input.rows;
	int s2 = (cols / 8) / 2;
	
	integralImg = new unsigned long[cols*rows];
	
	for (i = 0; i < cols; i++){
		sum = 0;
		for (j = 0; j < rows; j++){
			index = j*cols + i;
			sum += input.at<unsigned char>(j, i);//[index];
												 //input.at()
			if (i == 0)
				integralImg[index] = sum;
			else
				integralImg[index] = integralImg[index - 1] + sum;
		}
	}
	
	for (i = 0; i<cols; i++){
		for (j = 0; j<rows; j++){
			index = j*cols + i;

			// set the SxS region
			x1 = i - s2; x2 = i + s2;
			y1 = j - s2; y2 = j + s2;

			// check the border
			if (x1 < 0) x1 = 0;
			if (x2 >= cols) x2 = cols - 1;
			if (y1 < 0) y1 = 0;
			if (y2 >= rows) y2 = rows - 1;

			count = (x2 - x1)*(y2 - y1);

			// I(x,y)=s(x2,y2)-s(x1,y2)-s(x2,y1)+s(x1,x1)
			sum = integralImg[y2*cols + x2] -
				integralImg[y1*cols + x2] -
				integralImg[y2*cols + x1] +
				integralImg[y1*cols + x1];

			if ((long)(input.at<unsigned char>(j, i) * count) < (long)(sum*(1.0 - T)))
				bin.at<unsigned char>(j, i) = 0;
			else
				bin.at<unsigned char>(j, i) = 255;
		}
	}
	delete integralImg;
}
