#ifndef PROGRAM_H
#define PROGRAM_H

#include "Rendering.h"

class Program {
private:
    GLuint programHandle;

    std::map<std::string, Uniform_t> uniforms;
    std::map<std::string, VertexAttribute_t> vertexAttributes;

    VertexArray* vertexArray;

    Program() : programHandle(0), vertexArray(NULL) {}
    ~Program() {}

public:
    void setUniform(Uniform_t* uniform, void* value);
};

#endif