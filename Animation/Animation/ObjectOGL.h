#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <iostream>

class ObjectOGL
{
public:
	Mesh *mesh;
	Shader *shader;
	Texture *texture;
	Transform transform;

public:
	ObjectOGL();
	~ObjectOGL();
	ObjectOGL(std::string nobj, std::string nshader, std::string ntexture);
	ObjectOGL(Vertex *vertices, unsigned int *indices, int n, std::string nshader);

	void Draw(CameraOGL camera);
	void Draw(cv::Mat frame, CameraOGL camera);	
	void DrawCV(CameraOGL camera);
	void DrawCV(cv::Mat frame, CameraOGL camera);
};


