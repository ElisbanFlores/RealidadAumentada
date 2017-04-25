#pragma once

#include "Display.h"
#include "ObjectOGL.h"
#include "PatternTracker.h"

#include "ShaderAnim.h"
#include "MD5MODEL.h"

class ObjectAnim
{

public:
	//Variables para la animacion
	MD5Model* m_pModel;
	ShaderAnim* m_pShader;
	ShaderMatrices* m_pShaderMatrices;
	ObjectAnim() {
		// Cargar los shaders
		m_pShader = new ShaderAnim();
		m_pShader->Init("VertexShader.glsl", "FragmentShader.glsl");
		// Cargar el modelo y la animacion
		m_pModel = new MD5Model();
		m_pModel->Init("Assets\\Models\\vaquero.md5mesh", "Assets\\Animations\\vaquero.md5anim");
	}

	ObjectAnim(std::string mesh, std::string anim)
	{
		// Cargar los shaders
		m_pShader = new ShaderAnim();
		m_pShader->Init("VertexShader.glsl", "FragmentShader.glsl");
		// Cargar el modelo y la animacion
		m_pModel = new MD5Model();
		m_pModel->Init(mesh, anim);
	}

	~ObjectAnim() {	}

};



// Solo la animacion

int main()
{
	/////////////////////////////////////////////////
	// PARA OPENCV
	/////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	//Parametros de Camara Micronics - USB2.0
	////////////////////////////////////////////////
	double camD[] = { 1066.55, 0., 210.925,
		0.,	1055.94, 320.292,
		0., 0., 1. };

	double distCoeffD[] = { -0.377108, 1.40182,	0, 0, -2.90869 };
	/*
	double camD[] = { 700.226, 0., 303.429,
		0.,	706.457, 209.038,
		0., 0., 1. };

	double distCoeffD[] = { 0.276822, -1.35345,	0, 0, 2.68112 };
	*/

	Mat camera_matrix = Mat(3, 3, CV_64FC1, camD);
	Mat distortion_coefficients = Mat(5, 1, CV_64FC1, distCoeffD);
	Mat map1, map2, frame;
	VideoCapture cap(1); // open the default camera
	if (!cap.isOpened()) {
		cout << "Ups, connect the cam please....";
		getchar();
		return -1;
	}
	float width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	float height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	// Definir el Buscador de Patron
	PatternTracker patternTracker(width, height, 3, 4, 22.09);
	initUndistortRectifyMap(camera_matrix, distortion_coefficients, Mat(), camera_matrix, Size(width, height), CV_16SC2, map1, map2);

	/////////////////////////////////////////////////
	// PARA OPENGL
	/////////////////////////////////////////////////

	//Declarar Ventana penGL
	Display window(width, height, std::string("Augmente Reality - Animation"));


	///////////////
	//Animacion
	///////////////

	//Variables para la animacion
	MD5Model* m_pModel;
	ShaderAnim* m_pShader;
	ShaderMatrices* m_pShaderMatrices;

	// Cargar los shaders
	m_pShader = new ShaderAnim();
	m_pShader->Init("VertexShader.glsl", "FragmentShader.glsl");

	// Cargar el modelo y la animacion
	m_pModel = new MD5Model();
	m_pModel->Init("Assets\\Models\\boblampclean.md5mesh", "Assets\\Animations\\boblampclean.md5anim");
	Transform transformAnim;
	//shader matrices
	m_pShaderMatrices = new ShaderMatrices();

	///////////////////
	// Otros Objetos
	//////////////////

	/////////////////////////////////
	// Para el fondo
	///////////////////////////////////
	Vertex vertices[] = {

		Vertex(glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(1.0f,-1.0f,-1.0f),glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(1.0f, 1.0f,-1.0f),glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(1.0f, 1.0f,-1.0f),glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(-1.0f, 1.0f,-1.0f),glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)) };

	unsigned int indices[] = { 0,1,2,3,4,5 };
	ObjectOGL fondo(vertices, indices, 6, "shaders/basicshader");
	ObjectOGL cubo1("modelos/cubo2.obj", "shaders/basicshader", "textures/cubohielo.png");
	ObjectOGL cubo2("modelos/cubo2.obj", "shaders/basicshader", "textures/cubomadera.png");

	//Definir Camaras
	CameraOGL cameraOGLFondo(glm::vec3(0, 0, 0), 45.0f, width / height, 0.01f, 10000.0f);

	CameraOGL cameraOGL(camera_matrix, distortion_coefficients, Size(width, height), glm::vec3(0, 0, 0), 0.01f, 10000.0f);

	m_pShaderMatrices->mProj = cameraOGL.GetProjection();
	m_pShaderMatrices->mView = cameraOGL.GetPerspective();

	float counter = 0.0f, tiempototal = 0.0;

	float paso = 0.05;
	float inc = paso;
	float angle = 0.0f;
	float escalaAnim = 1;

	
	////////////////////////////////////////////////
	//Parametros de Camara Toshiba
	////////////////////////////////////////////////

	double camDT[] = { 700.226, 0., 303.429,
		0.,	706.457, 209.038,
		0., 0., 1. };

	double distCoeffDT[] = { 0.276822, -1.35345,	0, 0, 2.68112 };

	Mat camera_matrixT = Mat(3, 3, CV_64FC1, camDT);
	Mat distortion_coefficientsT = Mat(5, 1, CV_64FC1, distCoeffDT);

	Mat map1T, map2T, frameT;
	VideoCapture capT(0); // open the default camera
	if (!capT.isOpened()) {
		cout << "Ups, I can not open this device";
		getchar();
		return -1;
	}
	float widthT = capT.get(CV_CAP_PROP_FRAME_WIDTH);
	float heightT = capT.get(CV_CAP_PROP_FRAME_HEIGHT);

	// Definir el Buscador de Patron
	PatternTracker patternTrackerT(widthT, heightT, 3, 4, 22.09);
	initUndistortRectifyMap(camera_matrixT, distortion_coefficientsT, Mat(), camera_matrixT, Size(widthT, heightT), CV_16SC2, map1T, map2T);

	while (!window.IsClosed())
	{
		auto start = std::chrono::high_resolution_clock::now();
		
		
		/////////////////////////////////////////////////
		// PARA LA CAMARA TOSHIBA
		////////////////////////////////////////////////

		//////////////////////////////
		// Parte OpenCV
		//////////////////////////////
		//Leer imagenes
		Mat frametmpT;
		capT >> frametmpT;
		if (frametmpT.cols == 0)	break;
				
		Mat frametmp;
		cap >> frametmp;
		if (frametmp.cols == 0)	break;

		// Prorcesar imagenes

		Mat tvecT, rvecT, rvecRodT;

		double distan = 0;
		if (frametmpT.rows) {
			remap(frametmpT, frameT, map1T, map2T, INTER_LINEAR);
			distan = patternTrackerT.GetRotTra(frametmpT, camera_matrixT, distortion_coefficientsT, tvecT, rvecT, rvecRodT);
			frametmpT.copyTo(frameT);
			imshow("Control", frameT);
		}
		

		/////////////////////////////////////////////////
		// PARA LA CAMARA MICRONICS
		////////////////////////////////////////////////
				
		//////////////////////////////
		// Parte OpenCV
		//////////////////////////////
				
		Mat tvec, rvec, rvecRod;

		//double distan = 0;
		if (frametmp.rows) {
			remap(frametmp, frame, map1, map2, INTER_LINEAR);
			distan = patternTracker.GetRotTra(frametmp, camera_matrix, distortion_coefficients, tvec, rvec, rvecRod);
			frametmp.copyTo(frame);			
		}

		//////////////////////////////
		// Parte OpenGL
		//////////////////////////////	

		window.Clear(0.5f, 0.15f, 0.3f, 1.0f);
		//window.Clear(frame);
		////////////////////////
		// Realidad Aumentada
		/////////////////////////
				
		float prof = 1000;
		float scala = prof / ((camera_matrix.at<double>(0, 0) + camera_matrix.at<double>(1, 1)) / 2);
		glm::vec3 scale(width / scala, height / scala, 1);
		fondo.transform.GetPos().z = -scala*prof;
		fondo.transform.SetScale(scale);
			
		fondo.transform.CalculateModel();
		fondo.Draw(frame, cameraOGLFondo);

		if (!tvec.empty()) {

			std::cout << tvec << std::endl;
					
			////////////////////
			//Animacion
			///////////////////
			float dt = (1.0f / 10.0f);
			

			// Para el control del patron
		
			switch (patternTrackerT.nCircles)
			{
				case 11:
					inc = -paso;
					break;
				case 10:
					inc = +paso;
					break;
				case 9:
					m_pModel->Init("Assets\\Models\\vaquero.md5mesh", "Assets\\Animations\\vaquero.md5anim");
					escalaAnim = 8;

					break;
				case 8:
					m_pModel->Init("Assets\\Models\\boblampclean.md5mesh", "Assets\\Animations\\boblampclean.md5anim");
					escalaAnim = 1;
					break;
				case 7:
					break;
				case 6:
					break;

				default:
					break;
			}
			
			/*
			if (glfwGetKey(window.m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) inc = -paso;
			if (glfwGetKey(window.m_window, GLFW_KEY_LEFT) == GLFW_PRESS) inc = +paso;
			if (glfwGetKey(window.m_window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				m_pModel->Init("Assets\\Models\\vaquero.md5mesh", "Assets\\Animations\\vaquero.md5anim");
				escalaAnim = 8;
			}
			if (glfwGetKey(window.m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				m_pModel->Init("Assets\\Models\\boblampclean.md5mesh", "Assets\\Animations\\boblampclean.md5anim");
				escalaAnim = 1;
			}
			*/
			angle += inc;
						
			float ML_PI = 3.1415f;
			
			transformAnim.CalculateModelFromOpenCV(tvec, rvecRod, rvec);
			m_pShaderMatrices->mWorld = transformAnim.GetModel();
			m_pShaderMatrices->mWorld = glm::translate(m_pShaderMatrices->mWorld, glm::vec3(33,22,0));
			m_pShaderMatrices->mWorld = glm::rotate(m_pShaderMatrices->mWorld, -ML_PI, glm::vec3(1,0,0));
			m_pShaderMatrices->mWorld = glm::rotate(m_pShaderMatrices->mWorld, -ML_PI*angle, glm::vec3(0, 0, 1));
			m_pShaderMatrices->mWorld = glm::scale(m_pShaderMatrices->mWorld, glm::vec3(escalaAnim, escalaAnim, escalaAnim));

			m_pModel->Update(dt);
			
			m_pShader->SetShader();
			m_pShader->SetShaderParameters(m_pShaderMatrices, m_pModel->GetSkinnedMatrices());
			m_pModel->Render();
			
			//cubo1.transform.CalculateModelFromOpenCV(tvec, rvecRod, rvec);			
			//cubo1.Draw(cameraOGL);			
		}
		//cubo1.DrawCV(cameraOGL);
		window.Update();
		
		auto finish = std::chrono::high_resolution_clock::now();
		double tiempo = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000.0;
		tiempototal += tiempo;
		cout << tiempo << "ms" << endl;
		counter += 1.0f;
	}

	cout << endl;
	cout << tiempototal / counter << "ms promedio por frame" << endl;
	cout << 1000 / (tiempototal / counter) << "fps aprox.." << endl;
	getchar();

	return 1;
}





