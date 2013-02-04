#ifndef MESH_H
#define MESH_H

#include <vector>

#include "Rendering.h"

// Mesh: Contains the actual vertex data for a model
// also managed the lifetime of the attached vertex buffer object
// and the liftime of the attached index buffer object
class Mesh {
private:
    static const GLuint VBO_VERTICES = 0;
    static const GLuint VBO_INDICES = 1;

    GLuint vaoHandle;
    GLuint vboHandles[2];
    
    GLuint indexCount;
    GLuint vertexCount;
    GLuint vertexSize;
    
    IndexType indexFormat;
    std::vector<VertexAttributeBinding_t> vertexFormat;
    
    PrimitiveType primitiveType;

    // OpenGL performance hint
    GLenum isDynamic;

public:
    Mesh(VertexAttributeBinding_t* attribFormats, GLuint attribCount);
    ~Mesh();

    void SetVertexData(GLuint count, GLuint size, void* data);
    void SetIndexData(IndexType indexFormat, GLuint count, GLuint size, void* data);

    GLenum Render() const;
};

#endif