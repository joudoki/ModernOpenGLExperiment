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
    glEnable(GL_DEPTH_TEST);
    //glDepthRange(1.0, 128.0);
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glViewport(0, 0, 800, 600);
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
}

std::string readFile(const char* fileName) {
    std::ifstream in(fileName);
    if (!in) throw(errno);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

int main(int argc, char** argv) {
    printf("  GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    
    int width = 800, height = 600;
    if (!acquireContext(width, height)) return EXIT_FAILURE;
    if (!acquireFunctions()) return EXIT_FAILURE;
    
    printf("OpenGL %s\n", glGetString(GL_VERSION));
    setupOpenGL();

    printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);

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
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /*
        Upload vertex data into VBOs
    */
    GLfloat vertCoords[] = {
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f
    };

    GLuint vboVertCoords;
    glGenBuffers(1, &vboVertCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertCoords), vertCoords, GL_STATIC_DRAW);

    GLfloat vertColors[] = {
        0.5f, 0.5f, 0.5f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f
    };

    /*
    for (size_t i=0; i<8; ++i) {
        float t = ((3*i) % 16) / 16.0f;
        vertColors[3*i] = t;
        vertColors[3*i+1] = 1-t;
    }
    */

    GLuint vboVertColors;
    glGenBuffers(1, &vboVertColors);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertColors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertColors), vertColors, GL_STATIC_DRAW);

    GLubyte vertIndices[] = {
        // X
        7, 4, 5,
        6, 4, 7,
        2, 1, 0,
        3, 1, 2,

        // Y
        2, 6, 3,
        3, 6, 7,
        4, 0, 1,
        5, 4, 1,

        // Z
        1, 3, 5,
        5, 3, 7,
        4, 6, 0,
        0, 6, 2
    };

    GLuint iboVertIndices;
    glGenBuffers(1, &iboVertIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboVertIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertIndices), vertIndices, GL_STATIC_DRAW);

    /*
        Setup the VAO
    */
    glEnableVertexAttribArray(attrCoord);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertCoords);
    glVertexAttribPointer(attrCoord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(attrColor);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertColors);
    glVertexAttribPointer(attrColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    do {
        float time = (float) glfwGetTime();

        float x = 1.5f * glm::cos(time), z = 1.5f * glm::sin(time);

        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.125f));
        glm::mat4 view = glm::lookAt(glm::vec3(x, 1.5f, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 project = glm::perspectiveFov(45.0f, (float) width, (float) height, 1.0f, 16.0f);

        glm::mat4 matrix = project * view * model;

        glUniformMatrix4fv(
            program->GetUniform("transform"),
            1,          // Count
            GL_FALSE,   // Do not transpose
            glm::value_ptr(matrix)
        );

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Still in effect from above
        //glBindVertexArray(vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboVertIndices);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

        glfwSwapBuffers();
    } while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

    // Cleanup
    glDeleteBuffers(1, &vboVertCoords);
    glDeleteBuffers(1, &vboVertColors);
    glDeleteBuffers(1, &iboVertIndices);
    glDeleteVertexArrays(1, &vao);
    delete program;
    
    glfwTerminate();
    return EXIT_SUCCESS;
}