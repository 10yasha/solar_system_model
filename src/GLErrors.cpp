#include "GLErrors.h"

void GLClearError()
{
    /*while (glGetError() != GL_NO_ERROR);*/
    while (glGetError());
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << std::hex << error << function << " " << file << ":" << std::dec << line << ")" << std::endl;
        return false;
    }
    return true;
}