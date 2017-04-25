#ifndef PATTERNFINDER_CPP
#define PATTERNFINDER_CPP

#include "PatternFinder.h"

template<class T>
PatternFinder<T>::~PatternFinder() { 
}

template<class T>
void PatternFinder<T>::Init(T_INT _m, T_INT _n, T_INT type) {
	m = _m;
	n = _n;
	patternType = type;
}

template<class T>
void PatternFinder<T>::Init(T_INT n)
{
	nPoints = n;
	distances.Init(n, n);
	minDistances.Init(n, 1);
	maxRadio = 0;
	maxDistance = 0;
	if (radios.size() == 0)
		for (T_INT i = 0; i < nPoints; i++)
			radios.push_back(Size(0, 0));
}

template<class T>
void PatternFinder<T>::SetPatternFinder(PatternPoint &pc)
{
	points = pc.points;
	radios = pc.radios;
	Init(points.size());
	distances.Set_Matriz(pc.distances);
	minDistances.Set_Matriz(pc.minDistances);
	maxRadio = pc.maxRadio;
	maxDistance = pc.maxDistance;
	centroid = pc.centroid;
}

template<class T>
vector<Point2f> PatternFinder<T>::FitPatternToSimetricConvexHull()
{
	//tengo que buscar las esquinas 
	T_DOUBLE ma, mb, theta;
	Point2f	p0, p1, p2;
	//vector<Point2f> hullCorners;//, esquinasPatron;
	hullCorners.points.clear();
	patternCorners.points.clear();
	// se obtiene las esquinas de los bordes, con sus respectivos angulos
	T_INT contAng = 0;
	for (T_INT i = 0; i < hull.size(); i++)
	{
		if (i == 0)					p1 = hullPoints[hull[hull.size() - 1]];	else p1 = hullPoints[hull[i - 1]];
		if (i == hull.size() - 1)	p2 = hullPoints[hull[0]];				else p2 = hullPoints[hull[i + 1]];
		
		p0 = hullPoints[hull[i]];

		ma = (p1.y - p0.y) / (p1.x - p0.x);
		mb = (p2.y - p0.y) / (p2.x - p0.x);
		theta = abs(atanf((ma - mb) / (1 + ma*mb)));
		theta *= 180;
		if (theta > 60)	
			hullCorners.points.push_back(p0);		
	}
	
	// recupero las esquinas del patron generado del paso anterior
	patternCorners.points.push_back(patternGenerated.points[0]);
	patternCorners.points.push_back(patternGenerated.points[n - 1]);
	patternCorners.points.push_back(patternGenerated.points[m*n - 1]);
	patternCorners.points.push_back(patternGenerated.points[(m - 1)*n]);

	//aca debo buscar las esquinas mas cercanas del hull a las esquinas del patron
	T_DOUBLE dist, disttemp;
	T_INT pivote;
	for (T_INT i = 0; i < patternCorners.points.size(); i++) {
		dist = 10000;
		for (T_INT j = 0; j < hullCorners.points.size(); j++) {
			disttemp = norm(patternCorners.points[i] - hullCorners.points[j]);
			if (disttemp < dist) {
				dist = disttemp;
				pivote = j;
			}
		}
		patternCorners.points[i] = hullCorners.points[pivote];
	}
	
	//Mat esquina(Size(1280, 720), CV_64F);
	//for (int i = 0; i < patternCorners.points.size(); i++)
	//	circle(esquina, patternCorners.points[i], 5, Scalar(255, 0, 255), 1);
	//imshow("esquinas detectadas", esquina);
	
	// se vuelve a generar el patron para que se reajuste a su nueva posicion	
	patternGenerated.points = GeneratePattern(m, n, patternCorners.points[0], patternCorners.points[1], patternCorners.points[2], patternCorners.points[3], PATTERN::SIMETRICO);	

	return patternGenerated.points;
}

