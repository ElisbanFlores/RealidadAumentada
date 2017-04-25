#include "MD5ANIM.h"

MD5Anim::MD5Anim()
{
	m_pJointInfoArray = NULL;
	m_pBoundArray = NULL;
	m_pBaseFrameArray = NULL;
	m_pFrameDataArray = NULL;

	m_pFrameSkeletonArray = NULL;
	m_pAnimatedMatrices = NULL;
}

MD5Anim::~MD5Anim()
{

}

bool MD5Anim::Init(std::string filename)
{
	if(!LoadAnim(filename))
		return false;

	ShutdownAnim();

	return true;
}

void MD5Anim::Shutdown()
{
	if(m_pFrameSkeletonArray)
	{
		for(int i = 0; i < m_iNumFrames; i++)
		{
			delete[] m_pFrameSkeletonArray[i].joints;
			m_pFrameSkeletonArray[i].joints = NULL;
		}
		delete[] m_pFrameSkeletonArray;
		m_pFrameSkeletonArray = NULL;
	}
	if(m_pAnimatedMatrices)
	{
		delete[] m_pAnimatedMatrices;
		m_pAnimatedMatrices = NULL;
	}
}

void MD5Anim::Update(float dt)
{
	m_fAnimTime += dt;
	if(m_fAnimTime > m_fAnimDuration)
		m_fAnimTime = 0.0f;

	float curframe = m_fAnimTime * float(m_iFrameRate);
	int frame0 = (int)floorf(curframe);
	int frame1 = frame0 + 1;

	if(frame0 == m_iNumFrames - 1)
		frame1 = 0;

	float alpha = curframe - float(frame0);
	InterpolateSkeletons(&m_pFrameSkeletonArray[frame0], 
		                 &m_pFrameSkeletonArray[frame1], 
						 alpha);
}

glm::mat4* MD5Anim::GetAnimatedMatrices(){ return m_pAnimatedMatrices; }

bool MD5Anim::LoadAnim(std::string filename)
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
			if(m_iMD5Version != 10)
				return false;

			printf("MD5Version: %i\n", m_iMD5Version);
		}
		else if(param == "numFrames")
		{
			file >> m_iNumFrames;
			m_pBoundArray = new md5_bound_t[m_iNumFrames];
			if(!m_pBoundArray)
				return false;

			m_pFrameDataArray = new md5_frame_data_t[m_iNumFrames];
			if(!m_pFrameDataArray)
				return false;

			m_pFrameSkeletonArray = new md5_frame_skeleton_t[m_iNumFrames];
			if(!m_pFrameSkeletonArray)
				return false;

			printf("numFrame: %i\n", m_iNumFrames);
		}
		else if(param == "numJoints")
		{
			file >> m_iNumJoints;
			m_pJointInfoArray = new md5_joint_info_t[m_iNumJoints];
			if(!m_pJointInfoArray)
				return false;

			m_pBaseFrameArray = new md5_base_frame_t[m_iNumJoints];
			if(!m_pBaseFrameArray)
				return false;
			
			m_pAnimatedMatrices = new glm::mat4[m_iNumJoints];
			if(!m_pAnimatedMatrices)
				return false;

			printf("numJoints: %i\n", m_iNumJoints);
		}
		else if(param == "frameRate")
		{
			file >> m_iFrameRate;
			printf("frameRate: %i\n", m_iFrameRate);
		}
		else if(param == "numAnimatedComponents")
		{
			file >> m_iNumAnimatedComponents;
			printf("numAnimatedComponents: %i\n", m_iNumAnimatedComponents);
		}
		else if(param == "hierarchy")
		{
			file >> junk;
			for(int i = 0; i < m_iNumJoints; i++)
			{
				md5_joint_info_t joint;
				file >> joint.name >> joint.parentId >> joint.flags >> joint.startIdx;
				file.ignore(filelength, '\n');	

				m_pJointInfoArray[i] = joint;
			}
			file >> junk;
		}
		else if(param == "bounds")
		{
			file >> junk;
			for(int i = 0; i < m_iNumFrames; i++)
			{
				md5_bound_t bound;
				file >> junk;
				file >> bound.min[0] >> bound.min[1] >> bound.min[2];
				file >> junk >> junk;
				file >> bound.max[0] >> bound.max[1] >> bound.max[2];
				file >> junk;

				m_pBoundArray[i] = bound;
			}
			file >> junk;
		}
		else if(param == "baseframe")
		{
			file >> junk;
			for(int i = 0; i < m_iNumJoints; i++)
			{
				md5_base_frame_t joint;
				file >> junk;
				file >> joint.position[0] >> joint.position[1] >> joint.position[2];
				file >> junk >> junk;
				file >> joint.orientation[0] >> joint.orientation[1] >> joint.orientation[2];
				file >> junk;

				m_pBaseFrameArray[i] = joint;
			}
			file >> junk;
		}
		else if(param == "frame")
		{
			md5_frame_data_t frame;
			file >> frame.frameId >> junk;

			frame.data = new float[m_iNumAnimatedComponents];
			if(!frame.data)
				return false;

			m_pFrameSkeletonArray[frame.frameId].joints = new md5_skeleton_joint_t[m_iNumJoints];
			if(!m_pFrameSkeletonArray[frame.frameId].joints)
				return false;

			for(int i = 0; i < m_iNumAnimatedComponents; i++)
				file >> frame.data[i];

			m_pFrameDataArray[frame.frameId] = frame;
			BuildFrameSkeleton(&m_pFrameSkeletonArray[frame.frameId],
							   m_pJointInfoArray, 
							   m_pBaseFrameArray, 
							   &frame);

			file >> junk;
		}

		file >> param;
	}

	m_fFrameDuration = 1.0f / float(m_iFrameRate);
	m_fAnimDuration = m_fFrameDuration * float(m_iNumFrames);
	m_fAnimTime = 0.0f;

	printf("finished reading md5 anim\n");
	return true;
}

