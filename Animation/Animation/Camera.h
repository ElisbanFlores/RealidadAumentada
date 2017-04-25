#pragma once
#include "Common.h"

class Camera
{

public:

	Mat cameraMatrix; // Matriz de la camara
	Mat distortionCoefficient; // Coeficinetes de distorcion
	Size imageSize; // tamaño de la  imagen
	vector<Mat> rvecs, tvecs; // matrices de rotacion y traslacion
	//vector<float> reprojErrs; // Error de retropreyeccion	
	//double totalAvgErr; // Promedio del error de retroproyeccion	
	int NroFramesCalibracion; // la cantidad de frames que utilizamos para calibrar
	int NroElemDistCoefficient;
	
	Camera();
	~Camera();
	void Inicializar(VideoCapture *, int);
	void SaveParameters(string);
	void OpenParameters(string);
	void ShowParameters(int forma = 1);
};