template<class T>
vector<Point2f> PatternFinder<T>::FitPatternToAsimetricConvexHull()
{	
	hullCorners.points.clear();
	//tengo que buscar las esquinas 

	T_DOUBLE ma, mb, theta;
	Point2f	p0, p1, p2, p3, p4;
	vector<Point2f> esquinasFinal;
	vector<T_DOUBLE> angles;

	// se obtiene las esquinas de los bordes, con sus respectivos angles, en este caso son seis hullPoints
	T_INT contAng = 0;

	for (T_INT i = 0; i < hull.size(); i++)
	{
		if (i == 0)					p1 = hullPoints[hull[hull.size() - 1]];	else	p1 = hullPoints[hull[i - 1]];
		if (i == hull.size() - 1)	p2 = hullPoints[hull[0]];				else	p2 = hullPoints[hull[i + 1]];
		p0 = hullPoints[hull[i]];

		ma = (p1.y - p0.y) / (p1.x - p0.x);
		mb = (p2.y - p0.y) / (p2.x - p0.x);
		theta = abs(atanf((ma - mb) / (1 + ma*mb)));
		theta *= 180;
		if (theta > 10) {
			angles.push_back(theta); // con esto solo obtengo las esquinas de la figura
			hullCorners.points.push_back(p0); // y tambien las coordenadas			
		}
	}
	
	//hallo distancias entre puntos
	
	// Elijo solo los hullPoints que corresponden solo a las cuatro esquinas deseadas, y el oreden de las esquinas
	vector<T_INT> indesquina;
		
	vector<Point2f> pointsTmp;
	vector<T_DOUBLE> distTmp;
	T_DOUBLE dMin, dTmp;
	T_INT ia2, ia, is, is2;
	T_INT tam = hullCorners.points.size();

	// esto es con ditancias
	for (T_INT i = 0; i < tam; i++)
	{
		pointsTmp.clear();
		distTmp.clear();
		
		ia2 = i - 2;
		ia = i - 1;

		if (i == 0){
			ia2 = tam - 2;
			ia = tam - 1;
		}
		else{
			if (i == 1){
				ia2 = tam - 1;
				ia = i - 1;
			}
		}

		is = i + 1;
		is2 = i + 2;

		if (i == tam - 2){
			is = i + 1;
			is2 = 0;
		}
		else {
			if (i == tam - 1) {
				is = 0;
				is2 = 1;
			}		
		}

		pointsTmp.push_back(hullCorners.points[ia2]);
		pointsTmp.push_back(hullCorners.points[ia]);
		pointsTmp.push_back(hullCorners.points[i]);
		pointsTmp.push_back(hullCorners.points[is]);
		pointsTmp.push_back(hullCorners.points[is2]);
		dMin = 10000;
		for (T_INT j = 0;j < pointsTmp.size()-1;j++)
		{			
			dTmp = norm(pointsTmp[j] - pointsTmp[j + 1]);
			distTmp.push_back(dTmp);
			if (dTmp < dMin)
				dMin = dTmp;
		}

		bool seleccionado = false;
		dMin = dMin*1.50;
		
		if (distTmp[0] > dMin && distTmp[1] > dMin && distTmp[2] < dMin && distTmp[3] > dMin){
			seleccionado = true;
			indesquina.push_back(0);
		}
		if (distTmp[0] > dMin && distTmp[1] < dMin && distTmp[2] > dMin && distTmp[3] > dMin) {
			seleccionado = true;
			indesquina.push_back(3);
		}
		if (distTmp[0] < dMin && distTmp[1] > dMin && distTmp[2] > dMin && distTmp[3] > dMin) {
			seleccionado = true;
			indesquina.push_back(2);
		}
		if (distTmp[0] > dMin && distTmp[1] > dMin && distTmp[2] > dMin && distTmp[3] < dMin) {
			seleccionado = true;
			indesquina.push_back(1);
		}
		
		if (seleccionado){
			esquinasFinal.push_back(hullCorners.points[i]); // insetar los valores que correspondas
			patternCorners.points.push_back(hullCorners.points[i]);// para inicializar valores
		}
	}

	// esto es con angulos
	/*
	
	T_DOUBLE ang, ang_ant, ang_sig;
	for (T_INT i = 0; i < angles.size(); i++)
	{
		if (i == 0)
			ang_ant = angles[angles.size() - 1];
		else
			ang_ant = angles[i - 1];

		if (i == angles.size() - 1)
			ang_sig = angles[0];
		else
			ang_sig = angles[i + 1];

		ang = angles[i];
		T_DOUBLE angUmbral = 220;
		if (ang_ant > angUmbral || ang_sig > angUmbral)	{
			esquinasFinal.push_back(hullCorners.points[i]); // insetar los valores que correspondas
			patternCorners.points.push_back(hullCorners.points[i]);// para inicializar valores
			if (ang_ant > angUmbral && ang_sig < angUmbral && ang >angUmbral)
				indesquina.push_back(1);
			if (ang_ant > angUmbral && ang_sig < angUmbral && ang <angUmbral)
				indesquina.push_back(0);
			if (ang_ant < angUmbral && ang_sig > angUmbral && ang <angUmbral)
				indesquina.push_back(3);
			if (ang_ant < angUmbral && ang_sig > angUmbral && ang >angUmbral)
				indesquina.push_back(2);
		}
	}*/

	//Mat esquina(Size(640, 360), CV_64F);
	//for (int i = 0;i < esquinasFinal.size();i++)
	//	circle(esquina, esquinasFinal[i], 5, Scalar(255, 0, 255), 1);
	//imshow("esquinas detectadas", esquina);
	
	if (esquinasFinal.size() == 4){
		
		for (T_INT i = 0; i < esquinasFinal.size(); i++)
			patternCorners.points[i] = esquinasFinal[indesquina[i]];		
		patternGenerated = GeneratePattern(m, n, patternCorners.points[0], patternCorners.points[1], patternCorners.points[2], patternCorners.points[3], PATTERN::ASIMETRICO);
	}
	return patternGenerated.points;
}

