#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "obj_loader.h"


class Vertex
{

protected:

private:
	glm::vec3 pos;
	glm::vec2 textCoord;
	glm::vec3 normal;

public:
	Vertex(const glm::vec3& pos, const glm::vec2& textCoord, const glm::vec3& normal=glm::vec3(0,0,0))
	{
		this->pos = pos;
		this->textCoord = textCoord;
		this->normal = normal;
	}
	
	inline glm::vec3* GetPos(){ return &pos; }
	inline glm::vec2* GetTextCoord(){ return &textCoord; }
	inline glm::vec3* GetNormal() { return &normal; }
};

class Mesh
{
private:
	enum {
		POSITION_VB,
		TEXTCOORD_VB,
		NORMAL_VB,
		INDEX_VB,
		NUM_BUFFERS
	};

	GLuint m_vertexArrayObject;
	GLuint m_vertexArrayBuffer[NUM_BUFFERS];
	unsigned int m_drawCount;
	void InitMesh(const IndexedModel& model);


public:
	Mesh(){}
	Mesh(const std::string& fileName);
	Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
	void Draw();
	~Mesh();

};

