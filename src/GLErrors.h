#pragma once

#include <glad/glad.h>

#include <iostream>

/*
* This file defines macros to flush out openGL errors, used for debugging
* GLCall() is used to run statements to determine error values which can be found are defined in "glad.c"
* in hexadecimal
*/

#define ASSERT(x) if (!(x)) __debugbreak()
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);