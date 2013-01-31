#ifndef RENDERING_H
#define RENDERING_H

// Use a custom-generated opengl header that only includes the core profile
// glLoadGen - https://bitbucket.org/alfonse/glloadgen
#include "gl_core_3_3.h"

#define GLFW_NO_GLU
#include <GL/glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>

enum ShaderType {
    VertexShader = GL_VERTEX_SHADER,
    FragmentShader = GL_FRAGMENT_SHADER
};

// Uniform for a vertex/fragment shader
typedef struct {
    std::string name;
    GLuint location;
    GLenum type;
    GLint size;
} Uniform_t;

// Attribute mapping for vertex shader
typedef struct {
    std::string name;
    GLuint id;
    GLenum type;
} VertexAttribute_t;

// Format of single vertex attribute in VBO
typedef struct {
    GLuint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const GLvoid* offset;
} VertexFormat_t;

class VertexArray {
private:
    GLuint vaoHandle;

public:

};

#endif