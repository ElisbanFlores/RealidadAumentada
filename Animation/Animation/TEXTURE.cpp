#include "Texture.h"

Texture::Texture(const std::string& fileName)
{
	cv::Mat image;
	image = cv::imread(fileName.c_str(), CV_LOAD_IMAGE_COLOR);
	InitTexture(image);
}

Texture::Texture(cv::Mat image)
{
	InitTexture(image);
}

void Texture::InitTexture(const std::string& fileName)
{
	cv::Mat image;
	image = cv::imread(fileName.c_str(), CV_LOAD_IMAGE_COLOR);
	//cv::flip(image, image, 1);
	InitTexture(image);
}

void Texture::InitTexture(cv::Mat image)
{
	
	cvtColor(image, image, CV_BGR2RGB);

	int width = image.cols;
	int height = image.rows;	
	unsigned char *imageData = image.ptr();

	if (imageData == NULL)
		std::cerr << "Texture loading failed for texture: " << std::endl;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

}

void Texture::Bind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}
