#pragma once
#include <string>
#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


class Display
{
public:	
	GLFWwindow* m_window;	
	bool m_isClosed;
	int w, h;

public:
	Display();
	Display(int, int, std::string&);
	~Display();
	int Init(int, int, std::string&);
	void Update();
	void Clear(float, float, float, float);
	void Display::Clear(cv::Mat frame);
	bool IsClosed();
};

