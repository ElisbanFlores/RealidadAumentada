#ifndef APP_CPP
#define APP_CPP

#include "App.h"

template <typename SC>

App<SC>::App(setup_class_t& sct)
	:
	total_frame_counter(0), frame_counter(1),
	lost_frames(0.0), processed_frames(0.0) {
	
	setup_information = sct;

	calibration_video = new VideoCapture(setup_information.input_video_filename.c_str());
	benchmark_video = new VideoCapture(setup_information.benchmark_video_filename.c_str());
	video_name = setup_information.input_video_filename.c_str();

	if (!calibration_video->isOpened() || !benchmark_video->isOpened())
	{
		cout << "no hay videos jojolete";
		
		return;
	}
	
	cout << "App starting..." << endl;

	benchmark_video_frames = GetNumberOfFrames(benchmark_video);
	calibration_video_frames = GetNumberOfFrames(calibration_video);
	
	cout <<" videos explorados"<<endl;
	getchar();
	selection_frame_step = static_cast<int>(calibration_video_frames / setup_information.number_of_samples);

	cout << endl << "n frmaes:" << calibration_video_frames << " nsamples:" << setup_information.number_of_samples << " step:" << selection_frame_step << endl;

	camera_matrix = Mat::eye(3, 3, CV_64F);
	distortion_coefficient = Mat::zeros(8, 1, CV_64F);
	image_size.height = calibration_video->get(CV_CAP_PROP_FRAME_HEIGHT);
	image_size.width = calibration_video->get(CV_CAP_PROP_FRAME_WIDTH);

	patternFinder.Init(setup_information.pattern_points_rows, setup_information.pattern_points_cols, setup_information.pattern_type);

	// Para la calibracion de camara

	camParameters.Inicializar(calibration_video, setup_information.frames_per_calibration);	
	
	if (setup_information.select_frame_flag == DENSITY) {
		float padding = sqrt(setup_information.number_of_samples);
		float padding_x = (float)image_size.width / padding;
		float padding_y = (float)image_size.height / padding;
		for (float i = 0; i < (float)image_size.width + 1; i += padding_x) 
			for (float k = 0; k < (float)image_size.height + 1; k += padding_y)
				selection_frame_mesh.push_back(pair<Point2f, vector< FramePattern > >(Point2f(i, k), vector<FramePattern>()));			
	}
	cout << "App Started, let us to continue...." << endl;
	cout << "press any key to continue..."<<endl;
	getchar();

}

//------------------------------------------------------------
template <typename SC>
App<SC>::~App(void) {
	delete this;
}

//------------------------------------------------------------
template <typename SC>
long App<SC>::GetNumberOfFrames(VideoCapture *video) {
	long number_of_frames = 0;
	Mat temporal_frame;
	video->set(CAP_PROP_POS_FRAMES, 0.0);
	while (video->read(temporal_frame)) {
		if (temporal_frame.empty()) break;
		number_of_frames++;
	}
	video->set(CAP_PROP_POS_FRAMES, 0.0);
	return number_of_frames;
}

