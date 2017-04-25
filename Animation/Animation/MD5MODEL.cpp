#include "MD5MODEL.h"

MD5Model::MD5Model()
{
	m_pJointArray = NULL;
	m_pMeshArray = NULL;
	m_pFinalMeshArray = NULL;

	m_pInvBindPose = NULL;
	m_pSkinnedMatrices = NULL;
	m_pAnim = NULL;

	m_pVertexArrayIds = NULL;
	m_pVertexBufferIds = NULL;
	m_pIndexBufferIds = NULL;
	m_pNumIndices = NULL;
	m_ppTextures = NULL;
}

MD5Model::~MD5Model()
{

}

bool MD5Model::Init(std::string filename1, std::string filename2)
{
	if(!LoadMesh(filename1))return false;
	if(!PrepareMeshes())return false;
	if(!LoadAnim(filename2))return false;
	
	//opengl
	if(!InitBuffers())	return false;
	if(!InitTextures())	return false;
	
	ShutdownMesh();
	return true;
}

void MD5Model::Shutdown()
{
	ShutdownAnim();
	ShutdownBuffers();
	ShutdownTextures();
}

void MD5Model::Update(float dt)
{
	m_pAnim->Update(dt);

	glm::mat4* pAnimatedMatrices = m_pAnim->GetAnimatedMatrices();

	for (int i = 0; i < m_iNumJoints; i++) {
		m_pSkinnedMatrices[i] = pAnimatedMatrices[i] * m_pInvBindPose[i];
	}
	
}