template<class T>
vector<Point2f> PatternFinder<T>::FitPatternToConvexHull(){
	if (patternType == PATTERN::SIMETRICO)
		return FitPatternToSimetricConvexHull();
	return FitPatternToAsimetricConvexHull();
}

template<class T>
vector<Point2f> PatternFinder<T>::GeneratePattern(T_INT _m, T_INT _n, Point2f p1, Point2f p2, Point2f p3, Point2f p4, T_INT tipo){
	Point2f pt1, pt2, p;
	vector<Point2f> puntos;

	for (T_INT i = 0; i < _m*_n; i++)// inicializar los puntos
		puntos.push_back(p1);
	//deformar los puntos
	T_DOUBLE disx_x, disx_y, disy1_x, disy1_y, disy2_x, disy2_y;

	if (tipo == PATTERN::ASIMETRICO) {
		T_INT t = _m;
		_m = _n;
		_n = t;
	}

	// Insertar los cuatro puntos
	puntos[0] = p1;
	puntos[_n - 1] = p2;
	puntos[_m*_n - 1] = p3;
	puntos[(_m - 1)*_n] = p4;

	disy1_x = (p4.x - p1.x) / (_m - 1);
	disy1_y = (p4.y - p1.y) / (_m - 1);

	disy2_x = (p3.x - p2.x) / (_m - 1);
	disy2_y = (p3.y - p2.y) / (_m - 1);

	T_INT pas;

	for (T_INT i = 0; i < _m; i++)
	{
		if (i > 0) {
			puntos[i*_n].x = puntos[(i - 1)*_n].x + disy1_x;
			puntos[i*_n].y = puntos[(i - 1)*_n].y + disy1_y;
			puntos[i*_n + _n - 1].x = puntos[(i - 1)*_n + _n - 1].x + disy2_x;
			puntos[i*_n + _n - 1].y = puntos[(i - 1)*_n + _n - 1].y + disy2_y;
			pt1 = puntos[i*_n];
			pt2 = puntos[i*_n + _n - 1];
		}
		else {
			pt1 = puntos[0];
			pt2 = puntos[_n - 1];
		}

		disx_x = (pt2.x - pt1.x) / (_n - 1);
		disx_y = (pt2.y - pt1.y) / (_n - 1);
		if (i > 0) {
			if (tipo == PATTERN::ASIMETRICO && i % 2 == 1) {
				puntos[i*_n].x -= disx_x / 2;
				puntos[i*_n].y -= disx_y / 2;
				puntos[i*_n + _n - 1].x -= disx_x / 2;
				puntos[i*_n + _n - 1].y -= disx_y / 2;
				pt1 = puntos[i*_n];
				pt2 = puntos[i*_n + _n - 1];
			}
			else
				if (tipo == PATTERN::ASIMETRICO && i % 2 == 0) {
					puntos[i*_n].x += disx_x / 2;
					puntos[i*_n].y += disx_y / 2;
					puntos[i*_n + _n - 1].x += disx_x / 2;
					puntos[i*_n + _n - 1].y += disx_y / 2;
					pt1 = puntos[i*_n];
					pt2 = puntos[i*_n + _n - 1];
				}
		}

		for (T_INT j = 1; j < _n - 1; j++) {
			p.x = pt1.x + j*disx_x;
			p.y = pt1.y + j*disx_y;
			puntos[i*_n + j] = p;
		}
	}
	return puntos;
}

template<class T>
vector<Point2f> PatternFinder<T>::MovePattern(vector<Point2f> pts, Point2f actualCenter, Point2f newCenter){
	T_DOUBLE dx = newCenter.x - actualCenter.x;
	T_DOUBLE dy = newCenter.y - actualCenter.y;
	for (T_INT i = 0; i < pts.size(); i++) {
		pts[i].x += dx;
		pts[i].y += dy;
	}	
	return pts;
}