void MD5Anim::ShutdownAnim()
{
	if(m_pJointInfoArray)
	{
		delete[] m_pJointInfoArray;
		m_pJointInfoArray = NULL;
	}
	if(m_pBoundArray)
	{
		delete[] m_pBoundArray;
		m_pBoundArray = NULL;
	}
	if(m_pBaseFrameArray)
	{
		delete[] m_pBaseFrameArray;
		m_pBaseFrameArray = NULL;
	}
	if(m_pFrameDataArray)
	{
		for(int i = 0; i < m_iNumFrames; i++)
		{
			delete[] m_pFrameDataArray[i].data;
			m_pFrameDataArray[i].data = NULL;
		}
		delete[] m_pFrameDataArray;
		m_pFrameDataArray = NULL;
	}
}

void MD5Anim::BuildFrameSkeleton(md5_frame_skeleton_t* skeleton, 
								 md5_joint_info_t* jointInfoArray, 
								 md5_base_frame_t* baseFrameArray, 
								 md5_frame_data_t* frameDataArray)
{
	md5_skeleton_joint_t animjoint;
	for(int i = 0; i < m_iNumJoints; i++)
	{
		unsigned int j = 0;
		md5_joint_info_t jointinfo = jointInfoArray[i];
		md5_base_frame_t baseframe = baseFrameArray[i];			
		
		animjoint.parentId = jointinfo.parentId;
		animjoint.position = baseframe.position;
		animjoint.orientation = baseframe.orientation;
		
		if(jointinfo.flags & 1)
		{
			animjoint.position[0] = frameDataArray->data[jointinfo.startIdx + j++];
		}
		if(jointinfo.flags & 2)
		{
			animjoint.position[1] = frameDataArray->data[jointinfo.startIdx + j++];
		}
		if(jointinfo.flags & 4)
		{
			animjoint.position[2] = frameDataArray->data[jointinfo.startIdx + j++];
		}
		if(jointinfo.flags & 8) //qx
		{
			animjoint.orientation[0] = frameDataArray->data[jointinfo.startIdx + j++];
		}
		if(jointinfo.flags & 16)
		{
			animjoint.orientation[1] = frameDataArray->data[jointinfo.startIdx + j++];
		}
		if(jointinfo.flags & 32)
		{
			animjoint.orientation[2] = frameDataArray->data[jointinfo.startIdx + j++];
		}
				
		ComputeQuaternionW(animjoint.orientation);
		
		if(animjoint.parentId >= 0)
		{
			md5_skeleton_joint_t parentjoint = skeleton->joints[animjoint.parentId];
			glm::vec3 rotpos;
			rotpos = mlRotateVec3ByQuaternion(parentjoint.orientation, animjoint.position);

			animjoint.position[0] = parentjoint.position[0] + rotpos[0];
			animjoint.position[1] = parentjoint.position[1] + rotpos[1];
			animjoint.position[2] = parentjoint.position[2] + rotpos[2];
			
			animjoint.orientation = mlMultiplyQuaternion_2(parentjoint.orientation,animjoint.orientation);
			animjoint.orientation = mlNormalizeVec4( animjoint.orientation);

			skeleton->joints[i] = animjoint;
			//memcpy(&skeleton->joints[i], &animjoint, sizeof(md5_skeleton_joint_t));
		}
		else
		{
			skeleton->joints[i] = animjoint;
			//memcpy(&skeleton->joints[i], &animjoint, sizeof(md5_skeleton_joint_t));
		}
	}
}

void MD5Anim::InterpolateSkeletons(md5_frame_skeleton_t* skeleton0, md5_frame_skeleton_t* skeleton1, float alpha)
{
	glm::mat4 finalmatrix, rotation, translation;
	md5_skeleton_joint_t finaljoint;
	for(int i = 0; i < m_iNumJoints; i++)
	{
		md5_skeleton_joint_t& joint0 = skeleton0->joints[i];
		md5_skeleton_joint_t& joint1 = skeleton1->joints[i];

		finaljoint.position[0] = (1.0f - alpha) * joint0.position[0] + alpha * joint1.position[0];
		finaljoint.position[1] = (1.0f - alpha) * joint0.position[1] + alpha * joint1.position[1];
		finaljoint.position[2] = (1.0f - alpha) * joint0.position[2] + alpha * joint1.position[2];
		finaljoint.orientation = mlSlerp(joint0.orientation, joint1.orientation, alpha);

		rotation = mlQuaternionToMat4(finaljoint.orientation);
		translation = glm::translate(glm::mat4(), finaljoint.position);
		
		//mlTranslationMat4(translation, finaljoint.position);
		finalmatrix = translation * rotation;
		//mlMultiplyMat4_2(finalmatrix, translation, rotation);
		m_pAnimatedMatrices[i] = finalmatrix;
		//memcpy(m_pAnimatedMatrices[i], finalmatrix, sizeof(mat4));
	}
}
