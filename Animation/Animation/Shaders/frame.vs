#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 _color;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main()
{
	gl_Position = view * vec4(position, 1.0f);

	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y); //texCoord
	_color = color;
}