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
    //glDepthRange(0.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
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
    const char* vertShader =
        "#version 330 core\n"
        "uniform mat4 transform;\n"
        "in vec3 vCoord;\n"
        "in vec3 vColor;\n"
        "flat out vec3 fColor;\n"
        "void main(void) {\n"
        "  gl_Position = transform * vec4(vCoord, 1.0);\n"
        "  fColor = vColor;\n"
        "}\n";

    const char* fragShader =
        "#version 330 core\n"
        "flat in vec3 fColor;\n"
        "out vec3 color;\n"
        "void main(void) {\n"
        "  color = fColor;"
        //"  gl_FragColor[2] = gl_FragCoord.x/800.0;\n"
        //"  gl_FragColor[1] = gl_FragCoord.y/600.0;\n"
        //"  gl_FragColor[0] = 1.0;\n"
        //"  gl_FragColor = gl_Color;\n"
        "}\n";

    GLProgram* program = GLProgram::Create(vertShader, fragShader);

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
        -0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f,  0.25f,
        -0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,
         0.25f, -0.25f, -0.25f,
         0.25f, -0.25f,  0.25f,
         0.25f,  0.25f, -0.25f,
         0.25f,  0.25f,  0.25f
    };

    GLuint vboVertCoords;
    glGenBuffers(1, &vboVertCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertCoords), vertCoords, GL_STATIC_DRAW);

    GLfloat vertColors[8*3] = {};
    for (size_t i=0; i<8; ++i) {
        float t = ((5*i) % 16) / 16.0f;
        vertColors[3*i] = t;
        vertColors[3*i+1] = 1-t;
    }

    GLuint vboVertColors;
    glGenBuffers(1, &vboVertColors);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertColors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertColors), vertColors, GL_STATIC_DRAW);

    GLubyte vertIndices[] = {
        1, 3, 7,    1, 7, 5,
        7, 4, 5,    7, 6, 4,
        3, 2, 7,    2, 6, 7,
        2, 3, 1,    2, 1, 0,
        6, 2, 0,    6, 0, 4,
        0, 1, 5,    0, 5, 4
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
        //glm::mat4 proj = glm::perspectiveFov(glm::radians(70.0f), (float) width, (float) height, 1.0f, 128.0f);
        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        float time = glfwGetTime();
        float pitch = -15.0f, yaw = 25.0f * time;
        glm::mat4 matrix = glm::rotate(glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1.0f, 0.0f, 0.0f)), yaw, glm::vec3(0.0f, 1.0f, 0.0f));

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