#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class CameraOGL
{
protected:
	glm::mat4 m_perspective;
	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
public:
		
	CameraOGL() {}
	CameraOGL(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar) 
	{
		m_perspective = glm::perspective(fov, aspect, zNear, zFar);
		m_position = pos;
		m_forward = glm::vec3(0, 0, -30);
		m_up = glm::vec3(0, 1, 1);
	}

	CameraOGL(const cv::Mat camera_matrix, const cv::Mat distortion_coefficients, cv::Size size, const glm::vec3& pos, float zNear, float zFar)
	{				
	
		m_perspective[0][0] = 2.0*camera_matrix.at<double>(0, 0) / size.width;
		m_perspective[0][1] = 0.0;
		m_perspective[0][2] = 0.0;
		m_perspective[0][3] = 0.0;

		m_perspective[1][0] = 0.0;
		m_perspective[1][1] = 2.0*camera_matrix.at<double>(1, 1) / size.height;
		m_perspective[1][2] = 0.0;
		m_perspective[1][3] = 0.0;

		m_perspective[2][0] = -2.0 * camera_matrix.at<double>(0, 2) / size.width + 1.0;
		m_perspective[2][1] =  2.0 * camera_matrix.at<double>(1, 2) / size.height - 1.0;
		m_perspective[2][2] = -(zFar + zNear) / (-zNear + zFar);
		m_perspective[2][3] = -1.0;

		m_perspective[3][0] = 0.0;
		m_perspective[3][1] = 0.0;
		m_perspective[3][2] = -2.0*zNear*zFar / (-zNear + zFar);
		m_perspective[3][3] = 0.0;
		
		m_position = pos;
		m_forward = glm::vec3(0, 0, -30);
		m_up = glm::vec3(0, 1, 1);
	}

	inline glm::mat4 GetPerspective() const
	{
		return m_perspective;
	}

	inline glm::mat4 GetProjection() const
	{
		return glm::lookAt(m_position, m_position + m_forward, m_up);

	}

	inline glm::mat4 GetViewProjection() const
	{
		return m_perspective *glm::lookAt(m_position, m_position + m_forward, m_up);
	}

	inline glm::mat4 GetViewProjectionCV() const
	{
		return m_perspective*glm::lookAt(m_position, m_position + m_forward, m_up);
	}
	
	~CameraOGL(){}
};

