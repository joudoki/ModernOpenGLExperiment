#ifndef SHADER_H
#define SHADER_H

#include "Rendering.h"

/**
 * Shader - Represents a single OpenGL shader of a specific type. The template on the
 * class ensures compile-time checking of making sure that the right types of shaders
 * get assigned in the right places.
 *
 * Typically, two of these shaders will be accumulated into a Program.
 */
template <ShaderType type>
class Shader
{
private:
    GLuint shaderHandle;
    GLint  compileResult;

    Shader() : shaderHandle(glCreateShader(type)), compileResult(GL_FALSE) { }

    void SetSource(const char* source) {
        glShaderSource(shaderHandle, 1, &source, NULL);
    }

    void Compile() {
        glCompileShader(shaderHandle);
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileResult);
    }

public:
    ~Shader() {  glDeleteShader(shaderHandle); }

    static Shader* CompileFromSource(const std::string& source) {
        Shader* shader = new Shader();
        shader->SetSource(source.c_str());
        shader->Compile();

        return shader;
    }

    bool IsValid() const {
        return shaderHandle > 0 && compileResult == GL_TRUE;
    }

    std::string GetCompileLog() const {
        std::string log;
        
        GLint logLength;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);

        log.resize(logLength);
        glGetShaderInfoLog(shaderHandle, logLength, NULL, &log[0]);

        return log;
    }
};

#endif