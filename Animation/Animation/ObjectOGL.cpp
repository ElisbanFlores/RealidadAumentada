#include "ObjectOGL.h"

ObjectOGL::ObjectOGL()
{
	mesh = new Mesh("cubo.obj");
	shader = new Shader("basicshader");
	texture = new Texture("cubopiedra.png");
}

ObjectOGL::ObjectOGL(std::string nobj, std::string nshader, std::string ntexture)
{
	mesh = new Mesh(nobj);
	shader = new Shader(nshader);
	texture = new Texture(ntexture);
}

ObjectOGL::ObjectOGL(Vertex *vertices, unsigned int *indices, int n, std::string nshader)
{
	//mesh = new Mesh(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));		
	mesh = new Mesh(vertices, n, indices, n);
	shader = new Shader(nshader);
	texture = new Texture;
}

void ObjectOGL::Draw(CameraOGL camera)
{
	shader->Bind();
	texture->Bind(0);
	shader->Update(transform, camera);
	mesh->Draw();
}

void ObjectOGL::Draw(cv::Mat frame, CameraOGL camera)
{
	delete texture;
	texture = new Texture(frame);
	shader->Bind();
	texture->Bind(0);
	shader->Update(transform, camera);
	mesh->Draw();

	//cv::waitKey();
}

void ObjectOGL::DrawCV(CameraOGL camera)
{
	shader->Bind();
	texture->Bind(0);
	shader->UpdateCV(transform, camera);
	mesh->Draw();
}

void ObjectOGL::DrawCV(cv::Mat frame, CameraOGL camera)
{
	delete texture;
	texture = new Texture(frame);
	shader->Bind();
	texture->Bind(0);
	shader->UpdateCV(transform, camera);
	mesh->Draw();

	//cv::waitKey();
}


ObjectOGL::~ObjectOGL()
{

}