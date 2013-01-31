#ifndef PROGRAM_H
#define PROGRAM_H

#include "Rendering.h"
#include "Shader.h"

/**
 * Program - Contains linked shaders representing the programmable part of the pipeline.
 * We'll typically attach this to a Model for use in rendering a Mesh.
 *
 * If this implementation looks familiar - it's modeled after Gameplay's Effect class, though
 * this class is meant to be a flattened version of the material hierarchy in that engine (to
 * keep things simple).
 */
class Program {
private:
    GLuint programHandle;
    GLint  linkResult;

    std::map<std::string, Uniform_t> uniforms;
    std::map<std::string, VertexAttribute_t> attributes;

    VertexArray* vertexArray;

    Program() : programHandle(glCreateProgram()), linkResult(GL_FALSE), vertexArray(NULL) {}

    void Link();
    void AcquireUniforms();
    void AcquireAttributes();

public:
    static Program* CreateFromShaders(Shader<VertexShader>* vertexShader, Shader<FragmentShader>* fragmentShader);
    //static Program* CreateFromShaders(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    //static Program* CreateFromShaders(const char* vertexShaderPath, const char* fragmentShaderPath);

    ~Program();

    std::string GetLinkLog() const;

    bool IsValid() const { return programHandle != 0 && linkResult == GL_TRUE; }

    void Bind() const { glUseProgram(programHandle); }

    //Uniform_t& GetUniform(const std::string& name) { return uniforms[name]; }
    //VertexAttribute_t& 

    //void setUniform(Uniform_t* uniform, void* value);

};

#endif