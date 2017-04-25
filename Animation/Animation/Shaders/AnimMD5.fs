#version 330

in vec2 texCoord;
out vec4 fragment;

uniform sampler2D sampler;

void main()
{
	fragment = texture(sampler, texCoord);	
}

