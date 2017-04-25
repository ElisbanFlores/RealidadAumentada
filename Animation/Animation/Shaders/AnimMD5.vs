#version 330
#define MAX_BONES 33

uniform mat4 mSkinned[MAX_BONES];

layout(location = 0)  in vec3 in_position;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec4 in_indices;
layout(location = 3) in vec4 in_weights;
out vec2 texCoord;

uniform mat4 mWorld;
uniform mat4 mView;
uniform mat4 mProj;

void main()
{
	vec4 position = vec4(in_position, 1.0f);
	vec4 result = mSkinned[int(in_indices.x)] * position * in_weights.x;
	result = mSkinned[int(in_indices.y)] * position * in_weights.y + result;
	result = mSkinned[int(in_indices.z)] * position * in_weights.z + result;
	result = mSkinned[int(in_indices.w)] * position * in_weights.w + result;

	gl_Position = mWorld * result;
	gl_Position = mView * gl_Position;
	gl_Position = mProj * gl_Position;

	texCoord = in_texCoord;	
}