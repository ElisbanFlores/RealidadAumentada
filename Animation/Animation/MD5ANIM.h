
#pragma once

#include "MLMATH\MLQUATERNION.h"
#include "HELPERS.h"
#include <fstream>
#include <string>
#include <iostream>


struct md5_joint_info_t
{
	std::string name;
	int parentId;
	int flags;
	int startIdx;
};

struct md5_bound_t
{
	glm::vec3 min;
	glm::vec3 max;
};

struct md5_base_frame_t
{
	glm::vec3 position;
	glm::vec4 orientation;
};

struct md5_frame_data_t
{
	int frameId;
	float* data;
};

struct md5_skeleton_joint_t
{
	int parentId;
	glm::vec3 position;
	glm::vec4 orientation;
};

struct md5_frame_skeleton_t
{
	md5_skeleton_joint_t* joints;
};

class MD5Anim
{
public:
	MD5Anim();
	~MD5Anim();

	bool Init(std::string);
	void Shutdown();
	void Update(float);
	glm::mat4* GetAnimatedMatrices();

private:
	bool LoadAnim(std::string);
	void ShutdownAnim();
	void BuildFrameSkeleton(md5_frame_skeleton_t*, md5_joint_info_t*, md5_base_frame_t*, md5_frame_data_t*);
	void InterpolateSkeletons(md5_frame_skeleton_t*, md5_frame_skeleton_t*, float);

private:
	int m_iMD5Version;
	int m_iNumFrames;
	int m_iNumJoints;
	int m_iFrameRate;
	int m_iNumAnimatedComponents;

	float m_fFrameDuration;
	float m_fAnimDuration;
	float m_fAnimTime;

	md5_joint_info_t* m_pJointInfoArray;
	md5_bound_t* m_pBoundArray;
	md5_base_frame_t* m_pBaseFrameArray;
	md5_frame_data_t* m_pFrameDataArray;

	md5_frame_skeleton_t* m_pFrameSkeletonArray;
	glm::mat4* m_pAnimatedMatrices;
};


