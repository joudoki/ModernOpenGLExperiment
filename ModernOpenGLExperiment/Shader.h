#ifndef SHADER_H
#define SHADER_H

#include "Rendering.h"

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

    std::string GetErrorLog() const {
        std::string log;
        
        GLint logLength;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);

        log.resize(logLength);
        glGetShaderInfoLog(shaderHandle, logLength, NULL, &log[0]);

        return log;
    }
};

#endif