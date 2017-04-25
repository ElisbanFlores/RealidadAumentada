#include "EllipseFinder.h"

EllipseFinder::~EllipseFinder() {
}

//esta parte es de acuerdo a la forma del contorno
bool EllipseFinder::VerifyContourShape(vector<Point> contour)
{
	double dist1, dist2;
	Point2f pini,
		pfin;
	size_t count = contour.size();

	pini = contour[count*0.25];	pfin = contour[count*0.75];
	dist1 = norm(pini - pfin);

	pini = contour[0];	pfin = contour[count*0.5];
	dist2 = norm(pini - pfin);

	if (dist1 < 4 || dist2 < 4)
		return false; // se descarta
	return true; // se procesa contorno
}

// ajustar contornos a elipses
RotatedRect EllipseFinder::FitContourToEllipse(vector<Point> contour)
{
	Mat pointsf;
	Mat(contour).convertTo(pointsf, CV_32F);
	return fitEllipse(pointsf);
}

//Verifica que la elipse detectada no sea muy achatada
bool EllipseFinder::VerifyEllipseNoEnlarged(RotatedRect box)
{
	if (MIN(box.size.width, box.size.height) / MAX(box.size.width, box.size.height) < 0.25)
		return false; // se descarta
	return true;// se procesa contorno
}


// Verifica si se un contorno se asemeja a una elipse
/*bool  EllipseFinder::VerifyEllipseShaped(RotatedRect box, Mat &cimage)
{
	// se buscara ocho putnos d ela elipse para verificar la forma
	Point2f vtx[8];
	Point2f vtxq[8];
	int  v;

	double h = box.center.x;
	double k = box.center.y;
	double theta = box.angle;
	double a = MAX(box.size.width, box.size.height)/2;
	double b = MIN(box.size.width, box.size.height) / 2;
	
	//for()



	for (v = 0; v < 3; v++)
	{
		vtxq[v].x = (vtx[v].x + vtx[v + 1].x) / 2;
		vtxq[v].y = (vtx[v].y + vtx[v + 1].y) / 2;

		circle(cimage, vtxq[v], 3, Scalar(124, 255, 255));
	}
	vtxq[v].x = (vtx[0].x + vtx[v].x) / 2;
	vtxq[v].y = (vtx[0].y + vtx[v].y) / 2;
	circle(cimage, vtxq[v], 3, Scalar(124, 255, 255));

	//verificar si alguno de los puntos se intersectan con el rectangulo
	int  nc = 3, // se busca en una grilla 3 x 3
		fnc, // badera que indica que encontro un punto blanco
		cn = 0,
		cm = nc / 2;
	for (register int j = 0; j < 4; j++)
	{
		//buscar un punto blanco en el contorno del punto que intersecta
		fnc = 0;

		if (vtxq[j].y - cm >= 0 &&
			vtxq[j].y + cm < cimage.rows &&
			vtxq[j].x - cm >= 0 &&
			vtxq[j].x + cm < cimage.cols) // comprobar qu esta dentro dela imagen

		{
			for (int k = 0; k < nc; k++)
				for (int l = 0; l < nc; l++)
					if (cimage.at<Vec3b>(vtxq[j].y + k - cm, vtxq[j].x + l - cm)[0] == 255) {
						fnc = 1;
						goto AQUI;
					}
		AQUI:
			if (fnc)
				cn++;
		}
		else
			break;
	}
	ellipse(cimage, box, Scalar(255, 0, 125));

	imshow("contornito", cimage);

	if (cn < 4)
		return false; // se desctarta
	else // se verifica que no sea muy peque;na
		if ((box.size.width + box.size.height) / 4 < 2) return false;
	return true;
}
*/
// Verifica si se un contorno se asemeja a una elipse
bool  EllipseFinder::VerifyEllipseShaped(RotatedRect box, Mat &cimage)
{
	Point2f vtx[4];
	box.points(vtx);
	
	Point2f vtxq[4];
	int  v;


	for (v = 0; v < 3; v++)
	{
		vtxq[v].x = (vtx[v].x + vtx[v + 1].x) / 2;
		vtxq[v].y = (vtx[v].y + vtx[v + 1].y) / 2;
	}
	vtxq[v].x = (vtx[0].x + vtx[v].x) / 2;
	vtxq[v].y = (vtx[0].y + vtx[v].y) / 2;

	//verificar si alguno de los puntos se intersectan con el rectangulo
	int  nc = 5, // se busca en una grilla 3 x 3
		fnc, // badera que indica que encontro un punto blanco
		cn = 0,
		cm = nc / 2;
	for (register int j = 0; j < 4; j++)
	{
		//buscar un punto blanco en el contorno del punto que intersecta
		fnc = 0;

		if (vtxq[j].y - cm >= 0 &&
			vtxq[j].y + cm < cimage.rows &&
			vtxq[j].x - cm >= 0 &&
			vtxq[j].x + cm < cimage.cols) // comprobar qu esta dentro dela imagen

		{
			for (int k = 0; k < nc; k++)
				for (int l = 0; l < nc; l++)
					if (cimage.at<Vec3b>(vtxq[j].y + k - cm, vtxq[j].x + l - cm)[0] == 255)	{
						fnc = 1;
						goto AQUI;
					}
		AQUI:
			if (fnc)
				cn++;
		}
		else
			break;		
	}

	if (cn < 4)
		return false; // se desctarta
	else // se verifica que no sea muy peque;na
		if ((box.size.width + box.size.height) / 4 < 2) return false;
	return true;
}

