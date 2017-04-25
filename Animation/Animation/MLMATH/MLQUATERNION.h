#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>

inline 
float mlDotVec4(glm::vec4 a, glm::vec4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline 
float mlLengthVec4(glm::vec4 in)
{
	return sqrtf(mlDotVec4(in, in));
}

inline 
glm::vec4 mlNormalizeVec4(glm::vec4 in)
{
	glm::vec4 finalq;
	float invLength = 1.0f / mlLengthVec4(in);
	finalq.x = in.x * invLength;
	finalq.y = in.y * invLength;
	finalq.z = in.z * invLength;
	finalq.w = in.w * invLength;
	return finalq;
}

inline
glm::vec4 mlMultiplyQuaternion_2(glm::vec4 a, glm::vec4 b)
{
	glm::vec4 q;
	
	glm::vec3 va = glm::vec3(a.x, a.y, a.z);
	glm::vec3 vb = glm::vec3(b.x, b.y, b.z);
	
	float wa = a.w; 
	float wb = b.w;  

	glm::vec3 va_X_vb = glm::cross(va, vb);
			
	q.x = va.x * wb  + vb.x * wa + va_X_vb.x;
	q.y = va.y * wb  + vb.y * wa + va_X_vb.y;
	q.z = va.z * wb  + vb.z * wa + va_X_vb.z;
	q.w = wa * wb - glm::dot(va, vb);
	return q;
}

inline
glm::vec3 mlRotateVec3ByQuaternion(glm::vec4 q, glm::vec3 pos)
{
	glm::vec3 finalpos;
	
	glm::vec4 pos_quat = glm::vec4(pos.x, pos.y, pos.z, 0.0f); 
	glm::vec4 q_star = glm::vec4(-q.x, -q.y, -q.z, q.w);

	glm::vec4 result, temp;

	temp = mlMultiplyQuaternion_2(pos_quat, q_star);
	result = mlMultiplyQuaternion_2(q, temp);

	finalpos.x = result.x;
	finalpos.y = result.y;
	finalpos.z = result.z;

	return finalpos;
}

inline
glm::vec4  mlAxisAgle(glm::vec3 n, float theta)
{
	glm::vec4 q;
	float cos_ht = cosf(theta * 0.5f);
	float sin_ht = sinf(theta * 0.5f);

	q.x = n.x * sin_ht;
	q.y = n.y * sin_ht;
	q.z = n.z * sin_ht;
	q.w = cos_ht;
	return q;
}

inline
glm::vec4 mlSlerp( glm::vec4 a, glm::vec4 b, float t)
{
	glm::vec4 q;
	float cosOmega = glm::dot(a, b); // a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	
	if(cosOmega < 0.0f)	{
		a = -a;
		cosOmega = -cosOmega;
	}
	
	float k0, k1;
	if(cosOmega > 0.9999f){
		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		float sinOmega = sqrtf(1.0f - cosOmega * cosOmega);
		float omega = atan2f(sinOmega, cosOmega);
		float oneOverSinOmega = 1.0f / sinOmega;

		k0 = sinf((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sinf(t * omega) * oneOverSinOmega;
	}

	q.x = a.x * k0 + b.x * k1;
	q.y = a.y * k0 + b.y * k1;
	q.z = a.z * k0 + b.z * k1;
	q.w = a.w * k0 + b.w * k1;

	return q;
} 

inline
glm::mat4 mlQuaternionToMat4(glm::vec4 Q)
{
	glm::mat4 M;

	M[0][0] = 1.0f - 2.0f * (Q.y * Q.y  + Q.z * Q.z);
	M[0][1] = 2.0f * (Q.x * Q.y + Q.w * Q.z);
	M[0][2] = 2.0f * (Q.x * Q.z - Q.w * Q.y);
	M[0][3] = 0.0f;

	M[1][0] = 2.0f * (Q.x * Q.y - Q.w * Q.z);
	M[1][1] = 1.0f - 2.0f * (Q.x * Q.x  + Q.z * Q.z);
	M[1][2] = 2.0f * (Q.y * Q.z + Q.w * Q.x);
	M[1][3] = 0.0f;

	M[2][0] = 2.0f * (Q.x * Q.z + Q.w * Q.y);
	M[2][1] = 2.0f * (Q.y * Q.z - Q.w * Q.x);
	M[2][2] = 1.0f - 2.0f * (Q.x * Q.x + Q.y * Q.y);
	M[2][3] = 0.0f;

	M[3][0] = 0.0f;
	M[3][1] = 0.0f;
	M[3][2] = 0.0f;
	M[3][3] = 1.0f;

	return M;
}