template <typename SC>
void App<SC>::CreateVideo(int camera)
{
	VideoCapture vcap(camera);
	if (!vcap.isOpened()) {
		cout << "Error opening video device" << endl;
		return;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "w,h" << frame_width << "," << frame_height << endl;
	VideoWriter video("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);

	while(1) {

		Mat frame;
		vcap >> frame;
		video.write(frame);
		imshow("Frame", frame);
		char c = (char)waitKey(1);
		if (c == 27) break;
	}

}

//------------------------------------------------------------
template <typename SC>
void App<SC>::SelectPointsToCalibrate(void)
{
	Mat in;
	double tiempoProm = 0.0;
	double tiempo = 0.0;

	while (total_frame_counter++ < calibration_video_frames) {
		
		*calibration_video >> in;
		//imshow("imagen", in);
		auto start = std::chrono::high_resolution_clock::now();
		
		execute(in, setup_information.method, setup_information.operation_mode,false);
		
		auto finish = std::chrono::high_resolution_clock::now();
		tiempo = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000.0;
		tiempoProm += tiempo;
		//esquinasPatron.clear();
		if (cv::waitKey(1) == 27)break;
	}

	cout << "POINTS LOCATED:" << endl;
	cout << endl << "RESUMEN:" << endl;
	cout << "Nro Frames:\t" << calibration_video_frames << endl;
	cout << "Nro Frames procesados:\t" << --total_frame_counter << endl;
	cout << "Nro Frames descartados:\t" << total_frame_counter - processed_frames << endl;
	cout << "Tiempo Promedio:\t" << tiempoProm / total_frame_counter << " ms por frame" << endl;
	cout << "Frames por segundo:\t" << 1000 / (tiempoProm / total_frame_counter) << " frames" << endl;
}

//------------------------------------------------------------
template <typename SC>
void App<SC>::SaveBoardPoints(bool Ankur) {
	
	if (setup_information.select_frame_flag == DENSITY) {
		
		for (int i = 0; i < selection_frame_mesh.size(); i++) {
			std::sort(selection_frame_mesh[i].second.begin(), selection_frame_mesh[i].second.end());
		}

		//int frames_per_generator = setup_information.number_of_samples / selection_frame_mesh.size() + 2;
		int frames_per_generator;

		vector< int > noEmptyFrames;

		for (int i = 0; i < selection_frame_mesh.size(); i++)
			if (!selection_frame_mesh[i].second.empty())
			{
				noEmptyFrames.push_back(i);
				cout << i<<", ";
			}
		cout << endl;

		frames_per_generator = setup_information.number_of_samples / noEmptyFrames.size();
		
		// Para limpiar los archivos puntos.data y fpuntos.data
		if (noEmptyFrames.size() != 0){
			string path = "samples/" + video_name + string("/");
			string fname = "puntos.data";
			ClearFile(path+ fname);
			ClearFile(path + string("f") + fname);
		}
		
		for (int j = 0; j < noEmptyFrames.size(); j++)
		{
			int i = noEmptyFrames[j];

			if (selection_frame_mesh[i].second.size() < frames_per_generator) {
				for (int k = 0; k < selection_frame_mesh[i].second.size(); k++) {
					selectFrame(selection_frame_mesh[i].second[k].frame,
						selection_frame_mesh[i].second[k].path,
						selection_frame_mesh[i].second[k].imageFileName,
						selection_frame_mesh[i].second[k].dataFileName,
						selection_frame_mesh[i].second[k].pattern_points);
				}
			}
			else {
				for (int k = 0; k < frames_per_generator; k++) {
					selectFrame(selection_frame_mesh[i].second[k].frame,
						selection_frame_mesh[i].second[k].path,
						selection_frame_mesh[i].second[k].imageFileName,
						selection_frame_mesh[i].second[k].dataFileName, 
						selection_frame_mesh[i].second[k].pattern_points);
				}
			}

		}
	}
}

//------------------------------------------------------------
template <typename SC>
void App<SC>::ourCalibrateCamera(string camParamPath, bool ankur) {
	
	vector< vector<Point3f> > objectPoints = GenerarObjectPoints(GenerarObjectPoint(setup_information.pattern_points_rows, setup_information.pattern_points_cols, setup_information.distance_between_points, setup_information.pattern_type, image_size), setup_information.frames_per_calibration);
	
	string Path = "samples/" + video_name + string("/");
	string dataFileName;
	
	dataFileName = "puntos.data";

	//vector< vector<Point2f> > imagePoints = GenerarImagePoints(Path, dataFileName, setup_information.frames_per_calibration,ankur);
	imagePoints = GenerarImagePoints(Path, dataFileName, setup_information.frames_per_calibration, ankur);

	Mat imgNubePuntos = DibujarNubePuntos(imagePoints, image_size);
		
	cout << "Calibrando...." << endl;	
	vector<Mat> rvecs;
	vector<Mat> tvecs;

	double RMS = calibrateCamera(objectPoints, imagePoints, camParameters.imageSize, camParameters.cameraMatrix, camParameters.distortionCoefficient, camParameters.rvecs, camParameters.tvecs, CV_CALIB_ZERO_TANGENT_DIST);

	imshow("Nube de puntos", imgNubePuntos);
	cout << "RESULTADOS DE CALIBRACION:" << endl;
	cout << "RMS :" << RMS << endl;
	camParameters.ShowParameters(1);
	cout << "Calibracion terminada... press any key to continue"<<endl;
	camParameters.SaveParameters(camParamPath);
}

//------------------------------------------------------------
// Para esto ya debe estar calibrado la camara

template <typename SC>
void App<SC>::distortPoints(std::vector<cv::Point2f> src, std::vector<cv::Point2f> & dst,
	cv::Mat cameraMatrix, cv::Mat distorsionMatrix)
{
	cout << "distortPoints init" << endl;


	dst.clear();
	double fx = cameraMatrix.at<double>(0, 0);
	double fy = cameraMatrix.at<double>(1, 1);
	double ux = cameraMatrix.at<double>(0, 2);
	double uy = cameraMatrix.at<double>(1, 2);
	cout << "1" << endl;
	double k1 = distorsionMatrix.at<double>(0);
	double k2 = distorsionMatrix.at<double>(1);
	double p1 = distorsionMatrix.at<double>(2);
	double p2 = distorsionMatrix.at<double>(3);
	double k3 = distorsionMatrix.at<double>(4);
	cout << "2" << endl;

	for (unsigned int i = 0; i < src.size(); i++)
	{
		const cv::Point2f & p = src[i];
		double x = p.x;
		double y = p.y;
		double xCorrected, yCorrected;
		//Step 1 : correct distorsion
		{
			double r2 = x*x + y*y;
			//radial distorsion
			xCorrected = x * (1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2);
			yCorrected = y * (1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2);

			//tangential distorsion
			//The "Learning OpenCV" book is wrong here !!!
			//False equations from the "Learning OpenCv" book below :
			//xCorrected = xCorrected + (2. * p1 * y + p2 * (r2 + 2. * x * x));
			//yCorrected = yCorrected + (p1 * (r2 + 2. * y * y) + 2. * p2 * x);
			//Correct formulae found at : http://www.vision.caltech.edu/bouguetj/calib_doc/htmls/parameters.html
			xCorrected = xCorrected + (2. * p1 * x * y + p2 * (r2 + 2. * x * x));
			yCorrected = yCorrected + (p1 * (r2 + 2. * y * y) + 2. * p2 * x * y);
		}
		//Step 2 : ideal coordinates => actual coordinates
		{
			xCorrected = xCorrected * fx + ux;
			yCorrected = yCorrected * fy + uy;
		}
		dst.push_back(cv::Point2f(xCorrected, yCorrected));
	}
	cout << "distortPoints finish" << endl;

}


template <typename SC>
void App<SC>::AnkurDatta(void) {

	double tiempoProm = 0.0;
	double tiempo = 0.0;
	
	//configurar parametros de la camara
	string path= "samples/" + video_name + string("/");	
	string filePuntosAnkur = "ankur_puntos.data";
	string camParamPath = path + ("CamParam.data"); // donde se almacena los parametros de calibracion

	// Calibracion de la cámara, es la calibracion general, son los valores iniciales para el ciclo de Ankur.
	cout << "Calibrando la camara" << endl;	
	ourCalibrateCamera(camParamPath);
	
	// Recuperando datos de calibracion
	cout << endl<< "Recuperando datos de calibracion de la camara" << endl;
	camParameters.OpenParameters(camParamPath);
	//camParameters.ShowParameters(2);	
	cout << "Calibracion inicial terminada...." << endl;
	getchar();
		
	vector< vector<Point2f> > AnkurPuntos;
	
	T_INT nIter=10;
	T_INT *lostFrames;
	lostFrames = new T_INT[setup_information.frames_per_calibration];

	//DESDE ACA CORRE ANKUR ITERATIVAMENTE
	long cont = 0;
	while (++cont < nIter)
	{
		imagePoints.clear();
		
		// se hace la calibracion de camara con los puntos generados por el ciclo de Ankur

		cout << "ITERACION " << cont << endl << endl;
		cout << "Calibrando la camara para la iteracion: " << cont << endl;
		ourCalibrateCamera(camParamPath, true);

		// Recuperando datos de calibracion
		cout <<endl<< "Recuperando datos de calibracion de la camara" << endl;
		camParameters.OpenParameters(camParamPath);
		cout << "Press any key to continue"<<endl;
		getchar();

		AnkurPuntos.clear();
		// limpiar los frames perdidos
		for (int i = 0; i < setup_information.frames_per_calibration; i++)
			lostFrames[i] = 0;

		//camParameters.ShowParameters(2);
		//cout << "press any key to continue..." << endl;
		

		// Inicializando valores para los frames
		total_frame_counter = 0;
		frame_counter = 1;

		// Hallando parametros para el undistort de la imagen
		Mat view, rview, map1, map2;

		//Hallando new Camera Matrix
		Mat newCameraMatrix = getOptimalNewCameraMatrix(camParameters.cameraMatrix, camParameters.distortionCoefficient, camParameters.imageSize, 1, camParameters.imageSize, 0);
		//Hallando parámetros para el Undistor
		initUndistortRectifyMap(camParameters.cameraMatrix, camParameters.distortionCoefficient, Mat(), newCameraMatrix, camParameters.imageSize, CV_16SC2, map1, map2);

		// Generacion de los puntos destino para el frontoparalelo
		Size size = camParameters.imageSize;
		Mat imFrontoParalelo = Mat::zeros(size, CV_8UC3);
		int m = setup_information.pattern_points_rows, n = setup_information.pattern_points_cols;
		Mat patron(image_size, CV_64FC3);
		vector<Point2f> pts_dst = GeneratePointsForFrontoParallel(m, n, setup_information.pattern_type, image_size);
		patternFinder.DrawPattern(patron, m, n, pts_dst, colorTab, setup_information.pattern_type);

		// Leer las imagenes seleccionadas para la calibracion, los nombres se almacenan en imgNames
		const int dim = 256;
		char linea[dim];
		vector<string> imgNames;
		string fileNameIS = path + string("spuntos.data");
		ifstream fileImgNamesSel;
		fileImgNamesSel.open(fileNameIS);

		while (!fileImgNamesSel.eof()) {
			fileImgNamesSel.getline(linea, dim, '\n');
			if (strlen(linea) > 1)imgNames.push_back(string(linea));
		}


		// Iniciarl el ciclo de Ankur Datta para las imagenes preseleccionadas
		for (int i = 0; i < imgNames.size(); i++, total_frame_counter++)
		{
			auto start = std::chrono::high_resolution_clock::now();

			// Leer la imagen
			cout << "Frame name: " << imgNames[i] << endl;
			view = imread(path + imgNames[i]);

			//Obtener la imagen sin distorsion, view es la imagen originaL, rview es la imagen rectificada o sin distorsion		
			remap(view, rview, map1, map2, INTER_LINEAR);

			Mat	imgtmp;
			rview.copyTo(imgtmp);

			// Buscar el patron en la imagen sin distorision
			bool foundInUndistorted = execute(rview, setup_information.method, SIZE);
			cout << "parece bueno esto" << endl;
			//waitKey();
			// Si se reconoce el patron exitosamente
			if (!foundInUndistorted)
			{
				lostFrames[i] = 1;
			}			
			else
			{
				//Para la Homografia
				Mat imgtemporal(image_size, CV_64FC3);
				//Se calcula la matriz de homografia
				Mat mHomography = findHomography(patternFinder.patternGenerated.points, pts_dst, RANSAC);
				cout << "Matriz de Homografìa" << endl;
				cout << mHomography << endl;
				//Se Halla la imagen Frontoparalelo
				warpPerspective(imgtmp, imFrontoParalelo, mHomography, size);
				// Se muestra la imagen del Frontoparalelo
				//imshow("Image Frontoparalelo", imFrontoParalelo);

				// Buscar el patron en la imagen del Frontoparalelo
				bool foundInFrontoparalelo = execute(imFrontoParalelo, setup_information.method, SIZE, true);

				//Si se reconoce el patron exitosamente				
				if (!foundInFrontoparalelo)
				{
					lostFrames[i] = 1;
				}
				else
				{
					//-------------------------------------------------
					// RETROPROYECTAR PUNTOS EN IMAGEN SIN DISTORSION
					//-------------------------------------------------

					// Mostrar los puntos detectados en el FrontoParalelo
					for (int j = 0; j < patternFinder.patternGenerated.points.size(); j++)
						circle(imFrontoParalelo, patternFinder.patternGenerated.points[j], 1, colorTab[11]);
					imshow("Puntos en la homografia en el Frontoparalelo", imFrontoParalelo);

					// Hallar la inversa de la Matriz de Homografia
					Mat invMHomography = mHomography.inv();
					cout << "Matriz de Homografia invertida" << endl;
					cout << invMHomography << endl;

					//Hallo la Imagen de la Homografia retroproyectada
					Mat imRetroproyected(image_size, CV_64FC3);
					warpPerspective(imFrontoParalelo, imRetroproyected, invMHomography, size);

					//Aca hallo los coordenadas de los puntos retroproyectados que fueron hallados en la Homografia				
					PatternPoint<T_DOUBLE> puntosRepro;
					// Recorro todos los puntos que estan en 2D
					for (int j = 0; j < patternFinder.patternGenerated.points.size(); j++)
					{
						// Convierto los puntos en 2D a 3D

						Mat mPunto3(3, 1, CV_64F);
						Point2f punto2 = patternFinder.patternGenerated.points[j];
						mPunto3.at<double>(0, 0) = punto2.x; // x
						mPunto3.at<double>(1, 0) = punto2.y; // y
						mPunto3.at<double>(2, 0) = 1;		 // z

						// multiplico la Matriz inversa de la Homografia por el punto 3D para hallar su nueva coordenada
						Mat mPunto32 = invMHomography*mPunto3;

						// Convierto el punto 3D hallado a 2D usando formulas de la Camara pinhole
						Point3f punto3;
						punto3.x = mPunto32.at<double>(0, 0);
						punto3.y = mPunto32.at<double>(1, 0);
						punto3.z = mPunto32.at<double>(2, 0);
						punto2.x = punto3.x / punto3.z;
						punto2.y = punto3.y / punto3.z;

						// Se muestra el punto 3D y su correspondiente 2D
						cout << punto3 << endl;
						cout << punto2 << endl << endl;

						// Se almacena las nuevas coordenadas
						puntosRepro.points.push_back(punto2);
						circle(imRetroproyected, punto2, 1, colorTab[0], 1);
					}

					imshow("Imagen de Homografia retroproyectada y puntos encontrados..", imRetroproyected);

					//-------------------------------------------------
					// RETROPROYECTAR PUNTOS EN IMAGEN ORIGINAL
					//-------------------------------------------------

					//Convierto los puntos encontrados a una matriz
					Mat original = Mat(puntosRepro.points);
					Mat undistorted;

					// aplico el undistort usando los parametros de la new Camera con la que fueron hallados los puntos
					undistortPoints(original, undistorted, newCameraMatrix, Mat::zeros(Size(5, 1), CV_64F));

					// Convierto los puntos de la Matriz a un vector
					vector< cv::Point2f > undistorted_points;
					for (int j = 0; j < puntosRepro.points.size(); ++j) {
						Point2f p;
						p.x = undistorted.at<float>(j, 0);
						p.y = undistorted.at<float>(j, 1);
						undistorted_points.push_back(p);
					}

					// hallo el distort del undistort pero con loa parametros de la camara utilizados para la calibracion
					PatternPoint<T_DOUBLE> puntosReproDistort;
					distortPoints(undistorted_points, puntosReproDistort.points, camParameters.cameraMatrix, camParameters.distortionCoefficient);

					//Almaceno los puntos para la siguiente interaccion;
					AnkurPuntos.push_back(puntosReproDistort.points);

					// se Muestra y grafica los nuevos puntos
					for (int j = 0; j < puntosReproDistort.points.size(); j++)
					{
						cout << imagePoints[i][j] << endl;
						cout << puntosReproDistort.points[j] << endl;
						cout << "distancia: " << norm(imagePoints[i][j] - puntosReproDistort.points[j]) << endl;
						circle(view, imagePoints[i][j], 1, colorTab[0], 1);
						circle(view, puntosReproDistort.points[j], 1, colorTab[1], 1);
					}
				
				}
				// se debe guardar el nuevo listado de las imagenes;
				imshow("Resultado final en la imagen Original", view);
			}
			

			auto finish = std::chrono::high_resolution_clock::now();
			tiempo = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000.0;
			cout << "Tiempo: para procesar un frame " << tiempo << "ms por frame" << endl;
			waitKey(10);
		}
		
		// aca termina un ciclo de Ankur data
		// y aca deberia trabajar con la funcion de Levenberg

		// Aca guardo el nuevo listado de las imagenes
		ofstream newFileImgNamesSel;
		newFileImgNamesSel.open(fileNameIS);
				
		for (int i = 0; i < imgNames.size(); i++)
			if (lostFrames[i] == 0)
				newFileImgNamesSel << imgNames[i] << endl;
			else
				setup_information.frames_per_calibration--;
		newFileImgNamesSel.close();
		camParameters.NroFramesCalibracion = setup_information.frames_per_calibration;


		// Los puntos resultantes estan en AnkurPuntos, y los de la corrida anterior estan en imagePoints en ambos casos son vectores de vectores de Point2f
		ClearFile(path + filePuntosAnkur);
		for (int i = 0; i < AnkurPuntos.size(); i++)
			selectFrame(path,filePuntosAnkur, AnkurPuntos[i]); // esto es para la siguiente iteracion
	}


	// se hace la calibracion de camara con los puntos generados por el ciclo de Ankur

	cout <<"CALIBRACION FINAL"<<endl<< "ITERACION " << cont << endl << endl;
	cout << "Calibrando la camara para la iteracion: " << cont << endl;
	ourCalibrateCamera(camParamPath, true);

	// Recuperando datos de calibracion
	cout << endl << "Recuperando datos de calibracion de la camara" << endl;
	camParameters.OpenParameters(camParamPath);
	AnkurPuntos.clear();

	//camParameters.ShowParameters(2);
	//cout << "press any key to continue..." << endl;
	getchar();


	cout << "concluido....";
	getchar();
}

//! [compute_errors] from opencv
template <typename SC>
double App<SC>::computeReprojectionErrors(vector<vector<Point3f> >& objectPoints,
	vector<vector<Point2f> >& imagePoints,
	vector<Mat>& rvecs, vector<Mat>& tvecs,
	Mat& cameraMatrix, Mat& distCoeffs,
	vector<float>& perViewErrors, bool fisheye)
{
	vector<Point2f> imagePoints2;
	size_t totalPoints = 0;
	double totalErr = 0, err;
	perViewErrors.resize(objectPoints.size());

	for (size_t i = 0; i < objectPoints.size(); ++i)
	{
		if (fisheye)
		{
			fisheye::projectPoints(objectPoints[i], imagePoints2, rvecs[i], tvecs[i], cameraMatrix,
				distCoeffs);
		}
		else
		{
			projectPoints(objectPoints[i], rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints2);
		}
		
		err = norm(imagePoints[i], imagePoints2, NORM_L2);
		
		size_t n = objectPoints[i].size();
		perViewErrors[i] = (float)std::sqrt(err*err / n);
		totalErr += err*err;
		totalPoints += n;
	}
	return std::sqrt(totalErr / totalPoints);
}


//------------------------------------------------------------
// Aca es donde se recorre todo el video y se llega ahasta calibrar la camara
template <typename SC>
void App<SC>::init(void) {

	double tiempoProm = 0.0;
	double tiempo = 0.0;

	cout << "seleccionando los puntos a calibrar..." << endl;
	SelectPointsToCalibrate();

	cout << "Guardando los puntos..." << endl;
	SaveBoardPoints();

	cout << "Calibrando la camara" << endl;
	string camParamPath = "samples/" + video_name + string("/CamParam.data"); // dponde se almacena los parametros de calibracion
	ourCalibrateCamera(camParamPath);

	//-----------------------------------------------
	// Leer parametros de Calibracion
	//-----------------------------------------------
	camParameters.OpenParameters(camParamPath);
	camParameters.ShowParameters(2);
	getchar();


	/*total_frame_counter = 0;
	//Mat view, rview, map1, map2;
	//initUndistortRectifyMap(camera_matrix, distortion_coefficient, Mat(),
	//getOptimalNewCameraMatrix(camera_matrix, distortion_coefficient, image_size, 1, image_size, 0),image_size, CV_16SC2, map1, map2);

	Mat view, rview, map1, map2;
	initUndistortRectifyMap(camParameters.cameraMatrix, camParameters.distortionCoefficient, Mat(),
		getOptimalNewCameraMatrix(camParameters.cameraMatrix, camParameters.distortionCoefficient, camParameters.imageSize, 1, camParameters.imageSize, 0),
		camParameters.imageSize, CV_16SC2, map1, map2);

	vector<Point3f> boardPoint;
	vector<Point2f> foundBoardPoint;
	boardPoint = GenerarObjectPoint(setup_information.pattern_points_rows, setup_information.pattern_points_cols, setup_information.distance_between_points, setup_information.pattern_type, image_size);

	// Para el frontoparalelo

	Size size = camParameters.imageSize;
	Mat imFrontoParalelo = Mat::zeros(size, CV_8UC3);

	int offsetx = 30;
	int offsety = 30;
	int m = setup_information.pattern_points_rows, n = setup_information.pattern_points_cols;
	if (size.width / n > size.height / m) {
		auto tmp = size.height / (m - 1);
		offsetx = (size.width - (tmp*(n - 1))) / 2 + tmp / 3;
		offsety = tmp / 3;
	}

	// Create a vector of points.
	vector<Point2f> pts_dst;
	pts_dst.push_back(Point2f(offsetx, offsety));
	pts_dst.push_back(Point2f(size.width - offsetx, offsety));
	pts_dst.push_back(Point2f(size.width - offsetx, size.height - offsety));
	pts_dst.push_back(Point2f(offsetx, size.height - offsety));


	while (total_frame_counter++ < benchmark_video_frames) {
		*benchmark_video >> view;

		auto start = std::chrono::high_resolution_clock::now();
		// corregir la imagen	
		// se corrige la imagen de view en rview
		remap(view, rview, map1, map2, INTER_LINEAR);
		//view es la imagen original.
		// rview es la imagen corregida.

		//aca es donde se saca el fronto paralelo
		Mat	imgtmp;
		rview.copyTo(imgtmp);

		// la distancia se calcula en el frame original
		double d = CalcularDistancia(rview, boardPoint, foundBoardPoint, setup_information.method);

		if (d != -1.0)
		{

			//Mat tform = findHomography(esquinasPatron, pts_dst, RANSAC);
			//cout << tform << endl;
			//warpPerspective(imgtmp, imFrontoParalelo, tform, size);
			//imshow("Image Frontoparalelo", imFrontoParalelo);
			//execute(imFrontoParalelo, setup_information.method, SIZE);


		}
		//esquinasPatron.clear();

		//cleanning();
		//imshow("Video calibrado + distancia", src);
		auto finish = std::chrono::high_resolution_clock::now();
		tiempo = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000.0;
		//cout << "Tiempo: "<< tiempo<< "ms por frame"<< endl;				
		if (cv::waitKey(1) == 27)break;


	}




	//PAra las distancias
	while (total_frame_counter++ < benchmark_video_frames) {
	*benchmark_video >> view;
	auto start = std::chrono::high_resolution_clock::now();
	remap(view, rview, map1, map2, INTER_LINEAR);
	Mat src;
	rview.copyTo(src);
	double d = CalcularDistancia(view, boardPoint, foundBoardPoint, setup_information.method);

	if (d != -1.0){
	string distancia;
	distancia = to_string(d);
	distancia += " mm";
	putText(src, distancia, Point(20, image_size.height - 20), FONT_HERSHEY_PLAIN, 2, CV_RGB(0, 255, 0), 2);
	string dataPath = "samples/" + video_name + string("/distancias.data");
	saveDistances(dataPath, total_frame_counter, d);
	}
	imshow("Video calibrado + distancia", src);

	auto finish = std::chrono::high_resolution_clock::now();
	tiempo = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000.0;
	if (cv::waitKey(1) == 27)break;
	}*/
	waitKey(0);
}

template <typename SC>
void App<SC>::FindDistances(void)
{
	double tiempoProm = 0.0;
	double tiempo = 0.0;
	
	cout << "Medir distancias" << endl;
	string camParamPath = "samples/" + video_name + string("/CamParamAnkur.data"); // dponde se almacena los parametros de calibracion

	//-----------------------------------------------
	// Leer parametros de Calibracion
	//-----------------------------------------------
	camParameters.OpenParameters(camParamPath);
	camParameters.ShowParameters(2);
	getchar();

	
	total_frame_counter = 0;
	//Mat view, rview, map1, map2;
	//initUndistortRectifyMap(camera_matrix, distortion_coefficient, Mat(),
	//getOptimalNewCameraMatrix(camera_matrix, distortion_coefficient, image_size, 1, image_size, 0),image_size, CV_16SC2, map1, map2);

	Mat view, rview, map1, map2;
	initUndistortRectifyMap(camParameters.cameraMatrix, camParameters.distortionCoefficient, Mat(),
		getOptimalNewCameraMatrix(camParameters.cameraMatrix, camParameters.distortionCoefficient, camParameters.imageSize, 1, camParameters.imageSize, 0),
		camParameters.imageSize, CV_16SC2, map1, map2);

	vector<Point3f> boardPoint;
	vector<Point2f> foundBoardPoint;
	boardPoint = GenerarObjectPoint(setup_information.pattern_points_rows, setup_information.pattern_points_cols, setup_information.distance_between_points, setup_information.pattern_type, image_size);

	//PAra las distancias
	while (total_frame_counter++ < benchmark_video_frames) {
		*benchmark_video >> view;
		auto start = std::chrono::high_resolution_clock::now();
		remap(view, rview, map1, map2, INTER_LINEAR);
		Mat src;
		rview.copyTo(src);
		double d = CalcularDistancia(view, boardPoint, foundBoardPoint, setup_information.method);

		if (d != -1.0){
			string distancia;
			distancia = to_string(d);
			distancia += " mm";
			putText(src, distancia, Point(20, image_size.height - 20), FONT_HERSHEY_PLAIN, 2, CV_RGB(0, 255, 0), 2);
			string dataPath = "samples/" + video_name + string("/distanciasAnkur.data");
			saveDistances(dataPath, total_frame_counter, d);
		}
		imshow("Video calibrado + distancia", src);

		auto finish = std::chrono::high_resolution_clock::now();
		tiempo = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000.0;
		if (cv::waitKey(1) == 27)break;
	}
	waitKey(0);
}


//------------------------------------------------------------
template <typename SC>
double App<SC>::CalcularDistancia(Mat imgframe, vector<Point3f> & boardPoint, vector<Point2f> &foundBoardPoint, int type) {
	Mat rvec, tvec, rvecRodrigues;
	bool found = execute(imgframe, type, SIZE);
	if (found) {
		foundBoardPoint = patternFinder.patternGenerated.points;
		solvePnP(boardPoint, foundBoardPoint, camera_matrix, distortion_coefficient, rvec, tvec, false, SOLVEPNP_ITERATIVE);
		Rodrigues(rvec, rvecRodrigues);
		return norm(-rvecRodrigues.t() * tvec);
	}
	return -1.0;
}

//------------------------------------------------------------
template <typename SC>
bool App<SC>::execute(Mat& in, int type, int modoOper, bool thr) {
	bool found = false;
	if (type == METHOD::OPENCV) {
		//Mat	out;
		//OpencvCalibration opencvCalib;
		//found = opencvCalib(in, out);
	}
	else {			
		Mat pre, edge, cimage;	
		preProcessing.PreProcess(in,pre,thr);		
		ellipseFinder.FindContours(pre, edge);	
		imshow("Los bordes",edge);
		bool ellipsesThresh = ellipseFinder.FindEllipse(edge, cimage, frame_counter, new_centroid, maximun_pattern_distance, setup_information.pattern_type, setup_information.pattern_points_size);	
		
		if (ellipsesThresh == true) {
		
			cout << "here" << endl;
			boxFinal = ellipseFinder.finalPoints;
			boxFinal.Calculate();
			
			if (boxFinal.points.size() == setup_information.pattern_points_size) {				

				Mat tmp;

				patternFinder.SetPatternFinder(boxFinal);
				patternFinder.FindCentersAndConvex(cimage, tmp);
				//imshow("Centros y convex", tmp);

				maximun_distance = patternFinder.maxRadio;
				maximun_pattern_distance = patternFinder.maxDistance;
				new_centroid = patternFinder.centroid;

				patternFinder.FindPattern(in, cimage, frame_counter, lost_frames);
				// showing pattern
				imshow(video_name.c_str(), cimage);

				if (modoOper == CALIBRATE) {
					ostringstream oss;
					oss << total_frame_counter;
					string ntotal_frame_counter = oss.str();
					string path = "samples/" + video_name + string("/");
					string imageFile = ntotal_frame_counter + string(".png");
					string dataFile = string("puntos.data");

					if (setup_information.select_frame_flag == DENSITY) {

						Point2f mean_value(0, 0);

						for (int i = 0; i < patternFinder.patternGenerated.points.size(); i++) {
							mean_value += patternFinder.patternGenerated.points[i];
						}
						mean_value.x /= patternFinder.patternGenerated.points.size();
						mean_value.y /= patternFinder.patternGenerated.points.size();

						double dist_min = std::numeric_limits<double>::max();
						int pos = -1;
						for (int k = 0; k < selection_frame_mesh.size(); k++) {
							double dist = cv::norm(mean_value - selection_frame_mesh[k].first);
							if (dist < dist_min) {
								dist_min = dist;
								pos = k;
							}
						}

						//cout << frame_counter << ": " << patternFinder.patternGenerated.points.size() << " mean:" << mean_value << " pos:" << pos << endl;

						FramePattern fp;
						fp.mean = mean_value;
						fp.min_dist = dist_min;
						fp.pattern_points = patternFinder.patternGenerated.points;
						fp.path = path;
						fp.imageFileName = imageFile;
						fp.dataFileName = dataFile;
						in.copyTo(fp.frame);
						//cimage.copyTo(fp.frame);
						selection_frame_mesh[pos].second.push_back(fp);
					}

					if (setup_information.select_frame_flag == NORMAL) {
						if (total_frame_counter % selection_frame_step == 0)
							selectFrame(in, path, imageFile, dataFile, patternFinder.patternGenerated.points);
					}
				}
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
			imshow("lost frame", in);
		}
	}
	return found;
}

//------------------------------------------------------------
template <typename SC>
void App<SC>::selectFrame(Mat& in, string &path, string &nameImage, string &fileName, vector<Point2f> &Puntos) {
	
	imwrite(path + nameImage, in);
	
	selectFrame(path, fileName, Puntos);
	
	ofstream file;
	string finalFileName = path+ string("f") + fileName;
	file.open(finalFileName.c_str(), ios::out | ios::app);
	file << nameImage<<endl;
	file.close();

}

template <typename SC>
void App<SC>::selectFrame(string &path, string &fileName, vector<Point2f> &Puntos) {
	
	ofstream file;
	string finalFileName = path + fileName;
	cout << finalFileName << endl;
	file.open(finalFileName.c_str(), ios::out | ios::app);
	for (int i = 0; i < Puntos.size(); i++) {
		file << Puntos[i].x << "," << Puntos[i].y << endl;
	}
	file.close();
}

//------------------------------------------------------------
template <typename SC>
void App<SC>::saveDistances(string &fileName, int frame, double distancia) {
	ofstream file;
	file.open(fileName, ios::out | ios::app);
	file << frame << "," << distancia << endl;
	file.close();
}

//------------------------------------------------------------
template <typename SC>
void App<SC>::calcBoardCornerPositions(Size boardSize, float squareSize, vector<Point3f>& corners, Size imgSize, int pattern_type) {
	corners.clear();
	Mat imagen(imgSize, CV_32FC3);
	switch (pattern_type) {
	case SIMETRICO:
		for (int i = 0; i < boardSize.height; ++i)
			for (int j = 0; j < boardSize.width; ++j)
				corners.push_back(Point3f(j*squareSize, i*squareSize, 0));
		break;
	case ASIMETRICO:
		for (int i = 0; i < boardSize.height; i++)
			for (int j = 0; j < boardSize.width; j++)
				corners.push_back(Point3f((2 * j + i % 2)*squareSize, i*squareSize, 0));
		break;
	default:
		break;
	}

	Point2f punto1, punto2;
	for (int i = 0; i < corners.size() - 1; i++) {
		punto1.x = corners[i].x;	punto1.y = corners[i].y;
		circle(imagen, punto1, 5, Scalar(255, 255, 225), 1);

		punto2.x = corners[i + 1].x;	punto2.y = corners[i + 1].y;
		circle(imagen, punto2, 5, Scalar(255, 255, 225), 1);

		line(imagen, punto1, punto2, Scalar(255, 255, 255), 1);

	}
}

template <typename SC>
vector<Point2f> App<SC>::GeneratePointsForFrontoParallel(int m, int n, int type, Size imgSize)
{
	Point2f p1, p2, p3, p4, p5=Point2f(imgSize.width/2,imgSize.height/2);		
	
	float xx, yy, distance_between_points;	
	T_INT mtmp=m;	
	if (type == ASIMETRICO)
		mtmp = m * 2;

	distance_between_points = (float)imgSize.height / ((float)mtmp+0.75);	
	xx = (distance_between_points * (n - 1));
	if (type == ASIMETRICO)
		yy = (distance_between_points*(m - 1) * 2);
	else
		yy = (distance_between_points*(m - 1));

	p1.x = 0; p1.y = 0;
	p2.x = xx; p2.y = 0;
	p3.x = xx; p3.y = yy;
	p4.x = 0; p4.y = yy;

	vector<Point2f> tempPoints;
	
	PatternFinder<T_DOUBLE> patternFinder;
	if (type == ASIMETRICO)
		tempPoints = patternFinder.GeneratePattern(m, n, p4, p1, p2, p3, type);
	else
		tempPoints = patternFinder.GeneratePattern(m, n, p1, p2, p3, p4, type);
	
	return patternFinder.MovePattern(tempPoints,patternFinder.FindCentroid(tempPoints), p5);
}

//------------------------------------------------------------
template <typename SC>
vector<Point3f> App<SC>::GenerarObjectPoint(int m, int n, int distance_between_points, int pattern_type, Size imgSize) {
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

	for (int i = 0; i < tam;i++) {
		punto.x = tempPoints[i].x;
		punto.y = tempPoints[i].y;
		punto.z = 0.0;
		puntostmp.push_back(punto);
	}
	return puntostmp;
}

//------------------------------------------------------------
template <typename SC>
vector<vector<Point3f> > App<SC>::GenerarObjectPoints(vector<Point3f> puntostmp, int nroImagePoints) {
	vector< vector<Point3f> > puntos;
	for (int i = 0; i < nroImagePoints;i++)
		puntos.push_back(puntostmp);
	return puntos;
}

//------------------------------------------------------------
template <typename SC>
Mat App<SC>::DibujarNubePuntos(vector<vector<Point2f> > puntos, Size imgSize) {
	Mat nube(imgSize, CV_32FC3);
	for (int i = 0;i < puntos.size();i++)
		for (int j = 0;j < puntos[i].size();j++)
			circle(nube, puntos[i][j], 2, colorTab[i % 11], 1);
	return nube;
}

template <typename SC>
Mat App<SC>::DibujarNubePuntosArrowed(vector<vector<Point2f> > puntos) {
	
	Mat nube(imgage_size, CV_32FC3);
	Point2f p_before(0, 0);
	bool first_time = true;
	
	for (int i = 0; i < points.size(); i++) {
		int R = rand() % 255;
		int G = rand() % 255;
		int B = rand() % 255;
		Point2f middle_mean(0, 0);
		for (int k = 0; k < points[i].size();k++) {
			middle_mean += points[i][k];
			circle(selected_frames, points[i][k], 2, Scalar(R, G, B, .5));
		}
		middle_mean.x /= points[i].size();
		middle_mean.y /= points[i].size();
		circle(selected_frames, middle_mean, 5, Scalar(R, G, B), 0);
		if (first_time == true) {
			p_before = middle_mean;
			first_time = false;
		}
		else {
			cv::arrowedLine(selected_frames, p_before, middle_mean, Scalar(R, G, B));
			p_before = middle_mean;
		}
	}	
}

//------------------------------------------------------------
template <typename SC>
vector< vector<cv::Point2f> > App<SC>::GenerarImagePoints(string path, string file, int nroImagePoints, bool ankur) {
	
	vector< vector<Point2f> > points;
	vector<Point2f> pointstmp;	
	vector<string> imgNames;

	string fileName = path + file; // los puntos
	string fileNameAnkur = path + String("ankur_") + file;
	string fileNameI = path + string("f") + file; // los nombres de las imagenes

	ifstream filePoints;
	ifstream fileImgNames;
	fileImgNames.open(fileNameI.c_str());
	if (!ankur) // si no ankur se lee todos los putnos preseleccioandos
		filePoints.open(fileName.c_str());
	else // solo se lee los puntos que corresponden para ankur
		filePoints.open(fileNameAnkur.c_str());
	
	const int dim = 256;
	char linea[dim], temp[dim];
	Point2f punto;

	int indice = 0;
	int nroframes = 0;

	while (!filePoints.eof()) {
		filePoints.getline(linea, dim, '\n');
		if (strlen(linea) > 0) {
			GetCoordenadas(linea, punto);
			pointstmp.push_back(punto);

			if (++indice == setup_information.pattern_points_size) {
				points.push_back(pointstmp);
				fileImgNames.getline(linea, dim, '\n');
				imgNames.push_back(string(linea));
				pointstmp.clear();
				indice = 0;
				nroframes++;
			}
		}
	}
	filePoints.close();
	fileImgNames.close();		

	T_INT intervalo = 1; // si ankur se trabaja con el intervalo 1 de uno a uno lee los archivos	
	if (!ankur) { // si no ankur entonces se trabaja con un intervalo que permite preseleccionar
		T_DOUBLE interv = (double)points.size() / (double)setup_information.frames_per_calibration;
		intervalo = interv;
		if (interv > (double)intervalo)
			intervalo++;
	}
	
	cout <<" Nro frames:"<< nroframes<<" intervalo: "<<intervalo<<" Frames para calibrar: "<< setup_information.frames_per_calibration <<endl;
	
	// Desde aca leo todos los putnos necesarios
	vector< vector<Point2f> > pointsfinal;	

	string fileNameIS = path + string("s") + file;
	ofstream fileImgNamesSel;
	ofstream fileAnkurPuntos;
	
	if (!ankur)// Se crea los archivos para guardar los puntoos Ankur, y el nombre de las imagenes seleccioandas
	{
		fileImgNamesSel.open(fileNameIS);
		fileAnkurPuntos.open(fileNameAnkur);
	
		int npoints = points.size();	
		for (int i = 0, j = 0; i < setup_information.frames_per_calibration; i++) {
			if (i % 2 == 0) {
				pointsfinal.push_back(points[j*intervalo]);	
				fileImgNamesSel << imgNames[j*intervalo] << endl;
				for (int k = 0; k < points[j*intervalo].size(); k++) {
					fileAnkurPuntos << points[j*intervalo][k].x << "," << points[j*intervalo][k].y << endl;
				}	
			}
			else {
				pointsfinal.push_back(points[npoints - 1 - (j)*intervalo]);
				fileImgNamesSel << imgNames[npoints - 1 - (j)*intervalo] << endl;
				for (int k = 0; k < points[npoints - 1 - (j)*intervalo].size(); k++) {
					fileAnkurPuntos << points[npoints - 1 - (j)*intervalo][k].x << "," << points[npoints - 1 - (j)*intervalo][k].y << endl;
				}				
				j++;
			}
		}
	
		fileAnkurPuntos.close();
		fileImgNamesSel.close();
	}
	else {
		for (int i = 0, j = 0; i < points.size(); i++) // deberia ser igual a setup_information.frames_per_calibration, pero se pierden frames.
			pointsfinal.push_back(points[i]);			
	}


	//cout <<"tengo de estos frames esta cantidad   "<< pointsfinal.size()<<endl;
	return pointsfinal;
}

//------------------------------------------------------------
template <typename SC>
void App<SC>::ClearFile(string fileName) { // funcion modificada porque se estaba gruardando al inf de los puntos finales, y deberia ser del padron generado
	ofstream file;
	file.open(fileName);
	file.close();
}

//------------------------------------------------------------
template <typename SC>
void App<SC>::GetCoordenadas(char linea[], Point2f & punto) {
	const int dim = 256;
	char temp[dim];
	int j = 0, i;
	for (i = 0;i < strlen(linea);i++) {
		if (linea[i] == ',') {
			j = i + 1;
			temp[i] = '\0';
			punto.x = atof(temp);
		}
		else
			temp[i - j] = linea[i];
	}
	temp[i - j] = '\0';
	punto.y = atof(temp);
}

#endif