PatternPoint<T_DOUBLE> EllipseFinder::ElegirElipsesFinales(int nro_puntos)
{
	return ElegirElipsesFinalesSimetrico(nro_puntos);
}


PatternPoint<T_DOUBLE> EllipseFinder::ElegirElipsesFinalesSimetrico(int nro_puntos)//vector< RotatedRect > boxElegidos, h_Matriz<double> distancias, T_DOUBLE &dmax)
{
	// 15 ms aproximadamente algo muy pesado

	// Primero elimino las esferas concentricas
	//Aca se reducen las esferas concentricas 
	int varCenter = 3;
	PatternPoint<T_DOUBLE> box;
	double distance = 0, x, y, cont;
	Size r;

	//Mat imInitPoints(Size(1280, 720), CV_64FC3);
	//for (int i = 0; i<initialPoints.nPoints; i++)
	//	circle(imInitPoints, initialPoints.points[i], (initialPoints.radios[i].height + initialPoints.radios[i].height) / 4, colorTab2[i%20], 1);
	//imshow("Puntos Iniciales", imInitPoints);
	

	// solo se seleccionan los que tienen concentricos en los demas casos no funciona

	finalPoints.Clear();

	int i, j;

	for (i = 0; i < initialPoints.nPoints; ++i) {
		Point2f nuevo = initialPoints.points[i];
		x = 0;
		y = 0;
		r = initialPoints.radios[i];
		cont = 1;
		for (j = 0; j < initialPoints.nPoints; ++j) {
			if (i != j) {
				Point2f temp = initialPoints.points[j];
				Size rtemp = initialPoints.radios[j];
				distance = norm(nuevo - temp);
				if (distance <= varCenter &&  temp.x != 0) {
					x += temp.x; y += temp.y;
					initialPoints.points[j] = Point2f(0, 0);
					if (rtemp.height > r.height) r.height = rtemp.height;
					if (rtemp.width > r.width) r.width = rtemp.width;
					cont++;
				}
			}
		}
		if (cont >3) {
			x += initialPoints.points[i].x;
			y += initialPoints.points[i].y;
			//cout <<"x: "<< x/cont <<" y: "<< y/cont <<" cont: "<< cont<<endl;
			initialPoints.points[i].x = x / cont;
			initialPoints.points[i].y = y / cont;
			initialPoints.radios[i] = r;
			finalPoints.points.push_back(initialPoints.points[i]);
			finalPoints.radios.push_back(initialPoints.radios[i]);
		}
	}	
	
	finalPoints.Init(finalPoints.points.size());
	finalPoints.Calculate();
	initialPoints = finalPoints;	
	//cout << finalPoints.points.size();
	//Mat imFinPoints(Size(1280, 720), CV_64FC3);
	//for (int i = 0; i<finalPoints.points.size(); i++)
	//	circle(imFinPoints, finalPoints.points[i], (finalPoints.radios[i].height + finalPoints.radios[i].height) / 4, colorTab2[i % 20], 1);
	//imshow("Puntos finales", imFinPoints);


	T_INT insertable = 0;
	T_INT c1, c2, c3;
	c1 = 0;
	c2 = 1;
	c3 = 0;
	T_DOUBLE rProp = 3.3;
	T_INT maxP = 4;

	c1 = initialPoints.nPoints;

	finalPoints.Clear();
	while (c1 != c2 && c3++<10)
	{
		//Mat imPoints(Size(1280,720), CV_64FC3);

		c1 = initialPoints.nPoints;
		finalPoints.Clear();
		for (int i = 0; i < initialPoints.nPoints; i++)
		{
			insertable = 1;
			for (int j = 0; j < initialPoints.nPoints; j++) {
				if (i != j)
					if (initialPoints.distances.Get(i, j) <= initialPoints.maxRadio * rProp)
						insertable++;
				if (insertable == maxP)
					break;
			}

			//cout <<" Max radio:"<< initialPoints.maxRadio<<" Insertable:"<<insertable;

			if (insertable == maxP)
			{
				//cout << " ok";
				finalPoints.points.push_back(initialPoints.points[i]);
				finalPoints.radios.push_back(initialPoints.radios[i]);
				//circle(imPoints, initialPoints.points[i], initialPoints.radios[i].width/2, colorTab2[i], 1);
				//circle(imPoints,initialPoints.points[i],initialPoints.maxRadio*rProp , colorTab2[i],1,2);
			}
			//cout << endl;
		}
		finalPoints.Init(finalPoints.points.size());
		finalPoints.Calculate();
		c2 = finalPoints.nPoints;
		initialPoints = finalPoints;

		//imshow("puntitos .......", imPoints);
		//waitKey(250);
	}




	return finalPoints;
}


