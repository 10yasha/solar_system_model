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

out vec3 Normal;
out vec3 FragPosition;

void main()
{
	// determines final position of vertices
	gl_Position = camMatrix * instanceMatrix * vec4(position, 1.0);

	// assigns texture coordinates to pass to the fragment shader
	texCoord = texCoor;

	FragPosition = vec3(instanceMatrix * vec4(position, 1.0f));

	Normal = mat3(transpose(inverse(instanceMatrix))) * normal;
}