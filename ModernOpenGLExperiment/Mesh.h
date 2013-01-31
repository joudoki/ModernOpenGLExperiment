#ifndef MESH_H
#define MESH_H

#include "Rendering.h"

// Mesh: Contains the actual vertex data for a model
// also managed the lifetime of the attached vertex buffer object
// and the liftime of the attached index buffer object
class Mesh {
private:
    GLuint vboHandle;
    GLint vertexCount;
    VertexFormat_t* vertexFormat; // Specify the layout of attributes in VBO

    GLuint iboHandle;
    GLuint indexCount;
    void* indexFormat;

    GLenum primitiveType;

    // OpenGL performance hint
    GLenum isDynamic;

    Mesh() : vboHandle(0), vertexCount(0), vertexFormat(NULL),
        iboHandle(0), indexCount(0), indexFormat(NULL),
        primitiveType(GL_TRIANGLES), isDynamic(GL_STATIC_DRAW) {

    }

    ~Mesh() {}

public:
    void SetVertexData(void* data, GLint vertexCount);
    
    void GetMesh();
    void GetProgram();
};

#endif