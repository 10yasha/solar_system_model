#include "Mesh.h"

// constructor
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture texture)
	: m_vertices(vertices), m_indices(indices), m_texture(texture)
{
	initMesh();
}

// clean up vertex array and buffers
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &this->m_VAO);
	glDeleteBuffers(1, &this->m_VBO);
	glDeleteBuffers(1, &this->m_EBO);
}

// sets up the mesh
// instead of creating separate classes for VAO, VBO, EBO, just initialize them together inside this mesh
void Mesh::initMesh()
{
	// generate objects
	glGenVertexArrays(1, &this->m_VAO);
	glGenBuffers(1, &this->m_VBO);
	glGenBuffers(1, &this->m_EBO);

	// vertex array object must be bound first
	glBindVertexArray(this->m_VAO);

	// vertex buffer object
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(Vertex), &this->m_vertices[0], GL_STATIC_DRAW));

	// element/index buffer object
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_indices.size() * sizeof(unsigned int), &this->m_indices[0], GL_STATIC_DRAW));

	// setup layout for shaders
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoor)));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)));

	// unbind to be safe
	glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// draw call based on shader
void Mesh::draw(Shader& shader)
{
	// Bind shader to be able to access uniforms
	
	GLCall(shader.bind());
	GLCall(glBindVertexArray(this->m_VAO));

	// Gets the location of the uniform
	GLCall(int texUni = glGetUniformLocation(shader.m_ID, "tex0"));
	// Sets the value of the uniform
	GLCall(glUniform1i(texUni, 0));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, this->m_texture.ID));

	// Draw the actual mesh
	GLCall(glDrawElements(GL_TRIANGLES, this->m_indices.size(), GL_UNSIGNED_INT, 0));

	// unbind texture and VAO
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glBindVertexArray(0));
}