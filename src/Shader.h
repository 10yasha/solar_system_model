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
public:
	unsigned int m_ID;

	// Reads a text file and outputs a string with everything in the text file
	Shader(const char* vertShader, const char* fragShader);

	void bind();
	void unbind();

	// checks for correct compilation
	void compileErrors(unsigned int shader, const char* type);
};