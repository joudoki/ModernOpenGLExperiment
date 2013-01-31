#ifndef PROGRAM_H
#define PROGRAM_H

#include "Rendering.h"
#include "Shader.h"

/**
 * Program - Contains linked shaders representing the programmable part of the pipeline.
 * We'll typically attach this to a Model for use in rendering a Mesh.
 */
class Program {
private:
    GLuint programHandle;
    GLint  linkResult;

    Uniform_t* uniforms;
    VertexAttribute_t* attributes;

    VertexArray* vertexArray;

    Program() : programHandle(glCreateProgram()), linkResult(GL_FALSE),
        vertexArray(NULL), uniforms(NULL), attributes(NULL) {}

    void Link();
    void AcquireUniforms();
    //void AcquireAttributes();

public:
    static Program* CreateFromShaders(Shader<VertexShader>* vertexShader, Shader<FragmentShader>* fragmentShader);
    //static Program* CreateFromShaders(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    //static Program* CreateFromShaders(const char* vertexShaderPath, const char* fragmentShaderPath);

    ~Program();

    std::string GetLinkLog() const;

    bool IsValid() const { return programHandle != 0 && linkResult == GL_TRUE; }

    void Bind() const { glUseProgram(programHandle); }

    //void setUniform(Uniform_t* uniform, void* value);

};

#endif