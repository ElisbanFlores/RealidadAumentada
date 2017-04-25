#pragma once

#include "Common.h"
#include "h_Matriz.h"

template<class T>
class PatternPoint
{
public:
	//vector< RotatedRect > boxes;
	vector<Point2f> points;
	vector<Size> radios;
	T_INT nPoints;
	h_Matriz<T> distances;
	h_Matriz<T> minDistances;
	T maxRadio;
	T maxDistance;
	Point2f centroid;

	PatternPoint();
	PatternPoint(T_INT);
	PatternPoint(vector< RotatedRect >);
	PatternPoint(vector<Point2f>);	
	~PatternPoint();
	void Init(T_INT);
	void Calculate();
	Point2f FindCentroid();
	Point2f FindCentroid(vector<Point2f>);

	PatternPoint& PatternPoint::operator = (const PatternPoint &pc)
	{
		if(this != &pc)
		{			
			this->points = pc.points;
			this->radios = pc.radios;
			this->Init(points.size());
			this->distances.Set_Matriz(pc.distances);
			this->minDistances.Set_Matriz(pc.minDistances);
			this->maxRadio = pc.maxRadio;
			this->maxDistance = pc.maxDistance;
			this->centroid = pc.centroid;
		}
		return *this;
	}

	void Clear()
	{
		points.clear();
		radios.clear();
		nPoints=0;
		maxRadio=0.0;
		maxDistance=0.0;
		centroid=Point2f(0,0);	
	}
};

#include "PatternPoint.cpp"