// Elegir las Elipses finales, rpimer algoritmo, ademas se eligen posisiones unicas

PatternPoint<T_DOUBLE> EllipseFinder::ElegirElipsesFinalesAsimetrico( int nro_puntos)//vector< RotatedRect > boxElegidos, h_Matriz<double> distancias, T_DOUBLE &dmax)
{
	// 15 ms aproximadamente algo muy pesado
	
	// Primero elimino las esferas concentricas
	//Aca se reducen las esferas concentricas 
	int varCenter = 3;
	PatternPoint<T_DOUBLE> box;
	double distance = 0, x, y, cont;
	Size r;
	
	//Mat imInitPoints(Size(1280, 720), CV_64FC3);
	//for (int i = 0; i<initialPoints.nPoints; i++)
	//			circle(imInitPoints, initialPoints.points[i], (initialPoints.radios[i].height + initialPoints.radios[i].height) / 4, colorTab2[i%20], 1);
	//imshow("Puntos Iniciales", imInitPoints);
	
	
	// Para eliminar lospuntos concentricos

	if (initialPoints.nPoints > 2 * nro_puntos) // solo se ejecuta si se detecta concentricos
	{
		int i, j;

		for (i = 0; i < initialPoints.nPoints; ++i) {
			Point2f nuevo = initialPoints.points[i];
			x = 0;
			y = 0;
			r = initialPoints.radios[i];
			cont = 1;
			for (j = 0; j < initialPoints.nPoints; ++j) {
				if (i != j) {
					Point2f temp = initialPoints.points[j];
					Size rtemp = initialPoints.radios[j];
					distance = norm(nuevo - temp);
					if (distance <= varCenter &&  temp.x != 0) {
						x += temp.x; y += temp.y;
						initialPoints.points[j] = Point2f(0, 0);
						if (rtemp.height > r.height) r.height = rtemp.height;
						if (rtemp.width > r.width) r.width = rtemp.width;
						cont++;
					}
				}
			}
			x += initialPoints.points[i].x;
			y += initialPoints.points[i].y;
			//cout <<"x: "<< x <<" y: "<< y <<" cont: "<< cont<<endl;
			initialPoints.points[i].x = x / cont;
			initialPoints.points[i].y = y / cont;
			initialPoints.radios[i] = r;
		}

		for (i = 0; i < initialPoints.nPoints; i++) {
			if (initialPoints.points[i].x != 0) {
				box.points.push_back(initialPoints.points[i]);
				box.radios.push_back(initialPoints.radios[i]);
			}
		}

		box.Init(box.points.size());
		box.Calculate();
		initialPoints = box;
	}
	
	// Se eliminan las esferas muy pequeñas
	
	

	//Mat imSinPeques(Size(1280, 720), CV_64FC3);
	finalPoints.Clear();
	T_DOUBLE w_h_max;
	//cout << " eliminar peques"<<endl;
	//initialPoints.Calculate();
	for (int i = 0; i < initialPoints.nPoints; i++)
	{		
		w_h_max = (initialPoints.radios[i].height + initialPoints.radios[i].width) / 2;
		//cout << " w_h_max:" << w_h_max<<" max radio"<< initialPoints.maxRadio <<endl;
		if (w_h_max < initialPoints.maxRadio*1.45 && w_h_max > initialPoints.maxRadio*0.60)
		{
			//cout << " ok";
			finalPoints.points.push_back(initialPoints.points[i]);
			finalPoints.radios.push_back(initialPoints.radios[i]);
			//circle(imSinPeques, initialPoints.points[i], initialPoints.radios[i].width / 2, colorTab2[i], 1);
			//circle(imSinPeques, initialPoints.points[i], initialPoints.maxRadio*rProp, colorTab2[i], 1, 2);
		}
	}
	finalPoints.Init(finalPoints.points.size());
	finalPoints.Calculate();
	initialPoints = finalPoints;

	//imshow("si  peques .......", imSinPeques);	
	//cout << endl;	
	//waitKey(250);



	
	//Mat imInitPointsUnico(Size(_width, 360), CV_64FC3);
	//for (int i = 0; i<initialPoints.nPoints; i++)
	//	circle(imInitPointsUnico, initialPoints.points[i], (initialPoints.radios[i].height+ initialPoints.radios[i].height)/4, colorTab2[i] , 1);
		
	//imshow("puntitos unicos sin concetricos ", imInitPointsUnico);
	
	// desde aca se verifica que los puntos pertenecesn al patron
	
	//Esta parte es para detectar a las eferas que podrian estar dentro del patron		
	//T_INT insertable = 0;

	T_INT insertable = 0;
	T_INT c1, c2, c3;
	c1 = 0;
	c2 = 1;
	c3 = 0;
	T_DOUBLE rProp = 3.3;
	T_INT maxP = 4;

	c1 = initialPoints.nPoints;

	finalPoints.Clear();
	while (c1 != c2 && c3++<10)
	{
		//Mat imPoints(Size(1280,720), CV_64FC3);
		
		c1 = initialPoints.nPoints;
		finalPoints.Clear();		
		for (int i = 0; i < initialPoints.nPoints; i++)
		{
			insertable = 1;
			for (int j = 0; j < initialPoints.nPoints; j++) {
				if (i != j)
					if (initialPoints.distances.Get(i, j) <= initialPoints.maxRadio * rProp)
						insertable++;					
				if (insertable == maxP)
					break;
			}
			
			//cout <<" Max radio:"<< initialPoints.maxRadio<<" Insertable:"<<insertable;
			
			if (insertable == maxP)
			{
				//cout << " ok";
				finalPoints.points.push_back(initialPoints.points[i]);
				finalPoints.radios.push_back(initialPoints.radios[i]);
				//circle(imPoints, initialPoints.points[i], initialPoints.radios[i].width/2, colorTab2[i], 1);
				//circle(imPoints,initialPoints.points[i],initialPoints.maxRadio*rProp , colorTab2[i],1,2);
			}
			//cout << endl;
		}
		finalPoints.Init(finalPoints.points.size());
		finalPoints.Calculate();
		c2 = finalPoints.nPoints;
		initialPoints = finalPoints;
		
		//imshow("puntitos .......", imPoints);
		//waitKey(250);
	}
	
	//imshow("puntitos patron...", imInitPoints);

	
	//finalPoints.FindCentroid();
	//Mat finpoints(Size(_width, 360), CV_64FC3);
	//for (int i = 0; i<finalPoints.nPoints; i++)
	//	circle(finpoints, finalPoints.points[i], (finalPoints.radios[i].height + finalPoints.radios[i].height) / 4, colorTab[i % 12], 1);
	//circle(finpoints, finalPoints.centroid, 3, colorTab[4], 1);
	
	//imshow("puntitos patron...", imInitPoints);
	
	return finalPoints;
}

