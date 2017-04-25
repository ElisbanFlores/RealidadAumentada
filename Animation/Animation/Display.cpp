#include "Display.h"

Display::Display()
{

}

Display::Display(int width, int height, std::string& title)
{
	Init(width, height, title);
}

int Display::Init(int width, int height, std::string& title)
{
	if (!glfwInit())
	{
		std::cerr << "Failde to initialize GLFW" << std::endl;
		return 0;
	}

	w = width;
	h = height;

	//Anti-aliasing
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // opengl 4.4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

	if (m_window == NULL)
	{
		std::cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version." << std::endl;
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(m_window);

	// Inicializar GLEW

	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Glew failed to initialize" << std::endl;
		return 0;
	}

	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	m_isClosed = false;
	glViewport(0, 0, width, height);
	return 1;
}


void Display::Clear(cv::Mat frame)
{
	glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, frame.ptr());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Display::Clear(float r, float g, float b, float a)
{	
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
}

void Display::Update()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();	
	if (glfwWindowShouldClose(m_window) != 0)
		m_isClosed = true;	
}

bool Display::IsClosed()
{
	return m_isClosed;
}

Display::~Display()
{
	glfwTerminate();
}