template<class T>
Mat PatternFinder<T>::DrawPattern(Mat img, T_INT _m, T_INT _n,vector<Point2f> points, Scalar color[], T_INT type)
{	
	if (type == ASIMETRICO) {
		T_INT t = _m;
		_m = _n;
		_n = t;
	}

	for (T_INT i = 0; i < _m; i++) {
		T_INT j;
		for (j = 0; j < _n-1; j++) {
			circle(img, points[i*_n + j], 2, color[i], 1, 8);
			line(img, points[i*_n + j], points[i*_n + j + 1], color[i], 1, 8);
		}
		circle(img, points[i*_n + j], 2, color[i], 1, 8);		
		if (i < _m - 1)
			line(img, points[i*_n + j], points[(i + 1)*_n], color[i], 1, 8);
	}
	return img;
}

template<class T>
Mat PatternFinder<T>::DrawConvexHull(Mat img)
{
	for (int i = 0; i < hull.size(); i++)
		circle(img, hullPoints[hull[i]], 3, Scalar(125, 14, 136), 1, 8);
	return img;
}

template<class T>
void PatternFinder<T>::FindPattern(cv::Mat in, cv::Mat& out, int &frame_counter, double &frames_perdidos) {

	in.copyTo(out);
	if (frame_counter == 1)// solo en el primer frame
	{
		//Definir la plantilla inicial
		Point2f p1, p2, p3, p4;

		T_DOUBLE xx, yy;

		if (patternType == PATTERN::SIMETRICO) {
			xx = (maxRadio + maxRadio / 2)*(n / 2);
			yy = (maxRadio + maxRadio / 2)*(m / 2);
		}
		else { // en caso ASIMETRICO					
			xx = (maxRadio + maxRadio / 3)*(n / 2);
			yy = (maxRadio + maxRadio)*(m / 2);
		}

		p1.x = centroid.x - xx;	p1.y = centroid.y - yy;
		p2.x = centroid.x + xx;	p2.y = centroid.y - yy;
		p3.x = centroid.x + xx;	p3.y = centroid.y + yy;
		p4.x = centroid.x - xx;	p4.y = centroid.y + yy;

		if (patternType == PATTERN::SIMETRICO)
			patternGenerated.points = GeneratePattern(m, n, p1, p2, p3, p4, patternType);
		else// en caso ASIMETRICO
			patternGenerated.points = GeneratePattern(m, n, p4, p1, p2, p3, patternType);

		patternGenerated.points = FitPatternToConvexHull();
		patternGenerated.Init(patternGenerated.points.size()); //se inicializa el patron generado para gacer calculos
	}
	else
	{
		Point2f ca, cn;
		patternGenerated.FindCentroid();
		//cout << "Centroide..." << patternGenerated.centroid << endl;
		ca = patternGenerated.centroid;
		cn = centroid;

		patternGenerated.points = MovePattern(patternGenerated.points, ca, cn);
		
		//cout <<"moviendo de: "<<ca << " a: "<< cn<<endl;		
		//cout << "todavio esta bien" << endl;
		// Para recalcular el patron

		if (abs(ca.x - cn.x) > 30 || abs(ca.y - cn.y) > 30 || frames_perdidos>1)// en caso que los centroides esten muy alejados, seria bueno reajustar las dimensiones del patron generado
		{
			//std::cout << "no encaja bien, hay que reajustar..." << std::endl<<std::endl<<std::endl;
			patternGenerated.points = FitPatternToConvexHull();
		}
		//cout << "yeeeeeee...." << endl;
		patternGenerated.Init(patternGenerated.points.size()); //se inicializa el patron generado para hacer calculos
	}

	// el patron movido a su nuevo centroide
	//out = DrawPattern(out, patternGenerated.points, colorTab2);
	//imshow("Patron generado", out);
	//waitKey();

	// aca se deberia ajustar el patron, lo que se hace es acercar el punto mas cercano al centro mas ceercano del patron generado
	T_DOUBLE dtmp, dmin;
	T_INT i,j,it,jt;

	for (T_INT i = 0; i < points.size(); i++) {
		dmin = 100000;
		for (T_INT j = 0; j < patternGenerated.points.size(); j++) {
			// compara las distancias entre los puntos mas cercanos
			dtmp = norm(points[i] - patternGenerated.points[j]);
			if (dtmp < dmin) {
				dmin = dtmp;
				jt = j;
			}
		}		
		patternGenerated.points[jt] = points[i];
	}
	
	//Patron ajustado en su nueva posicion
	out = DrawPattern(out,m,n, patternGenerated.points, colorTab, patternType);
	//imshow("Patron final", out);
	//waitKey();
}


#endif // !PATTERNFINDER_CPP
