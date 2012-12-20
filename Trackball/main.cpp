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

void printLog(GLuint object) {
    char* log;
    GLint logLength = 0;

    if (glIsShader(object)) {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
    } else if (glIsProgram(object)) {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logLength);
    } else {
        fprintf(stderr, "printLog: Not a shader or a program\n");
    }

    log = new char[logLength];

    if (glIsShader(object)) {
        glGetShaderInfoLog(object, logLength, NULL, log);
    } else  if (glIsProgram(object)) {
        glGetProgramInfoLog(object, logLength, NULL, log);
    }

    fprintf(stderr, "%s", log);

    delete[] log;
}

typedef struct {
    GLuint programID;
    GLuint vertShaderID;
    GLuint fragShaderID;

    GLuint* attribs;
    GLuint attribCount;
} program_t;

GLint compileProgram(program_t& program, const char* vertShader, const char* fragShader, size_t argc, const char** argv) {
    GLint compileResult = GL_FALSE, linkResult = GL_FALSE;

    // Create and compile the vertex shader
    program.vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(program.vertShaderID, 1, &vertShader, NULL);
    glCompileShader(program.vertShaderID);

    // Check the compilation status of the vertex shader
    glGetShaderiv(program.vertShaderID, GL_COMPILE_STATUS, &compileResult);

    if (!compileResult) {
        fprintf(stderr, "Error in vertex shader\n");
        printLog(program.vertShaderID);
        return 0;
    }

    // Create and compile the fragment shader
    program.fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(program.fragShaderID, 1, &fragShader, NULL);
    glCompileShader(program.fragShaderID);

    // Check compilation of fragment shader
    glGetShaderiv(program.fragShaderID, GL_COMPILE_STATUS, &compileResult);

    if (!compileResult) {
        fprintf(stderr, "Error in fragment shader\n");
        printLog(program.fragShaderID);
        return 0;
    }

    // Link the two into a program
    program.programID = glCreateProgram();
    glAttachShader(program.programID, program.vertShaderID);
    glAttachShader(program.programID, program.fragShaderID);
    glLinkProgram(program.programID);

    // Check link status
    glGetProgramiv(program.programID, GL_LINK_STATUS, &linkResult);

    if (!linkResult) {
        fprintf(stderr, "Error during linking\n");
        printLog(program.programID);
        return 0;
    }

    // Attach attributes
    program.attribCount = argc;
    program.attribs = new GLuint[argc];
    for (size_t i=0; i<argc; ++i) {
        program.attribs[i] = glGetAttribLocation(program.programID, argv[i]);

        if (program.attribs[i] == -1) {
            fprintf(stderr, "Could not bind attribute %s\n", argv[i]);
            delete[] program.attribs;
            return 0;
        }
    }

    return 1;
}

int main(int argc, char** argv) {
    printf("  GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    if (!acquireContext()) return EXIT_FAILURE;
    if (!acquireFunctions()) return EXIT_FAILURE;
    
    printf("OpenGL %s\n", glGetString(GL_VERSION));  
    
    GLfloat verts[] = {
         0.0f,  0.8f,
        -0.8f, -0.8f,
         0.8f, -0.8f
    };

    const char* vertShader =
        "#version 120\n"
        "attribute vec2 coord2d;\n"
        "void main(void) {\n"
        "  gl_Position = vec4(coord2d, 0.0, 1.0);\n"
        "}\n";

    const char* fragShader =
        "#version 120\n"
        "void main(void) {\n"
        "  gl_FragColor[0] = 0.0;\n"
        "  gl_FragColor[1] = 0.0;\n"
        "  gl_FragColor[2] = 1.0;\n"
        "}\n";

    const char* attribs = "coord2d";

    program_t program;
    if (!compileProgram(program, vertShader, fragShader, 1, &attribs)) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Main render loop
    while(true) {
        // Rendering here
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the program and bind the attribute
        glUseProgram(program.programID);

        // Send the verts as an attribute to the program
        glEnableVertexAttribArray(program.attribs[0]);
        glVertexAttribPointer(
            program.attribs[0], // Bind to coord2d
            2,                  // 2 attributes per vert
            GL_FLOAT,           // Type of element
            GL_FALSE,           // Do not normalize
            0,                  // Don't skip any elements
            verts
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);   // Send 3 vertices to the shader
        glDisableVertexAttribArray(program.attribs[0]);

        // Swap out buffer to render screen
        glfwSwapBuffers();
        
        // Check for stopping conditions
        if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
            break;
    }
    
    glfwTerminate();
    return EXIT_SUCCESS;
}