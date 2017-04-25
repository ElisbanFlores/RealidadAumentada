#ifndef CENTERSANDCONVEXFINDER_CPP
#define CENTERSANDCONVEXFINDER_CPP

#include "CentersAndConvexFinder.h"

template<class T>
void CentersAndConvexFinder<T>::FindCentersAndConvex(cv::Mat& in, cv::Mat& out) {

	hull.clear();
	hullPoints.clear();
	in.copyTo(out);
	
	// Dibujar los circulos de contorno de los puntos seleccionados con  convex Hull
	for (int i = 0; i < nPoints; i++){
		//circle(out, points[i], 2, Scalar(0, 125, 255), 2, 8);
		hullPoints.push_back(points[i]);
	}

	convexHull(hullPoints, hull, true);
	
	// Dibujar el contorno del Hull en color verde
	//for (int i = 0; i < hull.size() - 1; i++)
	//	line(out, hullPoints[hull[i]], hullPoints[hull[i + 1]], Scalar(0, 255, 0), 1, 8);
	//line(out, hullPoints[hull[0]], hullPoints[hull[hull.size() - 1]], Scalar(0, 255, 0), 1, 8);

	// Hallo las distancias entre todos los puntos finales		
	//Calculate();

	// Dibujar el centroide
	//circle(out, centroid, 2, Scalar(255, 255, 0), 3, 8);
}


#endif // !CENTERSANDCONVEXFINDER_CPP

