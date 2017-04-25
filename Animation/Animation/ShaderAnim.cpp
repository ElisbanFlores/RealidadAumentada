#include "ShaderAnim.h"

ShaderAnim::ShaderAnim(){
}

ShaderAnim::~ShaderAnim(){
}

bool ShaderAnim::Init(char* sVSName, char* sFSName)
{
	if(!InitShaders(sVSName, sFSName))
		return false;

	return true;
}

void ShaderAnim::Shutdown()
{
	ShutdownShaders();
}

void ShaderAnim::SetShader()
{
	glUseProgram(m_iProgramId);
}

bool ShaderAnim::SetShaderParameters(ShaderMatrices* pShaderMatrices, glm::mat4* pSkinnedMatrices)
{
	int iLocation = glGetUniformLocation(m_iProgramId, "sampler");
	if(iLocation == -1)
		return false;

	glUniform1i(iLocation, 0);

	//world
	iLocation = glGetUniformLocation(m_iProgramId, "mWorld");
	if(iLocation == -1)
		return false;
	
	glUniformMatrix4fv(iLocation, 1, false, &pShaderMatrices->mWorld[0][0]);

	//view
	iLocation = glGetUniformLocation(m_iProgramId, "mView");
	if(iLocation == -1)
		return false;

	glUniformMatrix4fv(iLocation, 1, false, &pShaderMatrices->mView[0][0]);

	//proj
	iLocation = glGetUniformLocation(m_iProgramId, "mProj");
	if(iLocation == -1)
		return false;

	glUniformMatrix4fv(iLocation, 1, false, &pShaderMatrices->mProj[0][0]);

	//skinnig
	iLocation = glGetUniformLocation(m_iProgramId, "mSkinned");
	if(iLocation == -1)
		return false;

	glUniformMatrix4fv(iLocation, MAX_BONES, false,&pSkinnedMatrices[0][0][0]);

	return true;
}

const char* ShaderAnim::ReadShaderFromFile(char* sFileName)
{
	FILE* file = fopen(sFileName, "rb");
	if(!file)
		return NULL;

	int iError = fseek(file, 0, SEEK_END);
	if(iError != 0)
		return NULL;

	long lFileSize = ftell(file);
	rewind(file);

	char* pBuffer = new char[lFileSize + 1];
	if(!pBuffer)
		return NULL;

	long lCount = fread(pBuffer, sizeof(char), lFileSize, file);
	if(lCount != lFileSize)
		return NULL;

	pBuffer[lFileSize] = '\0';

	iError = fclose(file);
	if(iError != 0)
		return NULL;

	return const_cast<const char*>(pBuffer);
}

void ShaderAnim::OutputShaderError(unsigned int iShaderId)
{
	int iSize;
	glGetShaderiv(iShaderId, GL_INFO_LOG_LENGTH, &iSize);

	char* pBuffer = new char[iSize];
	if(!pBuffer)
		return;

	glGetShaderInfoLog(iShaderId, iSize, 0, pBuffer);
	//OutputDebugStringA(pBuffer);

	delete[] pBuffer;
	pBuffer = NULL;
}

void ShaderAnim::OutputLinkerError(unsigned int iProgramId)
{
	int iBufferSize;
	glGetProgramiv(iProgramId, GL_INFO_LOG_LENGTH, &iBufferSize);

	char* pBuffer = new char[iBufferSize];
	if(!pBuffer)
		return;

	glGetProgramInfoLog(iProgramId, iBufferSize, 0, pBuffer);
	//OutputDebugStringA(pBuffer);

	delete[] pBuffer;
	pBuffer = NULL;
}

bool ShaderAnim::InitShaders(char* sVSName, char* sFSName)
{
	const char* pVertexShader = ReadShaderFromFile(sVSName);
	if(!pVertexShader)
		return false;

	const char* pFragmentShader = ReadShaderFromFile(sFSName);
	if(!pFragmentShader)
		return false;

	m_iVertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	m_iFragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(m_iVertexShaderId, 1, &pVertexShader,  0);
	glShaderSource(m_iFragmentShaderId, 1, &pFragmentShader, 0);

	glCompileShader(m_iVertexShaderId);
	glCompileShader(m_iFragmentShaderId);

	int iStatus;
	glGetShaderiv(m_iVertexShaderId, GL_COMPILE_STATUS, &iStatus);
	if(iStatus != 1){
		OutputShaderError(m_iVertexShaderId);
		return false;
	}

	glGetShaderiv(m_iFragmentShaderId, GL_COMPILE_STATUS, &iStatus);
	if(iStatus != 1){
		OutputShaderError(m_iFragmentShaderId);
		return false;
	}

	m_iProgramId = glCreateProgram();
	glAttachShader(m_iProgramId, m_iVertexShaderId);
	glAttachShader(m_iProgramId, m_iFragmentShaderId);
	glLinkProgram(m_iProgramId);

	glGetProgramiv(m_iProgramId, GL_LINK_STATUS, &iStatus);
	if(iStatus != 1){
		OutputLinkerError(m_iProgramId);
		return false;
	}

	return true;
}

void ShaderAnim::ShutdownShaders()
{
	glUseProgram(0);
	glDetachShader(m_iProgramId, m_iVertexShaderId);
	glDetachShader(m_iProgramId, m_iFragmentShaderId);

	glDeleteShader(m_iVertexShaderId);
	glDeleteShader(m_iFragmentShaderId);
	glDeleteProgram(m_iProgramId);
}
