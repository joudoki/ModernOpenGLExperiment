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

int main(int argc, char** argv) {
    printf("  GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    if (!acquireContext()) return EXIT_FAILURE;
    if (!acquireFunctions()) return EXIT_FAILURE;
    
    printf("OpenGL %s\n", glGetString(GL_VERSION));

    // Main render loop
    while(true) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering here

        glfwSwapBuffers();
        
        // Check for stopping conditions
        if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
            break;
    }
    
    glfwTerminate();
    return EXIT_SUCCESS;
}