#version 330 core
in vec2 TexCoord;
in vec3 _color;

out vec4 color;

// Texture samplers
uniform sampler2D sampler;

void main()
{
	color = texture(sampler, TexCoord);
	//color = vec4(_color, 1);
}