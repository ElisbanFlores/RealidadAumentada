#pragma once

#include "PreProcessing.h"
#include "ContourFinder.h"
#include "EllipseFinder.h"
#include "CentersAndConvexFinder.h"
#include "PatternFinder.h"
#include "OpencvCalibration.h"
#include "Camera.h"
#include "FramePattern.h"


template <typename SC>
class App {

protected:
	typedef SC	setup_class_t;

public:
	setup_class_t 	setup_information;

	Mat	selected_frames;
	Mat camera_matrix;
	Mat distortion_coefficient;

	int frame_counter;
	int total_frame_counter;
	int selection_frame_step;
	double processed_frames;
	double lost_frames;
	double calibration_video_frames;
	double benchmark_video_frames;
	double maximun_distance;
	double maximun_pattern_distance; // include in execute parameters to delete

	vector< vector<Point2f> > imagePoints;

	VideoCapture* calibration_video;
	VideoCapture* benchmark_video;
	Point2f new_centroid; // include in execute parameters to delete
	Point2f actual_centroid; // include in execute parameters to delete
	string video_name;
	Size image_size;

	//vector<Point2f> generated_pattern;
	vector< pair <Point2f, vector< FramePattern > > > selection_frame_mesh;
	//vector<Point2f> esquinasPatron;

	PatternPoint<T_DOUBLE> boxElegidos;
	PatternPoint<T_DOUBLE> boxFinal;

	PreProcessing preProcessing;
	ContourFinder contourFinder;
	EllipseFinder ellipseFinder;
	//CentersAndConvexFinder<T_DOUBLE> *centersAndConvexFinder;
	PatternFinder<T_DOUBLE> patternFinder;
	OpencvCalibration opencvCalib;

	Camera camParameters;

public:
	App(setup_class_t&);
	~App(void);

	long GetNumberOfFrames(VideoCapture*);
	void init(void);
	void CreateVideo(int);
	void AnkurDatta(void);
	void FindDistances(void);
	void SelectPointsToCalibrate();
	void SaveBoardPoints(bool Ankur=false);
	void ourCalibrateCamera(string, bool ankur=false);
	double CalcularDistancia(Mat, vector<Point3f> &, vector<Point2f> &, int);
	bool execute(Mat&, int, int, bool thr=false);
	void selectFrame(Mat&, string &,string &, string &, vector<Point2f> &);
	void selectFrame(string &, string &, vector<Point2f> &);
	void saveDistances(string &, int, double);
	void ClearFile(string);
	static void calcBoardCornerPositions(Size, float, vector<Point3f>&, Size, int);
	vector<Point3f> GenerarObjectPoint(int, int, int, int, Size);
	vector<vector<Point3f> > GenerarObjectPoints(vector<Point3f>, int);
	vector<Point2f> GeneratePointsForFrontoParallel(int, int, int, Size);
	Mat DibujarNubePuntos(vector<vector<Point2f> >, Size);
	Mat DibujarNubePuntosArrowed(vector<vector<Point2f> >);
	vector< vector<cv::Point2f> > GenerarImagePoints(string, string, int, bool ankur=false);
	void GetCoordenadas(char[], Point2f&);
	void distortPoints(std::vector<cv::Point2f>, std::vector<cv::Point2f> &, cv::Mat, cv::Mat);
	double computeReprojectionErrors(vector<vector<Point3f> >&, vector<vector<Point2f> >&, vector<Mat>&, vector<Mat>&, Mat&, Mat&, vector<float>&, bool fisheye = false);

};

#include "App.cpp"

