#include <cstdio>
#include <cstdlib>

// Use a custom-generated opengl header that only includes the core profile
// glLoadGen - https://bitbucket.org/alfonse/glloadgen
#include "gl_core_3_3.h"

#define GLFW_NO_GLU
#include <GL/glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "GLProgram.h"

#define BUFFER_OFFSET(i) ((char*) NULL + (i))

int acquireContext(int width, int height) {
    // Context Creation
    if (!glfwInit()) {
        fprintf(stderr, "Unable to initialize GLFW!\n");
        return 0;
    }

    // Request OpenGL v3.3 context
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);

    if (!glfwOpenWindow(
        width, height,
        8, 8, 8, 8, // RGBA all each 8 bits for 32-bit coloring
        8, 0, // 8 bit depth, 0 bit stencil
        GLFW_WINDOW
    )) {
        printf("Unable to open glfw window!\n");
        
        glfwTerminate();
        return 0;
    }

    return 1;
}

int acquireFunctions() {
    // Function loading
    int oglLoadResult = ogl_LoadFunctions();
    if (oglLoadResult != ogl_LOAD_SUCCEEDED) {
        fprintf(stderr, "Unable to load OpenGL functions!\n");

        glfwTerminate();
        return 0;
    }

    return 1;
}

void setupOpenGL() {
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // Enable 3D ops
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Setup windowing transform
    glViewport(0, 0, 800, 600);
    //glDepthRange(1.0f, 128.0f);
    
    // For flat shading, take the value of the first vertex in the primitive
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
}

std::string readFile(const char* fileName) {
    std::ifstream in(fileName);
    if (!in) throw(errno);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

GLuint createVBO(void* data, GLuint size, GLenum type, GLenum hint) {
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(type, vbo);
    glBufferData(type, size, data, hint);

    return vbo;
}

typedef struct {
    GLint attr;
    GLuint vbo;

    // Vertex Attribute Array Params
    GLuint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const GLvoid* offset;
} GLAttribute_t;

GLuint createVAO(GLAttribute_t* attrs, GLsizei attrCount) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    for (GLuint i=0; i<attrCount; ++i) {
        GLAttribute_t* attr = attrs + i;

        glEnableVertexAttribArray(attr->attr);
        glBindBuffer(GL_ARRAY_BUFFER, attr->vbo);
        glVertexAttribPointer(attr->attr, attr->size, attr->type, attr->normalized, attr->stride, attr->offset);
    }

    return vao;
}

typedef struct {
    GLfloat coord[3];
    GLfloat color[3];
} Vertex_t;

class Cube {
private:
    GLuint vao;
    GLuint vbo;
    GLuint ibo;

public:
    Cube() : vao(0), vbo(0), ibo(0) { }
    ~Cube() { }

    void Upload(GLuint attrCoord, GLuint attrColor) {
        Vertex_t data[] = {
            {{-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 0.5f}},
            {{-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}},
            {{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f, 1.0f}},
            {{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
            {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 1.0f}},
            {{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
            {{ 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}}
        };

        GLubyte indices[] = {
            2,3,1, 0,2,1,
            7,5,3, 5,1,3,
            6,4,7, 4,5,7,
            2,0,6, 0,4,6,
            6,3,2, 3,6,7,
            4,0,1, 1,5,4
        };

        vbo = createVBO(data,       sizeof(data),       GL_ARRAY_BUFFER,         GL_STATIC_DRAW);
        ibo = createVBO(indices,    sizeof(indices),    GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
        
        GLAttribute_t vaoAttrs[] = {
            {attrCoord, vbo, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0},
            {attrColor, vbo, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat))}
        };

        vao = createVAO(vaoAttrs, 2);
    }

    void Cleanup() {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);

        vbo = ibo = vao = 0;
    }

    void Render() {
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
    }
};

int main(int argc, char** argv) {
    printf("  GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    
    int width = 800, height = 600;
    if (!acquireContext(width, height)) return EXIT_FAILURE;
    if (!acquireFunctions()) return EXIT_FAILURE;
    
    printf("OpenGL %s\n", glGetString(GL_VERSION));
    setupOpenGL();

    /*
        Setup the program & shaders
    */
    GLProgram* program = GLProgram::Create(
        readFile("glsl/one.v.glsl").c_str(), 
        readFile("glsl/one.f.glsl").c_str()
    );

    if (program == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    program->Activate();

    GLint attrCoord = program->GetAttribute("vCoord");
    GLint attrColor = program->GetAttribute("vColor");
    
    /*
        Setup objects
    */
    Cube* cube = new Cube();
    cube->Upload(attrCoord, attrColor);

    do {
        float time = (float) glfwGetTime();

        float x = 1.5f * glm::cos(time), z = 1.5f * glm::sin(time);

        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.125f));
        glm::mat4 view = glm::lookAt(glm::vec3(x, 1.5f, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 project = glm::perspectiveFov(70.0f, (float) width, (float) height, 1.0f, 16.0f);

        glm::mat4 matrix = project * view * model;

        glUniformMatrix4fv(
            program->GetUniform("transform"),
            1,          // Count
            GL_FALSE,   // Do not transpose
            glm::value_ptr(matrix)
        );

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube->Render();

        glfwSwapBuffers();
    } while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

    /*
        Cleanup
    */
    cube->Cleanup();
    delete cube;
    delete program;
    
    glfwTerminate();
    return EXIT_SUCCESS;
}