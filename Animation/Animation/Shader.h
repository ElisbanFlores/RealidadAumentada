#pragma once

#include <string>
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include "Transform.h"
#include "CameraOGL.h"

class Shader
{
private:
	static const unsigned int NUM_SHADERS = 2;	
	enum
	{
		TRANSFORM_U,
		NUM_UNIFORMS
	};

	GLuint m_program;
	GLuint m_vertexShader;
	GLuint m_fragmentShader;
	GLuint m_uniforms[NUM_UNIFORMS];


public:
	Shader(){}
	Shader(const std::string& fileName);
	void Bind();
	void Update(const Transform& transform, const CameraOGL& cameraogl);
	void UpdateCV(const Transform& transform, const CameraOGL& cameraogl);
	~Shader();
};

