#pragma once

#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


inline 
void RemoveQuotes(std::string& input)
{
	size_t pos;
	while ( (pos = input.find('\"')) != std::string::npos )
		input.erase(pos, 1);
}

inline 
void ComputeQuaternionW(glm::vec4 &q)
{
	float w2 = 1.0f - q.x * q.x - q.y * q.y - q.z * q.z;
	if(w2 < 0.0f)
		q.w = 0.0f;
	else
		q.w = -sqrtf(w2);
}
