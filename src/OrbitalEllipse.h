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

public:
    OrbitalEllipse(float a, float b) {
        initVertices(a, b, m_numVertices);
        initIndices(m_numVertices);

        // generate objects
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        glGenBuffers(1, &m_instanceVBO);

        glBindVertexArray(m_VAO);

        // vertex buffer
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), &m_vertices[0], GL_STATIC_DRAW));

        // layout
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
        GLCall(glEnableVertexAttribArray(0));

        // index buffer
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), &m_indices[0], GL_STATIC_DRAW));
    }

    void initVertices(float a, float b, unsigned int numVertices)
    {
        double increment = PI * 2 / float(numVertices);
        double curAngle = 0.f;
        for (unsigned int i = 0; i < numVertices; i++)
        {
            m_vertices[i] = glm::vec3(a * sin(curAngle), 0.0f, b * cos(curAngle));
            curAngle += increment;
        }
    }

    void initIndices(unsigned int numVertices)
    {
        // fill in ellipse, vertex 0 connects to vertex 1, vertex 1 connects to vertex 2, etc. 
        for (unsigned int i = 0; i < numVertices - 1; i++)
        {
            m_indices[i * 2] = i;
            m_indices[i * 2 + 1] = i + 1;
        }

        // fill in last indices, last vertex connects to first vertex
        m_indices[(numVertices - 1) * 2] = numVertices - 1;
        m_indices[(numVertices - 1) * 2 + 1] = 0;
    }

    void draw(Shader& shader, glm::mat4 model) {
        shader.bind();
        GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, "model"), 1, GL_FALSE, &model[0][0]));
        GLCall(glUniform3fv(glGetUniformLocation(shader.m_ID, "color"), 1, &m_lineColor[0]));

        GLCall(glBindVertexArray(m_VAO));
        //glDrawArrays(GL_LINES, 0, 2);
        GLCall(glDrawElements(GL_LINES, m_numIndices, GL_UNSIGNED_INT, 0));
    }

    ~OrbitalEllipse() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }
};