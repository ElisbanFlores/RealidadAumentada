#pragma once

//#include "CImg.h"
#include <string>
#include <GL\glew.h>
#include <cassert>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class Texture
{
private:
	GLuint m_texture;

public:
	Texture() {}
	Texture(const std::string& fileName);
	Texture(cv::Mat image);
	void InitTexture(const std::string& fileName);
	void InitTexture(cv::Mat image);
	void Bind(unsigned int unit);
	~Texture();
};


