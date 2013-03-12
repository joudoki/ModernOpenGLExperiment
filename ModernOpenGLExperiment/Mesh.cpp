#include "Mesh.h"

Mesh::Mesh(PrimitiveType::PrimitiveType primitiveType, VertexAttributeBinding_t* attribFormats, GLuint attribCount) : 
    indexCount(0), vertexCount(0), vertexSize(0), indexFormat(IndexType::UnsignedShortIndex),
    primitiveType(primitiveType), isDynamic(GL_STATIC_DRAW) {

    glGenBuffers(2, vboHandles);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBO_VERTICES]);

    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    vertexFormat.resize(attribCount);
    for (GLuint i=0; i<attribCount; ++i) {
        VertexAttributeBinding_t* attr = attribFormats + i;
        vertexSize += attr->size;

        // Copy into internal vertex format record
        vertexFormat[i] = *attr;

        glEnableVertexAttribArray(attr->attrib);
        glVertexAttribPointer(attr->attrib, attr->size, attr->type, attr->normalized, attr->stride, attr->offset);
    }
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vaoHandle);
    glDeleteBuffers(2, vboHandles);
}

void Mesh::SetVertexData(GLuint count, GLuint size, void* data) {
    vertexCount = count;

    glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBO_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, size, data, isDynamic);
}

void Mesh::SetIndexData(IndexType::IndexType format, GLuint count, GLuint size, void* data) {
    indexCount = count;
    indexFormat = format;

    glBindBuffer(GL_ARRAY_BUFFER, vboHandles[VBO_INDICES]);
    glBufferData(GL_ARRAY_BUFFER, size, data, isDynamic);
}

GLenum Mesh::Render() const {
    glBindVertexArray(vaoHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[VBO_INDICES]);
    glDrawElements(primitiveType, indexCount, indexFormat, 0);

    return 0;
}