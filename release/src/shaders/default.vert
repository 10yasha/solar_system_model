#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoor;
layout (location = 2) in vec3 normal;

// outputs texture coordinates to fragment shader
out vec2 texCoord;

// input matrices needed for 3D viewing
uniform mat4 model;
uniform mat4 camMatrix; // proj * view

// need to pass to fragment shader
out vec3 Normal;
out vec3 FragPosition;

void main()
{
	vec4 tempPosition = model * vec4(position, 1.0f);

	// final vertix position
	gl_Position = camMatrix * tempPosition;

	FragPosition = vec3(tempPosition);

	texCoord = texCoor;

	Normal = mat3(transpose(inverse(model))) * normal;
}