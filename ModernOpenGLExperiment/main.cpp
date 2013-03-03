#include <cstdio>
#include <cstdlib>

#include "Rendering.h"

#include "Shader.h"
#include "Program.h"
#include "Mesh.h"

#include "Trackball.h"

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std;

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

    glfwDisable(GLFW_AUTO_POLL_EVENTS);

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
    glCullFace(GL_FRONT);

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

Program* MakeProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    Shader<VertexShader>* vertexShader = Shader<VertexShader>::CompileFromSource(vertexShaderSource);
    Shader<FragmentShader>* fragmentShader = Shader<FragmentShader>::CompileFromSource(fragmentShaderSource);

    if (!vertexShader->IsValid()) {
        fprintf(stderr, "Compile Error: %s\n", vertexShader->GetCompileLog().c_str());

        delete vertexShader;

        return NULL;
    }

    if (!fragmentShader->IsValid()) {
        fprintf(stderr, "Compile Error: %s\n", fragmentShader->GetCompileLog().c_str());

        delete vertexShader;
        delete fragmentShader;

        return NULL;
    }

    Program* program = Program::CreateFromShaders(vertexShader, fragmentShader);

    // Don't need shaders after the program is linked
    delete vertexShader;
    delete fragmentShader;
    
    if (!program->IsValid()) {
        fprintf(stderr, "Link Error: %s\n", program->GetLinkLog().c_str());
        
        delete program;

        return NULL;
    }

    return program;
}

Mesh* MakeCubeMesh(Program* program) {
    GLuint attrCoord = program->GetAttributeID("vCoord");
    GLuint attrColor = program->GetAttributeID("vColor");

    VertexAttributeBinding_t vertFmt[] = {
        {attrCoord, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), BUFFER_OFFSET(0)},
        {attrColor, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat))}
    };

    Mesh* mesh = new Mesh(TrianglesPrimitive, vertFmt, 2);
    
    float data[] = {
        -1.0f, -1.0f, -1.0f,     0.5f, 0.5f, 0.5f,
        -1.0f, -1.0f,  1.0f,     0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,     0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,     0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,     1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,     1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,     1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f
    };

    GLubyte indices[] = {
        2,3,1, 0,2,1,
        7,5,3, 5,1,3,
        6,4,7, 4,5,7,
        2,0,6, 0,4,6,
        6,3,2, 3,6,7,
        4,0,1, 1,5,4
    };

    mesh->SetVertexData(8, sizeof(data), data);
    mesh->SetIndexData(UnsignedByteIndex, 36, sizeof(indices), indices);

    return mesh;
}
Mesh* MakeAxisMesh(Program* program, float r) {
    GLuint attrCoord = program->GetAttributeID("vCoord");
    GLuint attrColor = program->GetAttributeID("vColor");

    VertexAttributeBinding_t vertFmt[] = {
        {attrCoord, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), BUFFER_OFFSET(0)},
        {attrColor, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), BUFFER_OFFSET(3*sizeof(GLfloat))}
    };

    Mesh* mesh = new Mesh(LinesPrimitive, vertFmt, 2);
    
    float data[] = {
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        r,    0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.0f, r,    0.0f,   0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, r,      0.0f, 0.0f, 1.0f,
    };

    GLubyte indices[] = {
        1, 0,
        2, 0,
        3, 0
    };

    mesh->SetVertexData(8, sizeof(data), data);
    mesh->SetIndexData(UnsignedByteIndex, 36, sizeof(indices), indices);

    return mesh;
}

int main(int argc, char** argv) {
    printf("  GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    
    int width = 800, height = 600;
    if (!acquireContext(width, height)) return EXIT_FAILURE;
    if (!acquireFunctions()) return EXIT_FAILURE;
    
    setupOpenGL();

    printf("\nOpenGL:\n");
    printf("  [GL_VENDOR]:     %s;\n", glGetString(GL_VENDOR));
    printf("  [GL_RENDERER]:   %s;\n", glGetString(GL_RENDERER));
    printf("  [GL_VERSION]:    %s;\n", glGetString(GL_VERSION));

    Program* program = MakeProgram(
        readFile("glsl/one.vert"),
        readFile("glsl/one.frag")
    );

    if (program == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Setup the program & shaders
    program->Bind();

    GLuint uniformTransform = program->GetUniform("transform")->location;
    
    // Setup objects
    Mesh* cube = MakeCubeMesh(program);
    Mesh* axis = MakeAxisMesh(program, 8.0f);

    // Setup trackball interface
    Trackball trackball(width, height, 1.0f, glm::mat4(1.0f));
    
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.125f));

    glm::mat4 viewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    glm::mat4 viewRotate = glm::mat4(1.0f);

    glm::mat4 project = glm::perspectiveFov(70.0f, (float) width, (float) height, 1.0f, 128.0f);

    float time(0.0f), lastTime(0.0f);

    do {
        time = (float) glfwGetTime();

        glfwPollEvents();

        //float x = 1.5f * glm::cos(time), z = 1.5f * glm::sin(time);
        //glm::mat4 view = glm::lookAt(glm::vec3(x, 1.5f, z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // Update trackball state
        int mouseX, mouseY;
        glfwGetMousePos(&mouseX, &mouseY);
        trackball.MouseUpdate(
            glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS,
            mouseX, mouseY
        );

        // Only render up to 60FPS
        if (time - lastTime >= 1/60.0f) {
            // Update the transformation matrix
            viewRotate = trackball.GetRotationMatrix();
            glm::mat4 matrix = project * viewTranslate * viewRotate * model;

            glUniformMatrix4fv(
                uniformTransform,
                1,          // Count
                GL_FALSE,   // Do not transpose
                glm::value_ptr(matrix)
            );

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            axis->Render();
            cube->Render();

            glfwSwapBuffers();
        }
    } while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

    // Cleanup
    delete cube;
    delete axis;
    delete program;
    
    glfwTerminate();
    return EXIT_SUCCESS;
}