// Para la realidad aumentada
/*
int main2()
{
	/////////////////////////////////////////////////
	// PARA OPENCV
	/////////////////////////////////////////////////
	
	//Parametros de Camara
	
	double camD[] = { 1012.81, 0., 647.663,
		0.,	1011.75, 333.743,
		0., 0., 1. };

	double distCoeffD[] = { 0.087675, -0.279504,	0, 0, 0.319087 };
	
	
	//double camD[] = { 1066.55, 0., 210.925,
	//	0.,	1055.94, 320.292,
	//	0., 0., 1. };

	//double distCoeffD[] = { -0.377108, 1.40182,	0, 0, -2.90869 };
	
	

	Mat camera_matrix = Mat(3, 3, CV_64FC1, camD);
	Mat distortion_coefficients = Mat(5, 1, CV_64FC1, distCoeffD);

	// Undistort Parameters
	Mat map1, map2, frame;

	//Abrir el Video
	//VideoCapture cap("PadronAnillos_03.avi"); // open the default camera
	VideoCapture cap("Prueba.wmv"); // open the default camera
	//VideoCapture cap(1);
	if (!cap.isOpened()) {
		cout << "No encuentro esse video....";
		getchar();
		return -1;
	}

	float width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	float height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	// Definir el Buscador de Patron
	PatternTracker patternTracker(width, height, 3,4, 22.09);


	initUndistortRectifyMap(camera_matrix, distortion_coefficients, Mat(), camera_matrix, Size(width, height), CV_16SC2, map1, map2);	

	/////////////////////////////////////////////////
	// PARA OPENGL
	/////////////////////////////////////////////////

	//Declarar Ventana penGL
	Display window(width, height, std::string("Augmente Reality - Animation"));
	
	///////////////
	//Animacion
	///////////////

	//Variables para la animacion
	MD5Model* m_pModel;
	ShaderAnim* m_pShader;
	ShaderMatrices* m_pShaderMatrices;
	
	// Cargar los shaders
	m_pShader = new ShaderAnim();	
	m_pShader->Init("VertexShader.glsl", "FragmentShader.glsl");
	
	// Cargar el modelo y la animacion
	m_pModel = new MD5Model();	
	m_pModel->Init("Assets\\Models\\vaquero.md5mesh", "Assets\\Animations\\vaquero.md5anim");
	
	//shader matrices
	m_pShaderMatrices = new ShaderMatrices();	
	mlIdentityMat4(m_pShaderMatrices->mWorld);
	
	

	float3 eye, target, up;
	mlSetVec3(eye, 0.0f, 25.0f, 100.0f);
	mlSetVec3(target, 0.0f, 25.0f, 0.0f);
	mlSetVec3(up, 0.0f, 1.0f, 0.0f);
		
	mlLookAtMat4(m_pShaderMatrices->mView, eye, target, up);

	mat4 m_mProj;
	mlIdentityMat4(m_mProj);

	float aspect = static_cast<float>(width) / static_cast<float>(height);
	mlPerspective(m_mProj, ML_PI*  0.25f, aspect, 0.01f, 1000.0f);
	
	///////////////////
	// Otros Objetos
	//////////////////

	//Cargar Modelos
	ObjectOGL cubo1("modelos/cubo2.obj", "shaders/basicshader", "textures/cubopiedra.png");
	ObjectOGL cubo2("modelos/cubo2.obj", "shaders/basicshader", "textures/cubomadera.png");
	ObjectOGL cubo3("modelos/cubo2.obj", "shaders/basicshader", "textures/cubohielo.png");


	// Para el fondo
	
	Vertex vertices[] = { 
	
		Vertex(glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(1.0f,-1.0f,-1.0f),glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(1.0f, 1.0f,-1.0f),glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(1.0f, 1.0f,-1.0f),glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(-1.0f, 1.0f,-1.0f),glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		Vertex(glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)) };
	
	
	
	unsigned int indices[] = { 0,1,2,3,4,5 };
	

	ObjectOGL fondo(vertices, indices, 6, "shaders/basicshader");

	//Definir Camaras
	CameraOGL cameraOGLFondo(glm::vec3(0, 0, 0), 45.0f, width / height, 0.01f, 10000.0f);
	CameraOGL cameraOGL(camera_matrix, distortion_coefficients, Size(width, height), glm::vec3(0, 0, 0), 0.01f, 10000.0f);

	float counter = 0.0f, tiempototal = 0.0;
	
	float paso = 0.005;
	float inc = paso;
	float angle = 0.0f;
	
	while(!window.IsClosed())
	{
		
		//////////////////////////////
		// Parte OpenCV
		//////////////////////////////
		Mat frametmp;
		cap >> frametmp;
		if (frametmp.cols == 0)
			break;
		//cv::flip(frametmp, frametmp, 1);


		Mat tvec, rvec, rvecRod;

		auto start = std::chrono::high_resolution_clock::now();

		double distan = 0;
		if (frametmp.rows) {
			remap(frametmp, frame, map1, map2, INTER_LINEAR);
			distan = patternTracker.GetRotTra(frametmp, camera_matrix, distortion_coefficients, tvec, rvec, rvecRod);
			frametmp.copyTo(frame);			
		}

		//////////////////////////////
		// Parte OpenGL
		//////////////////////////////	
				
		window.Clear(0.5f, 0.15f, 0.3f, 1.0f);
		//window.Clear(frame);
		////////////////////////
		// Realidad Aumentada
		/////////////////////////

		//glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, frame.ptr());
		float prof = 1000;
		float scala = prof/((camera_matrix.at<double>(0,0)+ camera_matrix.at<double>(1, 1))/2);
		glm::vec3 scale(width / scala, height / scala, 1);
		fondo.transform.GetPos().z = -scala*prof;
		fondo.transform.SetScale(scale);
		
		//glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, frame.ptr());

		fondo.transform.CalculateModel();
		
		fondo.Draw(frame, cameraOGLFondo);
		



		if (!tvec.empty()) {

			std::cout << tvec << std::endl;
			double esc = 25.5 / 3;
			cubo1.transform.GetScale().x = esc;
			cubo1.transform.GetScale().y = esc;
			cubo1.transform.GetScale().z = esc;

			cubo2.transform.GetScale().x = esc;
			cubo2.transform.GetScale().y = esc;
			cubo2.transform.GetScale().z = esc;

			cubo1.transform.GetScale().x = esc;
			cubo1.transform.GetScale().y = esc;
			cubo1.transform.GetScale().z = esc;

			cubo3.transform.GetScale().x = esc;
			cubo3.transform.GetScale().y = esc;
			cubo3.transform.GetScale().z = esc;

			
			cubo1.transform.GetPos() = glm::vec3(25.5, 0.0, 0.0);
			cubo1.transform.CalculateModelFromOpenCV(tvec, rvecRod,rvec);

			cubo2.transform.GetPos() = glm::vec3(0.0, 25.5, 0.0);
			cubo2.transform.CalculateModelFromOpenCV(tvec, rvecRod,rvec);
			
			cubo3.transform.GetPos() = glm::vec3(25.5, 25.5, 0.0);
			cubo3.transform.CalculateModelFromOpenCV(tvec, rvecRod,rvec);
		}

		cubo1.DrawCV(cameraOGL);
		cubo2.DrawCV(cameraOGL);
		cubo3.DrawCV(cameraOGL);
		
		////////////////////
		//Animacion
		///////////////////
		float dt = (1.0f / 30.0f);

		if (glfwGetKey(window.m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) inc = -paso;
		if (glfwGetKey(window.m_window, GLFW_KEY_LEFT) == GLFW_PRESS) inc = +paso;
		if (glfwGetKey(window.m_window, GLFW_KEY_UP) == GLFW_PRESS)
			m_pModel->Init("Assets\\Models\\vaquero.md5mesh", "Assets\\Animations\\vaquero.md5anim");
		if (glfwGetKey(window.m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
			m_pModel->Init("Assets\\Models\\boblampclean.md5mesh", "Assets\\Animations\\boblampclean.md5anim");
				
		angle += inc;



		mlRotateXYZMat4(m_pShaderMatrices->mWorld, -ML_PI * 0.5f, -ML_PI *angle, 0.0f);
		
		
		mlSetMat4(m_pShaderMatrices->mProj, m_mProj);

		m_pModel->Update(dt);

		m_pShader->SetShader();
		m_pShader->SetShaderParameters(m_pShaderMatrices, m_pModel->GetSkinnedMatrices());
		
		m_pModel->Render();
		
		window.Update();	

		auto finish = std::chrono::high_resolution_clock::now();
		double tiempo = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000.0;
		tiempototal += tiempo;
		cout << tiempo << "ms" << endl;
		counter += 1.0f;
	}
	
	cout << endl;
	cout << tiempototal / counter << "ms promedio por frame" << endl;
	cout << 1000 / (tiempototal / counter) << "fps aprox.." << endl;
	getchar();

	return 1;
}

*/
// para grabar video
/*
int main()
{
	VideoCapture vcap(1);
	if (!vcap.isOpened()) {
		cout << "Error opening video device" << endl;
		return 1;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "w,h" << frame_width << "," << frame_height << endl;
	VideoWriter video("out2webcam.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);

	while (1) {

		Mat frame;
		vcap >> frame;
		video.write(frame);
		imshow("Frame", frame);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
	return 1;
}
*/


