#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>

#include "Shader.h"
#include "Camera.h"

class Skybox
{
private:
	std::string m_cubemapPaths[6];

	// vertices for the skybox
	const float m_vertices[3*8] =
	{
		// coordinates
		-1.0f, -1.0f,  1.0f,// 0      7--------6
		 1.0f, -1.0f,  1.0f,// 1     /|       /|
		 1.0f, -1.0f, -1.0f,// 2    4--------5 |
		-1.0f, -1.0f, -1.0f,// 3    | |      | |
		-1.0f,  1.0f,  1.0f,// 4    | 3------|-2
		 1.0f,  1.0f,  1.0f,// 5    |/       |/
		 1.0f,  1.0f, -1.0f,// 6    0--------1
		-1.0f,  1.0f, -1.0f // 7
	};

	// indices for the skybox
	const unsigned int m_indices[3*12] =
	{
		// right
		1, 2, 6,
		6, 5, 1,
		// left
		0, 4, 7,
		7, 3, 0,
		// top
		4, 5, 6,
		6, 7, 4,
		// bottom
		0, 3, 2,
		2, 1, 0,
		// back
		0, 1, 5,
		5, 4, 0,
		// front
		3, 7, 6,
		6, 2, 3
	};

	// arrays and buffers
	unsigned int m_vao, m_vbo, m_ebo;

	// cubemap texture ID
	unsigned int m_cubemapTexID;

	// initializes and send vertices and indices data to buffers
	void initSkybox();

	// fills in array of filepaths
	void getTexturePaths(std::string directory);

	// uses skybox textures to create cubemap
	void createCubeMap();

public:
	Skybox(std::string directory);

	void draw(Shader& shader, const Camera& camera);
};