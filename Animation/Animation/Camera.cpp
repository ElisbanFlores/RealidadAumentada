#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Inicializar(VideoCapture *_cvideo, int nframes)
{
	cameraMatrix = Mat::eye(3, 3, CV_64F);
	NroElemDistCoefficient = 8;
	distortionCoefficient = Mat::zeros(NroElemDistCoefficient, 1, CV_64F);
	imageSize.height = _cvideo->get(CV_CAP_PROP_FRAME_HEIGHT);
	imageSize.width = _cvideo->get(CV_CAP_PROP_FRAME_WIDTH);
	// Aca cuantos frames voy a elegir para calibrar
	NroFramesCalibracion = nframes;
}

void Camera::SaveParameters(string FileName)
{
	std::ofstream destino(FileName);
	if (destino.fail())
		std::cerr << "Error opening the file: " << FileName << std::endl;
	else
	{
		//Camera Matrix				
		for (int i = 0; i<cameraMatrix.rows; i++)
			for (int j = 0; j < cameraMatrix.cols; j++)
				destino << cameraMatrix.at<double>(i, j) << endl;

		NroElemDistCoefficient = distortionCoefficient.rows;
		destino << NroElemDistCoefficient << endl;

		//Distortion Coefficient
		for (int i = 0; i<NroElemDistCoefficient; i++)
			for (int j = 0; j < distortionCoefficient.cols; j++)
				destino << distortionCoefficient.at<double>(i, j) << endl;
		// imageSize
		destino << imageSize.height << endl;
		destino << imageSize.width << endl;
		// Nro de Frames
		destino << NroFramesCalibracion << endl;
		// Nro de Frames				
		//rvecs
		for (int k = 0; k < NroFramesCalibracion; k++) {
			Mat val = rvecs[k];
			for (int i = 0; i<val.rows; i++)
				for (int j = 0; j < val.cols; j++)
					destino << val.at<double>(i, j) << endl;
		}
		//tvecs
		for (int k = 0; k < NroFramesCalibracion; k++) {
			Mat val = tvecs[k];
			for (int i = 0; i<val.rows; i++)
				for (int j = 0; j < val.cols; j++)
					destino << val.at<double>(i, j) << endl;
		}
		destino.close();
	}
}

void Camera::OpenParameters(string FileName)
{
	std::ifstream origen(FileName, std::ios::binary);
	if (origen.fail())
		std::cerr << "Error opening the file: " << FileName << std::endl;
	else {
		const int dim = 256;
		char linea[dim], temp[dim];

		//	origen >> cameraMatrix;
		//Camera Matrix				
		for (int i = 0; i<cameraMatrix.rows; i++)
			for (int j = 0; j < cameraMatrix.cols; j++) {
				origen.getline(linea, dim, '\n');
				cameraMatrix.at<double>(i, j) = atof(linea);
			}

		origen.getline(linea, dim, '\n');
		NroElemDistCoefficient = atoi(linea);

		distortionCoefficient = Mat::zeros(NroElemDistCoefficient, 1, CV_64F);

		//Distortion Coefficient
		for (int i = 0; i<distortionCoefficient.rows; i++)
			for (int j = 0; j < distortionCoefficient.cols; j++) {
				origen.getline(linea, dim, '\n');
				distortionCoefficient.at<double>(i, j) = atof(linea);
			}

		// imageSize
		origen.getline(linea, dim, '\n');
		imageSize.height = atoi(linea);
		origen.getline(linea, dim, '\n');
		imageSize.width = atoi(linea);

		// Nro de Frames
		origen.getline(linea, dim, '\n');
		NroFramesCalibracion = atoi(linea);

		//rvecs
		rvecs.clear();
		for (int k = 0; k < NroFramesCalibracion; k++)
		{
			Mat val = Mat::zeros(3, 1, CV_64F);
			for (int i = 0; i<val.rows; i++)
				for (int j = 0; j < val.cols; j++)
				{
					origen.getline(linea, dim, '\n');
					val.at<double>(i, j) = atof(linea);
				}
			rvecs.push_back(val);
		}
		//tvecs
		tvecs.clear();
		for (int k = 0; k < NroFramesCalibracion; k++)
		{
			Mat val = Mat::zeros(3, 1, CV_64F);
			for (int i = 0; i<val.rows; i++)
				for (int j = 0; j < val.cols; j++)
				{
					origen.getline(linea, dim, '\n');
					val.at<double>(i, j) = atof(linea);
				}
			tvecs.push_back(val);
		}
		origen.close();
	}
}

void Camera::ShowParameters(int forma)
{
	cout << "CAMERA PARAMETERS:" << endl;
	cout << "Camera Matrix" << endl;
	cout << cameraMatrix << endl;
	cout << "Distortion Coefficients:" << endl;
	cout << distortionCoefficient << endl;

	if (forma != 1)
	{
		cout << "Rotation Matrix" << endl;
		for (int i = 0; i<rvecs.size(); i++)
			cout << rvecs[i] << endl;

		cout << "Traslation Matrix:" << endl;
		for (int i = 0; i<tvecs.size(); i++)
			cout << tvecs[i] << endl;
	}
}
