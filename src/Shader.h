#pragma once

#include <glad/glad.h>

#include <string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

// reads text file and converts to string
static std::string getFileContents(const char* filename);

class Shader
{
private:
	// checks for correct compilation
	void compileErrors(unsigned int shader, const char* type);

public:
	unsigned int m_ID;

	// constructor, build the shader from vertex + fragment shaders
	Shader(const char* vertShader, const char* fragShader);

	void bind();
	void unbind();
};