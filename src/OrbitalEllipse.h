#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Shader.h"
#include "GLErrors.h"

#define PI 3.1415926
#define NUM_VERTICES 1000

class OrbitalEllipse
{
private:
    // openGL IDs
    unsigned int m_VAO, m_VBO, m_EBO, m_instanceVBO;

    const unsigned int m_numVertices = NUM_VERTICES;
    const unsigned int m_numIndices = 2 * NUM_VERTICES;

    glm::vec3 m_vertices[NUM_VERTICES];
    unsigned int m_indices[2 * NUM_VERTICES];

    glm::vec3 m_lineColor = glm::vec3(1.f, 1.f, 1.f);

    // initialize vertices data
    void initVertices(float a, float b, unsigned int numVertices);

    // initialize indices data
    void initIndices(unsigned int numVertices);

public:
    OrbitalEllipse(float a, float b);
    ~OrbitalEllipse();

    void draw(Shader& shader, glm::mat4 model);
};