void MD5Model::Render()
{
	for(int i = 0; i < m_iNumMeshes; i++)
	{
		glBindVertexArray(m_pVertexArrayIds[i]);
		m_ppTextures[i]->Bind(0);
		glDrawElements(GL_TRIANGLES, m_pNumIndices[i], GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

glm::mat4* MD5Model::GetSkinnedMatrices(){ return m_pSkinnedMatrices; }

bool MD5Model::LoadMesh(std::string filename)
{
	std::ifstream file(filename);
	if(!file.is_open())
		return false;

	int initpos = file.tellg();
	file.seekg(0, std::ios::end);
	int filelength = file.tellg();
	file.seekg(initpos);

	int imesh = 0;
	std::string param, junk;
	file >> param;

	while (!file.eof())
	{
		if(param == "MD5Version")
		{
			file >> m_iMD5Version;
			if(!m_iMD5Version)
				return false;

			printf("MD5Version: %i\n", m_iMD5Version);
		}
		else if(param == "numJoints")
		{
			file >> m_iNumJoints;
			m_pJointArray = new md5_joint_t[m_iNumJoints];
			if(!m_pJointArray)
				return false;

			m_pInvBindPose = new glm::mat4[m_iNumJoints];
			if(!m_pInvBindPose)
				return false;

			m_pSkinnedMatrices = new glm::mat4[m_iNumJoints];
			if(!m_pSkinnedMatrices)
				return false;

			printf("numJoints: %i\n", m_iNumJoints);
		}
		else if(param == "numMeshes")
		{
			file >> m_iNumMeshes;
			m_pMeshArray = new md5_mesh_t[m_iNumMeshes];
			if(!m_pMeshArray)
				return false;

			m_pFinalMeshArray = new ogl_mesh_t[m_iNumMeshes];
			if(!m_pFinalMeshArray)
				return false;

			printf("numMeshes: %i\n", m_iNumMeshes);
		}
		else if(param == "joints")
		{
			file >> junk;
			for(int i = 0; i < m_iNumJoints; i++)
			{
				md5_joint_t joint;
				file >> joint.name >> joint.parentId >> junk;
				file >> joint.position.x >> joint.position.y >> joint.position.z;
				file >> junk >> junk;
				file >> joint.orientation.x >> joint.orientation.y >> joint.orientation.z;
				file >> junk;
				file.ignore(filelength, '\n');

				RemoveQuotes(joint.name);

				ComputeQuaternionW(joint.orientation);

				m_pJointArray[i] = joint;
			}
			file >> junk;

			BuildBindPose();
		}
		else if(param == "mesh")
		{
			md5_mesh_t mesh;
			file >> junk;
			file >> param;

			while (param != "}")
			{
				if(param == "shader")
				{
					file >> mesh.shader;
					RemoveQuotes(mesh.shader);

					std::string texture = mesh.shader + '\n';
					printf(texture.c_str());
				}
				else if(param == "numverts")
				{
					file >> mesh.numverts;
					mesh.vertexArray = new md5_vertex_t[mesh.numverts];
					if(!mesh.vertexArray)
						return false;

					for(int i = 0; i < mesh.numverts; i++)
					{
						md5_vertex_t vert;
						file >> junk >> junk >> junk; //vert vertIdx (
						file >> vert.texCoord.x >> vert.texCoord.y >> junk; // )
						file >> vert.startWeight >> vert.numWeights;

						mesh.vertexArray[i] = vert; 
					}
				}
				else if(param == "numtris")
				{
					file >> mesh.numtris;
					mesh.triangleArray = new md5_triangle_t[mesh.numtris];
					if(!mesh.triangleArray)
						return false;

					for(int i = 0; i < mesh.numtris; i++)
					{
						md5_triangle_t tri;
						file >> junk >> junk; //tri triIdx
						file >> tri.indices[0] >> tri.indices[1] >> tri.indices[2];

						mesh.triangleArray[i] = tri;
					}
				}
				else if(param == "numweights")
				{
					file >> mesh.numweights;
					mesh.weightArray = new md5_weight_t[mesh.numweights];
					if(!mesh.weightArray)
						return false;

					for(int i = 0; i < mesh.numweights; i++)
					{
						md5_weight_t weight;
						file >> junk >> junk; //weight weightIdx
						file >> weight.jointId >> weight.bias >> junk; // (
						file >> weight.position.x >> weight.position.y >> weight.position.z;
						file >> junk; // )

						mesh.weightArray[i] = weight;
					}
				}

				file >> param;
			}

			m_pMeshArray[imesh] = mesh;
			imesh++;
		}
		file >> param;
	}
	return true;
}

void MD5Model::ShutdownMesh()
{
	if(m_pJointArray)
	{
		delete[] m_pJointArray;
		m_pJointArray = NULL;
	}
	if(m_pMeshArray)
	{
		for(int i = 0; i < m_iNumMeshes; i++)
		{
			delete[] m_pMeshArray[i].vertexArray;
			delete[] m_pMeshArray[i].triangleArray;
			delete[] m_pMeshArray[i].weightArray;
			m_pMeshArray[i].vertexArray = NULL;
			m_pMeshArray[i].triangleArray = NULL;
			m_pMeshArray[i].weightArray = NULL;
		}
		delete[] m_pMeshArray;
		m_pMeshArray = NULL;
	}
	if(m_pFinalMeshArray)
	{
		for(int i = 0; i < m_iNumMeshes; i++)
		{
///			delete[] m_pFinalMeshArray[i].vertexArray;
//			delete[] m_pFinalMeshArray[i].indexArray;
	//		m_pFinalMeshArray[i].vertexArray = NULL;
	//		m_pFinalMeshArray[i].indexArray = NULL;
		}
		delete[] m_pFinalMeshArray;
		m_pFinalMeshArray = NULL;
	}
}

void MD5Model::BuildBindPose()
{
	glm::mat4 rotation, rotationInv, translationInv, finalmatrix;

	for(int i = 0; i < m_iNumJoints; i++)
	{
		md5_joint_t& joint = m_pJointArray[i];		
		rotation = mlQuaternionToMat4(joint.orientation);		
		rotationInv = glm::transpose(rotation);
		translationInv = glm::translate(glm::mat4(), -joint.position);
		finalmatrix = rotationInv * translationInv;		
		m_pInvBindPose[i] = finalmatrix;
	}
}

void MD5Model::ComputeVertexPositionsFromWeights(md5_mesh_t* mesh, ogl_mesh_t* finalmesh)
{
	for(int i = 0; i < mesh->numverts; i++)
	{
		md5_vertex_t& vert = mesh->vertexArray[i];
		ogl_vertex_t& finalvert = finalmesh->vertexArray[i];
		
		finalvert.position = glm::vec3(0.0f);
		finalvert.texCoord = vert.texCoord;
		finalvert.indices = glm::vec4(0.0f);
		finalvert.weights = glm::vec4(0.0f);

		for(int j = 0; j < vert.numWeights; j++)
		{
			md5_weight_t& weight = mesh->weightArray[vert.startWeight + j];
			md5_joint_t& joint = m_pJointArray[weight.jointId];
			glm::vec3 rotpos;
			rotpos = mlRotateVec3ByQuaternion(joint.orientation, weight.position);

			finalvert.position.x += (joint.position.x + rotpos.x) * weight.bias;
			finalvert.position.y += (joint.position.y + rotpos.y) * weight.bias;
			finalvert.position.z += (joint.position.z + rotpos.z) * weight.bias;
			finalvert.indices[j] = weight.jointId;
			finalvert.weights[j] = weight.bias;
		}
	}
}

bool MD5Model::PrepareMeshes()
{
	for(int i = 0; i < m_iNumMeshes; i++)
	{
		md5_mesh_t& mesh = m_pMeshArray[i];
		ogl_mesh_t& finalmesh = m_pFinalMeshArray[i];

		finalmesh.texture = mesh.shader;
		finalmesh.numVertices = mesh.numverts;
		finalmesh.numIndices = mesh.numtris * 3;
		
		finalmesh.vertexArray = new ogl_vertex_t[finalmesh.numVertices];
		if(!finalmesh.vertexArray)
			return false;

		finalmesh.indexArray = new unsigned int[finalmesh.numIndices];
		if(!finalmesh.indexArray)
			return false;

		ComputeVertexPositionsFromWeights(&mesh, &finalmesh);

		for(int j = 0; j < mesh.numtris; j++)
		{
			finalmesh.indexArray[j * 3 + 0] = mesh.triangleArray[j].indices[0];
			finalmesh.indexArray[j * 3 + 1] = mesh.triangleArray[j].indices[1];
			finalmesh.indexArray[j * 3 + 2] = mesh.triangleArray[j].indices[2];
		}
	}

	return true;
}

bool MD5Model::LoadAnim(std::string filename)
{
	m_pAnim = new MD5Anim();
	if(!m_pAnim)
		return false;

	if(!m_pAnim->Init(filename))
		return false;

	return true;
}

void MD5Model::ShutdownAnim()
{
	if(m_pAnim)
	{
		m_pAnim->Shutdown();
		delete m_pAnim;
		m_pAnim = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
bool MD5Model::InitBuffers()
{
	m_pVertexArrayIds = new unsigned int[m_iNumMeshes];
	if(!m_pVertexArrayIds)
		return false;

	m_pVertexBufferIds = new unsigned int[m_iNumMeshes];
	if(!m_pVertexBufferIds)
		return false;

	m_pIndexBufferIds = new unsigned int[m_iNumMeshes];
	if(!m_pIndexBufferIds)
		return false;

	m_pNumIndices = new unsigned int[m_iNumMeshes];
	if(!m_pNumIndices)
		return false;

	glGenVertexArrays(m_iNumMeshes, m_pVertexArrayIds);
	glGenBuffers(m_iNumMeshes, m_pVertexBufferIds);
	glGenBuffers(m_iNumMeshes, m_pIndexBufferIds);

	for(int i = 0; i < m_iNumMeshes; i++)
	{
		m_pNumIndices[i] = m_pFinalMeshArray[i].numIndices;
		glBindVertexArray(m_pVertexArrayIds[i]);
	
		glBindBuffer(GL_ARRAY_BUFFER, m_pVertexBufferIds[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ogl_vertex_t) * m_pFinalMeshArray[i].numVertices, 
			                  m_pFinalMeshArray[i].vertexArray, GL_STATIC_DRAW);

		size_t textureOffset = sizeof(glm::vec3);
		size_t indicesOffset = sizeof(glm::vec2) + textureOffset;
		size_t weightsOffset = sizeof(glm::vec4) + indicesOffset;


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(ogl_vertex_t), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(ogl_vertex_t), (void*)textureOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(ogl_vertex_t), (void*)indicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(ogl_vertex_t), (void*)weightsOffset);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pIndexBufferIds[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_pFinalMeshArray[i].numIndices, 
			                  m_pFinalMeshArray[i].indexArray, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	return true;
}

void MD5Model::ShutdownBuffers()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(m_iNumMeshes, m_pIndexBufferIds);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(m_iNumMeshes, m_pVertexBufferIds);

	glBindVertexArray(0);
	glDeleteVertexArrays(m_iNumMeshes, m_pVertexArrayIds);

	if(m_pVertexArrayIds)
	{
		delete[] m_pVertexArrayIds;
		m_pVertexArrayIds = NULL;
	}
	if(m_pIndexBufferIds)
	{
		delete[] m_pIndexBufferIds;
		m_pIndexBufferIds = NULL;
	}
	if(m_pVertexBufferIds)
	{
		delete[] m_pVertexBufferIds;
		m_pVertexBufferIds = NULL;
	}
	if(m_pNumIndices)
	{
		delete[] m_pNumIndices;
		m_pNumIndices = NULL;
	}
}

bool MD5Model::InitTextures()
{
	m_ppTextures = new Texture*[m_iNumMeshes];
	if(!m_ppTextures)
		return false;

	for(int i = 0; i < m_iNumMeshes; i++)
	{
		m_ppTextures[i] = new Texture();
		if(!m_ppTextures[i])
			return false;
		// Inicializar textura...
		m_ppTextures[i]->InitTexture(m_pFinalMeshArray[i].texture);
	}
	return true;
}

void MD5Model::ShutdownTextures()
{
	if(m_ppTextures)
	{
		for(int i = 0; i < m_iNumMeshes; i++)
		{
			m_ppTextures[i]->~Texture();
			delete m_ppTextures[i];
			m_ppTextures[i] = NULL;
		}
		delete[] m_ppTextures;
		m_ppTextures = NULL;
	}
}
