#ifndef PATTERNPOINT_CPP
#define PATTERNPOINT_CPP

#include "PatternPoint.h"

template <class T>
PatternPoint<T>::PatternPoint()
{
}

template <class T>
PatternPoint<T>::PatternPoint(T_INT n)
{
	Init(n);
}

template <class T>
PatternPoint<T>::PatternPoint(vector< RotatedRect > box)
{
	Point2f point;
	Size radio;

	for (int i = 0; i < box.size(); i++) {
		point = box[i].center;
		radio = box[i].size;
		points.push_back(point);
		radios.push_back(radio);
	}
	Init(points.size());
}

template <class T>
PatternPoint<T>::PatternPoint(vector<Point2f> p)
{
	points = p;	
	Init(points.size());
}


template <class T>
void PatternPoint<T>::Init(T_INT n)
{	
	nPoints=n;
	distances.Init(n,n);
	minDistances.Init(n,1);
	maxRadio=0;
	maxDistance=0;
	if (radios.size() == 0)
		for (T_INT i = 0; i < nPoints; i++)
			radios.push_back(Size(0, 0));
}

// Calcular distancias de todos los puntos
template <class T>
void PatternPoint<T>::Calculate()
{
	Point2f p1, p2;	
	T_DOUBLE	dmin = 10000,// variable para la distancia minima, las distancias siempre son positivas
				dist1, dist2;
	maxRadio = 0;
	maxDistance = 0;

	FindCentroid();
	vector<float> maxRadios;

	for (T_INT i = 0; i < nPoints; i++){		
		dist2 = norm(centroid - points[i]);
		maxRadios.push_back(MAX(radios[i].width, radios[i].height));

		if (dist2 > maxDistance)
			maxDistance = dist2;				
		dmin = 10000;		
		for (T_INT j = i; j < nPoints; j++){
			if (i == j)
				distances.Set(i, j, 0.0);
			else{
				p1 = points[i];
				p2 = points[j];
				dist1 = norm(p1 - p2);
				distances.Set(i, j, dist1);
				distances.Set(j, i, dist1);
				if (dist1 < dmin)
					dmin = dist1;
			}
		}		
		minDistances.Set(i, 0, dmin);
	}	
	
	// Para hallar el max radio en base a una muestreo de las esferas mas grandes
	std::sort(maxRadios.begin(), maxRadios.end());
	float cont = 0;
	for (T_INT i = nPoints-1; i >= nPoints*0.75; i--) {
		maxRadio += maxRadios[i];
		cont++;
	}	
	maxRadio /= (cont); // es el grosor promedio de las esferas elegidas, las mas grandes
}

template <class T>
Point2f PatternPoint<T>::FindCentroid()
{	
	T_DOUBLE mediax = 0, mediay = 0;
	mediax = 0, mediay = 0;
	for (T_INT i = 0; i < nPoints; i++){
		centroid = points[i];
		mediax += centroid.x;
		mediay += centroid.y;
	}
	mediax /= nPoints;
	mediay /= nPoints;
	centroid.x = mediax;
	centroid.y = mediay;
	return centroid;
}

template <class T>
Point2f PatternPoint<T>::FindCentroid(vector<Point2f> pts)
{	
	Point2f ctr;
	T_DOUBLE mediax = 0, mediay = 0;
	mediax = 0, mediay = 0;
	for (T_INT i = 0; i < pts.size(); i++) {
		ctr = pts[i];
		mediax += ctr.x;
		mediay += ctr.y;
	}
	mediax /= pts.size();
	mediay /= pts.size();
	ctr.x = mediax;
	ctr.y = mediay;
	return ctr;
}



template <class T>
PatternPoint<T>::~PatternPoint()
{
}

#endif // !PATTERNPOINT_CPP