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

int acquireContext() {
    int width = 800, height = 600;

    // Context Creation
    if (!glfwInit()) {
        fprintf(stderr, "Unable to initialize GLFW!\n");
        return 0;
    }

    // Request OpenGL v3.3 context
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
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

int main(int argc, char** argv) {
    printf("  GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    if (!acquireContext()) return EXIT_FAILURE;
    if (!acquireFunctions()) return EXIT_FAILURE;
    
    printf("OpenGL %s\n", glGetString(GL_VERSION));
    setupOpenGL();

    printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);

    /*
        Setup the program & shaders
    */
    const char* vertShader =
        "#version 120\n"
        "uniform mat4 transform;\n"
        "attribute vec3 coord;\n"
        "attribute vec3 color;\n"
        "void main(void) {\n"
        "  gl_Position = transform * vec4(coord, 1.0);\n"
        "  gl_FrontColor = vec4(color, 1.0);\n"
        "}\n";

    const char* fragShader =
        "#version 120\n"
        "void main(void) {\n"
        //"  gl_FragColor[2] = gl_FragCoord.x/800.0;\n"
        //"  gl_FragColor[1] = gl_FragCoord.y/600.0;\n"
        //"  gl_FragColor[0] = 1.0;\n"
        "  gl_FragColor = gl_Color;\n"
        "}\n";

    GLProgram* program = GLProgram::Create(vertShader, fragShader);

    if (program == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    program->Activate();

    printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);

    glm::mat4 matrix = glm::mat4(1.0f);
        
    glUniformMatrix4fv(
        program->GetUniform("transform"),
        1,          // Count
        GL_FALSE,   // Do not transpose
        glm::value_ptr(matrix)
    );

    GLint attrCoord = program->GetAttribute("coord");
    GLint attrColor = program->GetAttribute("color");
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);

    /*
        Upload vertex data into VBOs
    */

    GLfloat vertAttributeData[] = {
        0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,
        
        0.0f, 0.0f, 0.5f,    0.0f, 0.0f, 1.0f,
        0.8f, 1.0f, 0.5f,    0.0f, 0.0f, 1.0f,
        1.0f, 0.8f, 0.5f,    0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.2f,    0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.2f,    0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.2f,    0.0f, 1.0f, 0.0f
    };

    GLuint vboVertAttributeData;
    glGenBuffers(1, &vboVertAttributeData);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertAttributeData);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertAttributeData), vertAttributeData, GL_STATIC_DRAW);

    printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);

    GLubyte vertIndices[] = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8
    };

    GLuint iboVertIndices;
    glGenBuffers(1, &iboVertIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboVertIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertIndices), vertIndices, GL_STATIC_DRAW);

    printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);

    /*
        Setup the VAO
    */

    // Set state of VAO
    glEnableVertexAttribArray(attrCoord);
    glVertexAttribPointer(
        attrCoord,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        0
    );
    
    printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);
    
    glEnableVertexAttribArray(attrColor);
    glVertexAttribPointer(
        attrColor,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    
    printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);

    while(true) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Still in effect from above
        //glBindVertexArray(vao);
        //glBindBuffer(GL_ARRAY_BUFFER, vboVertAttributeData);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboVertIndices);
        //glDrawArrays(GL_TRIANGLES, 0, 9);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, 0);
        printf("%d - %s:%d\n", glGetError(), __FILE__, __LINE__);

        glfwSwapBuffers();
        
        // Check for stopping conditions
        if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
            break;
    }

    // Cleanup
    glDeleteBuffers(1, &vboVertAttributeData);
    glDeleteBuffers(1, &iboVertIndices);
    glDeleteVertexArrays(1, &vao);
    delete program;
    
    glfwTerminate();
    return EXIT_SUCCESS;
}