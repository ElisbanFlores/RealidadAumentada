#version 400

// Interpolated values from the vertex shaders
in vec3 Color;
// Ouput data
out vec4 FragColor;

void main() {	

	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	FragColor = vec4(Color, 1.0);
}