
#pragma once

//#include "OPENGL.h"
#include <fstream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>

#define MAX_BONES 33

struct ShaderMatrices
{
	glm::mat4 mWorld;
	glm::mat4 mView;
	glm::mat4 mProj;
};

class ShaderAnim
{
public:
	ShaderAnim();
	~ShaderAnim();

	bool Init(char*, char*);
	void Shutdown();
	void SetShader();
	bool SetShaderParameters(ShaderMatrices*, glm::mat4*);

private:
	const char* ReadShaderFromFile(char*);
	void OutputShaderError(unsigned int);
	void OutputLinkerError(unsigned int);

private:
	bool InitShaders(char*, char*);
	void ShutdownShaders();

private:
	unsigned int m_iProgramId;
	unsigned int m_iVertexShaderId;
	unsigned int m_iFragmentShaderId;
};

