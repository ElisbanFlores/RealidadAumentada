#include "Shader.h"

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
static std::string LoadShader(const std::string& FileName);
static GLuint CreateShader(const std::string& text, GLenum shaderType);

Shader::Shader(const std::string& fileName)
{
	m_program = glCreateProgram();
	m_vertexShader = CreateShader(LoadShader(fileName+".vs"), GL_VERTEX_SHADER);
	m_fragmentShader = CreateShader(LoadShader(fileName+".fs"), GL_FRAGMENT_SHADER);
		
	glAttachShader(m_program, m_vertexShader);
	glAttachShader(m_program, m_fragmentShader);
	
	glBindAttribLocation(m_program, 0, "position");
	glBindAttribLocation(m_program, 1, "texCoord");
	glBindAttribLocation(m_program, 2, "normal");
	
	glLinkProgram(m_program);

	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error: Program linking failed... ");

	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Error: Program is invalid... ");

	m_uniforms[TRANSFORM_U] = glGetUniformLocation(m_program, "transform");

}

void Shader::Bind()
{
	glUseProgram(m_program);
}


void Shader::UpdateCV(const Transform& transform, const CameraOGL& cameraogl) {
	glm::mat4 model = cameraogl.GetViewProjectionCV() * transform.GetModel();
	glUniformMatrix4fv(m_uniforms[TRANSFORM_U], 1, GL_FALSE, &model[0][0]);
}


void Shader::Update(const Transform& transform, const CameraOGL& cameraogl){
	glm::mat4 model = cameraogl.GetViewProjection() * transform.GetModel();
	glUniformMatrix4fv(m_uniforms[TRANSFORM_U], 1, GL_FALSE, &model[0][0]);
}

Shader::~Shader()
{
	glDetachShader(m_program, m_vertexShader);
	glDeleteShader(m_vertexShader);
	glDetachShader(m_program, m_fragmentShader);
	glDeleteShader(m_fragmentShader);
	glDeleteProgram(m_program);
}

static GLuint CreateShader(const std::string& text, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	
	if (shader == 0)
		std::cerr << "Error: Shader creation failed" << std::endl;
	
	const GLchar* shaderSourceStrings;
	
	shaderSourceStrings = text.c_str();	
	std::cout << shaderSourceStrings<<std::endl;

	glShaderSource(shader,1,&shaderSourceStrings, NULL);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: shader compilation failed... ");

	return shader;
}

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };
	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}
}

static std::string LoadShader(const std::string& FileName)
{
	std::ifstream file;
	file.open(FileName.c_str());
	
	std::string output;
	std::string line;

	if (file.is_open())	{
		while (getline(file, line))
			output+="\n" + line;		
		file.close();
	}
	else{
		std::cerr << "Unable to load shader" << FileName << std::endl;
	}
	return output;
}


