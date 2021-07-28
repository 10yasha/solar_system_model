#include "Mesh.h"

// constructor
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture texture)
	: m_vertices(vertices), m_indices(indices), m_texture(texture), m_instancing(1)
{
	initMesh();
}

Mesh::Mesh(std::unique_ptr<Mesh>&& other, const float number, std::vector<glm::mat4> instanceMatrix)
	: m_vertices(other->m_vertices), m_indices(other->m_indices), m_texture(other->m_texture), m_instancing(number)
{
	initMesh(number, instanceMatrix);
}

// clean up vertex array and buffers
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

// sets up the mesh
// instead of creating separate classes for VAO, VBO, EBO, just initialize them together inside this mesh
void Mesh::initMesh(const float number, std::vector<glm::mat4> instanceMatrix)
{
	// generate objects
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glGenBuffers(1, &m_instanceVBO);

	// vertex array object must be bound first
	glBindVertexArray(m_VAO);

	// vertex buffer object
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW));

	// element/index buffer object
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));

	// setup layout for shaders
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoor)));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)));
	GLCall(glEnableVertexAttribArray(2));

	if (number != 1)
	{
		// instanced VBO
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, instanceMatrix.size() * sizeof(glm::mat4), instanceMatrix.data(), GL_STATIC_DRAW));

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		
		// Makes it so the transform is only switched when drawing the next instance
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
	}

	// unbind to be safe
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// draw call based on shader
void Mesh::draw(Shader& shader)
{
	// Bind shader to be able to access uniforms
	
	GLCall(shader.bind());
	GLCall(glBindVertexArray(m_VAO));

	// Gets the location of the uniform
	GLCall(int texUni = glGetUniformLocation(shader.m_ID, "tex0"));
	// Sets the value of the uniform
	GLCall(glUniform1i(texUni, 0));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture.ID));

	// Draw the actual mesh
	
	// Check if instance drawing should be performed
	if (m_instancing == 1)
	{
		GLCall(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0));
	}
	else
	{
		GLCall(glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, m_instancing));
	}

	// unbind texture and VAO
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glBindVertexArray(0));
	
}