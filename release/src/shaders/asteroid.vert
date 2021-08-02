#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoor;
layout (location = 2) in vec3 normal;

// for instancing transformations
layout (location = 3) in mat4 instanceMatrix;

// outputs texture coordinates to fragment shader
out vec2 texCoord;

// input matrices needed for 3D viewing
uniform mat4 camMatrix; // proj * view

// pass to fragmant shader
out vec3 Normal;
out vec3 FragPosition;

void main()
{
	vec4 tempPosition = instanceMatrix * vec4(position, 1.0f);

	// final vertex position
	gl_Position = camMatrix * tempPosition;

	FragPosition = vec3(tempPosition);

	texCoord = texCoor;

	Normal = mat3(transpose(inverse(instanceMatrix))) * normal;
}