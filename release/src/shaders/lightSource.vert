#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoor;
layout (location = 2) in vec3 normal;

// outputs texture coordinates to fragment shader
out vec2 texCoord;

// input matrices needed for 3D viewing
uniform mat4 model;
uniform mat4 camMatrix; // proj * view


void main()
{
	// final vertex position
	gl_Position = camMatrix * model * vec4(position, 1.0);

	// pass uvs to fragment shader
	texCoord = texCoor;
}