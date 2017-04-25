#pragma once


#include "TEXTURE.h"
#include "MD5ANIM.h"
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>

struct ogl_vertex_t
{
	
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec4 indices;
	glm::vec4 weights;
	
	/*float3 position;
	float2 texCoord;
	float4 indices;
	float4 weights;*/
};

struct ogl_mesh_t
{
	std::string texture;
	ogl_vertex_t* vertexArray;
	unsigned int* indexArray;
	unsigned int numVertices;
	unsigned int numIndices;
};

struct md5_vertex_t
{	
	//float2 texCoord;
	glm::vec2 texCoord;
	int startWeight;
	int numWeights;
};

struct md5_triangle_t
{
	int indices[3];
};

struct md5_weight_t
{
	int jointId;
	float bias;
	//float3 position;
	glm::vec3 position;
};

struct md5_joint_t
{
	std::string name;
	int parentId;
	//float3 position;
	//float4 orientation;
	glm::vec3 position;
	glm::vec4 orientation;

};

struct md5_mesh_t
{
	std::string shader;
	md5_vertex_t* vertexArray;
	md5_triangle_t* triangleArray;
	md5_weight_t* weightArray;
	int numverts;
	int numtris;
	int numweights;
};

class MD5Model
{
public:
	MD5Model();
	~MD5Model();

	bool Init(std::string, std::string);
	void Shutdown();
	void Update(float);
	void Render();
	glm::mat4* GetSkinnedMatrices();

private:
	bool LoadMesh(std::string);
	void ShutdownMesh();
	void BuildBindPose();
	void ComputeVertexPositionsFromWeights(md5_mesh_t*, ogl_mesh_t*);
	bool PrepareMeshes();

	bool LoadAnim(std::string);
	void ShutdownAnim();

private:
	bool InitBuffers();
	void ShutdownBuffers();
	bool InitTextures();
	void ShutdownTextures();

private:
	int m_iMD5Version;
	int m_iNumJoints;
	int m_iNumMeshes;

	md5_joint_t* m_pJointArray;
	md5_mesh_t* m_pMeshArray;
	ogl_mesh_t* m_pFinalMeshArray;

	glm::mat4* m_pInvBindPose;
	glm::mat4* m_pSkinnedMatrices;
	MD5Anim* m_pAnim;

	//graphics
	unsigned int* m_pVertexArrayIds;
	unsigned int* m_pVertexBufferIds;
	unsigned int* m_pIndexBufferIds;
	unsigned int* m_pNumIndices;

	Texture** m_ppTextures;
};


