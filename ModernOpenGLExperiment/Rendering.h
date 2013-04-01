#ifndef RENDERING_H
#define RENDERING_H

// Use a custom-generated opengl header that only includes the core profile
// glLoadGen - https://bitbucket.org/alfonse/glloadgen
#include "gl_core_3_3.h"

#define GLFW_NO_GLU
#include <GL/glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>

/**
 * Vertex_t
 * A semi-standard format for mesh vertex data (for now)
 */
typedef struct {
    glm::vec3 coord;
    glm::vec3 normal;
    glm::vec2 texCoord;
} MeshVertex_t;

/**
 * ShaderType - Specifies the type of shader that a Shader object holds.
 */
namespace ShaderType {
    enum ShaderType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER
    };
}

/**
 * IndexType - Specifies the size of the index in an index buffer
 */
namespace IndexType {
    enum IndexType {
        UnsignedByteIndex = GL_UNSIGNED_BYTE,
        UnsignedShortIndex = GL_UNSIGNED_SHORT,
        UnsignedIntIndex = GL_UNSIGNED_INT
    };
}

/**
 * PrimitiveType - Specifies the kind of primitives that a Mesh holds
 */
namespace PrimitiveType {
    enum PrimitiveType {
        PointPrimitive = GL_POINTS,
        LineStripPrimitive = GL_LINE_STRIP,
        LineLoopPrimitive = GL_LINE_LOOP,
        LinesPrimitive = GL_LINES,
        TriangleStripPrimitive = GL_TRIANGLE_STRIP,
        TriangleFanPrimitive = GL_TRIANGLE_FAN,
        TrianglesPrimitive = GL_TRIANGLES
    };
}

namespace TextureType {
    enum TextureType {
        Texture2D = GL_TEXTURE_2D
    };
}

/**
 * Uniform_t - Simple data structure holding information about a Uniform in a Shader/Program.
 */
typedef struct {
    std::string name;
    GLuint location;
    GLenum type;
    GLint size;
} Uniform_t;

/**
 * VertexAttribute_t - Simple data structure that records how to reference an attribute
 * in a Shader/Program, what kind of data it expects, etc.
 */
typedef struct {
    std::string name;
    GLuint location;
    GLenum type;
    GLint size;
} VertexAttribute_t;

#define BUFFER_OFFSET(i) ((char*) NULL + (i))

/**
 * VertexAttributeBinding_t - Data structure that mirrors a call to glVertexAttribPointer
 * We could use this in client-side VAO emulation (at some point, if determined necessary)
 * but for now this is merely used at Mesh initialization
 */
typedef struct {
    /**
     * The attribute that the data is bound to
     */
    GLuint attrib;

    /**
     * The number of components per generic vertex attribute - Should be 1, 2, 3, or 4
     */
    GLuint size;

    /**
     * The domain of the attribute (like GL_FLOAT, GL_BYTE, etc)
     */
    GLenum type;

    GLboolean normalized;

    /**
     * How many bytes to move by to get to the (n+1)th value from the nth value
     */
    GLsizei stride;

    /**
     * Offset (in bytes) from the start of the buffer to get to the 0th value
     */
    const GLvoid* offset;
} VertexAttributeBinding_t;

#endif