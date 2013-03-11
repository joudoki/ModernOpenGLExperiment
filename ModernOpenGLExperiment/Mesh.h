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
    Mesh(PrimitiveType primitiveType, VertexAttributeBinding_t* attribFormats, GLuint attribCount);
    ~Mesh();

    /**
     * SetVertexData
     * Uploads the given vertex data to the vertex buffer associated with this mesh
     *
     * count - The number of vertices
     * size  - The actual size of the data to upload (in bytes)
     * data  - Pointer to the data in memory
     */
    void SetVertexData(GLuint count, GLuint size, void* data);

    /**
     * SetIndexData
     * Uploads the given index data to the index buffer associated with this mesh
     *
     * count - The number of indices
     * size  - The actual size of the data to upload (in bytes)
     * data  - Pointer to the data in memory
     */
    void SetIndexData(IndexType indexFormat, GLuint count, GLuint size, void* data);

    GLenum Render() const;
};

#endif