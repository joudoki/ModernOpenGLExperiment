#include <cstdio>
#include <cstdlib>

#include "Rendering.h"

#include "Shader.h"
#include "Program.h"
#include "Mesh.h"
#include "Texture.h"

#include "Trackball.h"

#include <iostream>
#include <fstream>
#include <string>

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
    glClearColor(0.9, 0.9, 0.9, 1.0);

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

string readFile(const char* fileName) {
    ifstream in(fileName);
    if (!in) throw(errno);

    string contents = string(istreambuf_iterator<char>(in), istreambuf_iterator<char>());

    in.close();

    return contents;
}

Program* MakeProgram(const string& vertexShaderSource, const string& fragmentShaderSource) {
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
    float data[] = {
        -1.0f, -1.0f, -1.0f,     0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,     0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,     1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,     1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,     0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,     0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,     1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,     1.0f, 1.0f
    };

    GLubyte indices[] = {
        2,3,1, 0,2,1,
        7,5,3, 5,1,3,
        6,4,7, 4,5,7,
        2,0,6, 0,4,6,
        6,3,2, 3,6,7,
        4,0,1, 1,5,4
    };

    GLsizei stride = 5*sizeof(GLfloat);
    VertexAttributeBinding_t vertFmt[] = {
        {program->GetAttributeID("coord"), 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0)},
        {program->GetAttributeID("tex"),   2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3*sizeof(GLfloat))}
    };

    Mesh* mesh = new Mesh(TrianglesPrimitive, vertFmt, 2);
    mesh->SetVertexData(8, sizeof(data), data);
    mesh->SetIndexData(UnsignedByteIndex, 36, sizeof(indices), indices);

    return mesh;
}
Mesh* MakeAxisMesh(Program* program, float r) {
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
    
    GLsizei stride = 6*sizeof(GLfloat);
    VertexAttributeBinding_t vertFmt[] = {
        {program->GetAttributeID("coord"), 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0)},
        {program->GetAttributeID("color"), 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3*sizeof(GLfloat))}
    };

    Mesh* mesh = new Mesh(LinesPrimitive, vertFmt, 2);
    mesh->SetVertexData(8, sizeof(data), data);
    mesh->SetIndexData(UnsignedByteIndex, 36, sizeof(indices), indices);

    return mesh;
}

Mesh* FromMD3(const char* fileName) {
    return NULL;
}

int main(int argc, char** argv) {
    printf("  GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    ifstream modelFile("assets/rocket.md3", ios::binary);
    if (!modelFile) throw (errno);
    
    // Check magic and version
    int version;
    char magic[5] = {0};

    modelFile.read(magic, 4*sizeof(char));
    modelFile.read((char*)&version, sizeof(int));

    if (strcmp(magic, "IDP3") != 0 || version != 15) {
        printf("Got unexpected file format \"%s\" v%d, expecting \"IDP3\" v15\n", magic, version);
        return EXIT_FAILURE;
    }

    struct {
        char name[64];
        int flags;

        int numFrmes;
        int numTag;
        int numSurfaces;
        int numSkins;

        int offsetFrames;
        int offsetTags;
        int offsetSurfaces;
        int offsetEOF;
    } header;

    modelFile.read((char*)&header, sizeof(header));

    modelFile.close();


    return EXIT_SUCCESS;

    int width = 800, height = 600;
    if (!acquireContext(width, height)) return EXIT_FAILURE;
    if (!acquireFunctions()) return EXIT_FAILURE;
    
    setupOpenGL();

    printf("\nOpenGL:\n");
    printf("  [GL_VENDOR]:     %s;\n", glGetString(GL_VENDOR));
    printf("  [GL_RENDERER]:   %s;\n", glGetString(GL_RENDERER));
    printf("  [GL_VERSION]:    %s;\n", glGetString(GL_VERSION));

    Program* flatShade = MakeProgram(
        readFile("glsl/flatShade.vert"),
        readFile("glsl/flatShade.frag")
    );

    Program* textured = MakeProgram(
        readFile("glsl/textured.vert"),
        readFile("glsl/textured.frag")
    );

    Texture* tex = Texture::LoadFromFile("assets/rockammo2.tga");
    tex->Bind(0);

    textured->Bind();
    glUniform1i(textured->GetUniformID("tex"), 0);

    if (flatShade == NULL || textured == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Setup objects
    Mesh* cube = MakeCubeMesh(textured);
    Mesh* axis = MakeAxisMesh(flatShade, 8.0f);

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
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            flatShade->Bind();
            glUniformMatrix4fv(flatShade->GetUniformID("transform"), 1, GL_FALSE, glm::value_ptr(matrix));
            axis->Render();

            textured->Bind();
            glUniformMatrix4fv(textured->GetUniformID("transform"), 1, GL_FALSE, glm::value_ptr(matrix));
            cube->Render();

            glfwSwapBuffers();

            lastTime = time;
        }
    } while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

    // Cleanup
    delete cube;
    delete axis;
    delete flatShade;
    delete textured;
    delete tex;
    
    glfwTerminate();
    return EXIT_SUCCESS;
}