#include "OrbitalEllipse.h"

OrbitalEllipse::OrbitalEllipse(float a, float b) {
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

void OrbitalEllipse::initVertices(float a, float b, unsigned int numVertices)
{
    double increment = PI * 2 / float(numVertices);
    double curAngle = 0.f;

    // generate vertices along the entire arc of the ellipse
    for (unsigned int i = 0; i < numVertices; i++)
    {
        m_vertices[i] = glm::vec3(a * sin(curAngle), 0.0f, b * cos(curAngle));
        curAngle += increment;
    }
}

void OrbitalEllipse::initIndices(unsigned int numVertices)
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

void OrbitalEllipse::draw(Shader& shader, glm::mat4 model) {
    shader.bind();
    GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.m_ID, "model"), 1, GL_FALSE, &model[0][0]));
    GLCall(glUniform3fv(glGetUniformLocation(shader.m_ID, "color"), 1, &m_lineColor[0]));

    GLCall(glBindVertexArray(m_VAO));
    //glDrawArrays(GL_LINES, 0, 2);
    GLCall(glDrawElements(GL_LINES, m_numIndices, GL_UNSIGNED_INT, 0));
}

OrbitalEllipse::~OrbitalEllipse() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}