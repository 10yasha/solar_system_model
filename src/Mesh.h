#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "Shader.h"
#include "GLErrors.h"

// vertex
struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoor;
	glm::vec3 Normal;
};

// texture 
struct Texture
{
	unsigned int ID;
	std::string path;
};

// mesh contains combination of vertices/indices/texture to be rendered together
class Mesh
{
public:
	// the mesh data
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	Texture m_texture;

	// openGL IDs
	unsigned int m_VAO, m_VBO, m_EBO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture texture);
	~Mesh();

	// sets up the mesh
	void initMesh();

	// draw call based on shader
	void draw(Shader& shader);
};