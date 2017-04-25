#include "PatternTracker.h"

PatternTracker::PatternTracker()
{
	width = 640;
	height = 480;
	m = 5;
	n = 6;
	nPoints = m*n;
	frame_counter = 1;
	lost_frames = 0.0;
	maximun_distance = 0.0;
	maximun_pattern_distance = 0.0;
	boardPoint = GenerarObjectPoint(m, n, 2.55, SIMETRICO, Size(width, height));
}

PatternTracker::PatternTracker(int w, int h, int _m, int _n, int dist)
{
		
	width = w;
	height = h;
	m = _m;
	n = _n;
	nPoints = m*n;
	frame_counter = 1;
	lost_frames = 0.0;
	maximun_distance = 0.0;
	maximun_pattern_distance = 0.0;
	patternFinder.Init(m, n, SIMETRICO);
	boardPoint = GenerarObjectPoint(m, n, dist, SIMETRICO, Size(width, height));
}

PatternTracker::~PatternTracker()
{
}

vector<Point3f> PatternTracker::GenerarObjectPoint(int m, int n, int distance_between_points, int pattern_type, Size imgSize) {
	Point2f p1, p2, p3, p4, p5;
	float xx, yy;
	xx = (distance_between_points * (n - 1));

	if (pattern_type == ASIMETRICO)
		yy = (distance_between_points*(m - 1) * 2);
	else
		yy = (distance_between_points*(m - 1));

	p1.x = 0; p1.y = 0;
	p2.x = xx; p2.y = 0;
	p3.x = xx; p3.y = yy;
	p4.x = 0; p4.y = yy;

	vector<Point2f> tempPoints;

	PatternFinder<T_DOUBLE> patternFinder;
	if (pattern_type == ASIMETRICO)
		tempPoints = patternFinder.GeneratePattern(m, n, p4, p1, p2, p3, pattern_type);
	else
		tempPoints = patternFinder.GeneratePattern(m, n, p1, p2, p3, p4, pattern_type);

	vector<Point3f > puntostmp;
	Point3f punto;
	int tam = tempPoints.size();

	for (int i = 0; i < tam; i++) {
		punto.x = tempPoints[i].x;
		punto.y = tempPoints[i].y;
		punto.z = 0.0;
		puntostmp.push_back(punto);
	}
	return puntostmp;
}

bool PatternTracker::execute(Mat& in) {
	bool found = false;
	Mat pre, edge, cimage;

	preProcessing.PreProcess(in, pre, true);
	ellipseFinder.FindContours(pre, edge);
	bool ellipsesThresh = ellipseFinder.FindEllipse(edge, cimage, frame_counter, new_centroid, maximun_pattern_distance, SIMETRICO, nPoints);

	//imshow("edge", edge);
	//imshow("ellipse Finder", cimage);

	if (ellipsesThresh == true) {
		boxFinal = ellipseFinder.finalPoints;
		boxFinal.Calculate();

		nCircles = boxFinal.points.size();

		if (boxFinal.points.size() == nPoints) {

			Mat tmp;

			patternFinder.SetPatternFinder(boxFinal);
			patternFinder.FindCentersAndConvex(cimage, tmp);
			//imshow("Centros y convex", tmp);

			maximun_distance = patternFinder.maxRadio;
			maximun_pattern_distance = patternFinder.maxDistance;
			new_centroid = patternFinder.centroid;

			patternFinder.FindPattern(in, cimage, frame_counter, lost_frames);
			// showing pattern
			//imshow("Pattern", cimage);
			frame_counter++;
			lost_frames = 0;
			found = true;
		}
		else {
			
			boxFinal.Clear();
			found = false;
		}
	}
	else {
		found = false;
		lost_frames++;
		/*PatternPoint<T_DOUBLE> _distancias(boxFinal.points);
		new_centroid = _distancias.FindCentroid();
		cout << "Recalculating centroid: " << new_centroid << endl;*/
		for (int i = 0; i < ellipseFinder.initialPoints.nPoints; i++)
			circle(in, ellipseFinder.initialPoints.points[i], 2, Scalar(0, 125, 255), 2, 8);
		
		nCircles = ellipseFinder.initialPoints.nPoints;
		
		//imshow("Pattern", in);
		//waitKey();
	}
	

	return found;
}

double PatternTracker::GetRotTra(Mat imgframe, Mat camera_matrix, Mat distortion_coefficient, Mat &tvec, Mat &rvec, Mat &rvecRodrigues) {
	bool found = execute(imgframe);
	if (found) {
		foundBoardPoint = patternFinder.patternGenerated.points;
		solvePnP(boardPoint, foundBoardPoint, camera_matrix, distortion_coefficient, rvec, tvec, false, SOLVEPNP_ITERATIVE);
		Rodrigues(rvec, rvecRodrigues);
		return norm(-rvecRodrigues.t() * tvec);
	}
	return -1.0;
}