//Segundo algoritmo	, elegir los puntos mas cercanos al center, que es el centroide del patron anterior
PatternPoint<T_DOUBLE> EllipseFinder::ElegirElipsesFinales2(PatternPoint<T_DOUBLE> boxElegidos, Point2f center, T_DOUBLE dmax)
{
	PatternPoint<T_DOUBLE> boxFinal;	
	T_INT insertable = 0;
	for (int i = 0; i < boxElegidos.nPoints; i++)
		if (norm(boxElegidos.points[i] - center) < dmax*1.15)
		{
			boxFinal.points.push_back(boxElegidos.points[i]);
			boxFinal.radios.push_back(boxElegidos.radios[i]);
		}
	boxFinal.Init(boxFinal.points.size());
	return boxFinal;
}

//Obtiene las elipses necesarias
bool EllipseFinder::FindEllipse(cv::Mat& in, cv::Mat& out, int &frame_counter, Point2f center, double distMaxPatron, int tipo, int NroPuntos) {

	// Matriz para obtener los contornos semjantes a 
	
	out = Mat::zeros(in.size(), CV_8UC3);
	Mat cimage;
	PatternPoint<T_DOUBLE> boxFinaltmp;

	initialPoints.Clear();
	//initialPoints.points.clear();
	//initialPoints.radios.clear();

	// Rectangulo que contine a las elipses ajustadas a los contronos que mas se asemejan a una elipse		
	for (size_t i = 0; i < contours.size(); i++) {
		
		cimage = Mat::zeros(in.size(), CV_8UC3);

		// Contornos muy pequenos, se eliminan
		if (contours[i].size() < 10)	continue;// se descarta

		//esta parte es de acuerdo a la forma del contorno, si son muy achatados			
		if (tipo == PATTERN::SIMETRICO)
			if (!VerifyContourShape(contours[i])) continue; // se descarta

		//Ajustar Contorno a Elipse se obtiene u  box que circunscribe a la elipse
		RotatedRect box = FitContourToEllipse(contours[i]);

		if (!VerifyEllipseNoEnlarged(box)) continue; // se descarta

		//dibujar el contorno i
		//drawContours(out, contours, (int)i, Scalar::all(255), 1, 8);
		drawContours(cimage, contours, (int)i, Scalar::all(255), 1, 8);

		//imshow("contornos", cimage);
		//waitKey(250);

		// debo encontrar los puntos que se intersectan las elipses con los rectangulos
		if (!VerifyEllipseShaped(box, cimage)) continue; // se descarta

		// Se inserta datos del box elegido		
		initialPoints.points.push_back(box.center);
		initialPoints.radios.push_back(box.size);

	}// hasta aca se tiene todos los contornos que mas se asemejan a una elipse

	//imshow("contornos", out);


	initialPoints.Init(initialPoints.points.size()); // con esto ya se puede hacer calculos de distancias.
	initialPoints.Calculate();

	
	// aca es del frame anterior
	//if (frame_counter > 1)
	//	circle(out, center, distMaxPatron*1.15, Scalar(125, 125, 255), 1);

	// Solo si hay la cantidad de puntos necesarios, si son menos no se hace nada, se pierde el frame
	if (initialPoints.nPoints >= NroPuntos) {
	
		// Aca es donde descarto a los puntos que no me son utiles
		
		boxFinaltmp = ElegirElipsesFinales(NroPuntos);
		if (boxFinaltmp.nPoints == NroPuntos)
		{
			finalPoints = boxFinaltmp;
			return true;
			//Dibujar las elipses elegidas
		}
		else
			return false;

		/*
		// este codigo es para trabajar con el centroide anterior... pero lo vamos a mejorar
		// luego si hay muchos puntos entonces solo me quedo con los necesarios						
		if (frame_counter > 1 && boxFinaltmp.nPoints >= NroPuntos)
			finalPoints = ElegirElipsesFinales2(boxFinaltmp, center, distMaxPatron);
		else
			finalPoints = boxFinaltmp;

		//finalPoints.Init(finalPoints.points.size());
		if (finalPoints.nPoints == NroPuntos)
			return true;
		else
			return false;
		*/
	}
	else
		return false;
}