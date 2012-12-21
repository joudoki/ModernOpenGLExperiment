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

int main(int argc, char** argv) {
    printf("  GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    if (!acquireContext()) return EXIT_FAILURE;
    if (!acquireFunctions()) return EXIT_FAILURE;
    
    printf("OpenGL %s\n", glGetString(GL_VERSION));
    
    GLfloat vertCoords[] = {
         0.0f,  0.8f,
        -0.8f, -0.8f,
         0.8f, -0.8f
    };

    GLuint vboCoords;
    glGenBuffers(1, &vboCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertCoords), vertCoords, GL_STATIC_DRAW);

    const char* vertShader =
        "#version 120\n"
        "attribute vec2 coord2d;\n"
        "void main(void) {\n"
        "  gl_Position = vec4(coord2d, 0.0, 1.0);\n"
        "}\n";

    const char* fragShader =
        "#version 120\n"
        "void main(void) {\n"
        "  gl_FragColor[0] = gl_FragCoord.x/800.0;\n"
        "  gl_FragColor[1] = gl_FragCoord.y/600.0;\n"
        "  gl_FragColor[2] = 1.0;\n"
        "}\n";

    // mumble mumble vs2012 c++11 initialization lists mumble mumble
    std::vector<std::string> attribs;
    attribs.push_back("coord2d");

    GLProgram* program = GLProgram::Create(vertShader, fragShader, attribs);
    
    if (program == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    GLuint coord2d = (*program)["coord2d"];

    // Main render loop
    while(true) {
        // Rendering here
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the program and bind the attribute
        program->Activate();

        // Send the verts as an attribute to the program
        glBindBuffer(GL_ARRAY_BUFFER, vboCoords);

        glEnableVertexAttribArray(coord2d);
        glVertexAttribPointer(
            coord2d,    // Bind to coord2d
            2,          // 2 attributes per vert
            GL_FLOAT,   // Type of element
            GL_FALSE,   // Do not normalize
            0,          // Don't skip any elements
            NULL        // Pull data from current bound VBO
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);   // Send 3 vertices to the shader
        glDisableVertexAttribArray(coord2d);

        // Swap out buffer to render screen
        glfwSwapBuffers();
        
        // Check for stopping conditions
        if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
            break;
    }

    glDeleteBuffers(1, &vboCoords);
    
    glfwTerminate();
    return EXIT_SUCCESS;
}