#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix; // proj * view

uniform mat4 model;

void main()
{
   gl_Position = camMatrix * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}