#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


class Transform
{
protected:
	glm::vec3 m_pos;
	glm::vec3 m_rot;
	glm::vec3 m_scale;
	glm::mat4 m_transform;

public:
	Transform(const glm::vec3& pos = glm::vec3(), 
		      const glm::vec3& rot=glm::vec3(), 
		      const glm::vec3& scale = glm::vec3(1.0f,1.0f,1.0f)):
		      m_pos(pos), m_rot(rot),m_scale(scale){}

	inline glm::mat4 GetModel() const
	{
		return m_transform;
	}

	inline void CalculateModel()
	{
		glm::mat4 posMatrix = glm::translate(m_pos);
		
		glm::mat4 rotXMatrix = glm::rotate(m_rot.x, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 rotYMatrix = glm::rotate(m_rot.y, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 rotZMatrix = glm::rotate(m_rot.z, glm::vec3(0.0, 0.0, 1.0));
		
		glm::mat4 scaleMatrix = glm::scale(m_scale);

		glm::mat4 rotMatrix = rotZMatrix * rotYMatrix * rotXMatrix;		
		m_transform = posMatrix * rotMatrix * scaleMatrix;
	}

	inline glm::mat4 CalculateModelFromOpenCV(cv::Mat tvec, cv::Mat rvecRod, cv::Mat rvec)
	{		
		glm::mat4 rotMatrix;


		/*m_pos.x = tvec.at<double>(0) / tvec.at<double>(2);
		m_pos.y = tvec.at<double>(1) / tvec.at<double>(2);
		m_pos.z = tvec.at<double>(2) / tvec.at<double>(2);
		*/
				
		glm::mat4 posMatrix = glm::translate(m_pos);

		rotMatrix[0][0] = rvecRod.at<double>(0, 0);
		rotMatrix[0][1] = -rvecRod.at<double>(1, 0);
		rotMatrix[0][2] = -rvecRod.at<double>(2, 0);
		rotMatrix[0][3] = 0.0;

		rotMatrix[1][0] = rvecRod.at<double>(0, 1);
		rotMatrix[1][1] = -rvecRod.at<double>(1, 1);
		rotMatrix[1][2] = -rvecRod.at<double>(2, 1);
		rotMatrix[1][3] = 0.0;

		rotMatrix[2][0] = rvecRod.at<double>(0, 2);
		rotMatrix[2][1] = -rvecRod.at<double>(1, 2);
		rotMatrix[2][2] = -rvecRod.at<double>(2, 2);
		rotMatrix[2][3] = 0.0;

		rotMatrix[3][0] = tvec.at<double>(0);
		rotMatrix[3][1] = -tvec.at<double>(1);
		rotMatrix[3][2] = -tvec.at<double>(2);
		rotMatrix[3][3] = 1.0;
				
		glm::mat4 scaleMatrix = glm::scale(m_scale);
		
		m_transform = rotMatrix* posMatrix * scaleMatrix;
		return rotMatrix;
	}
	
	inline glm::vec3& GetPos() { return m_pos; }
	inline glm::vec3& GetRot() { return m_rot; }
	inline glm::vec3& GetScale() { return m_scale; }

	inline void SetPos(const glm::vec3& pos) { m_pos = pos; }
	inline void SetRot(const glm::vec3& rot) { m_rot = rot; }
	inline void SetScale(const glm::vec3& scale) { m_scale = scale; }

	~Transform() {}
};

