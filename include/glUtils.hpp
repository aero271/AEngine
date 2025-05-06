
#include <raylib.h>
#include "glad.h"
#include <vector>
#include "definitions.hpp"

class GLssbo 
{
public:
    GLuint ssbo;
    GLuint bindingPoint;

    GLssbo() {}

    template<typename T>
    GLssbo(std::vector<T>& data, GLuint bindingPoint) : bindingPoint(bindingPoint)
    {
        glGenBuffers(1, &ssbo);
        print("%1% buffer inited\n", ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) print("ERROR: SSBO write error: %1%\n", error);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        GLint bufSize = 0;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &bufSize);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        print("Buffer size: %1%\n", bufSize);

    }

    template<typename T>
    void updateBuffer(std::vector<T> data, int offset, int size)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size * sizeof(T), data.data() + offset * sizeof(T));
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }


    template<typename T>
    void getBufferData(std::vector<T>& data, int size)
    {
        while (glGetError() != GL_NO_ERROR) {}
        data.resize(size);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size * sizeof(T), data.data());

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) print("ERROR: SSBO read error: %1%\n", error);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void bind(GLuint shader)
    {
        glUseProgram(shader);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo);
    }

    ~GLssbo()
    {
        glDeleteBuffers(1, &ssbo);
    }
};