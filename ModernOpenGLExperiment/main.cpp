#include <cstdio>
#include <cstdlib>

#include "Rendering.h"

#include "Shader.h"
#include "Program.h"
#include "Mesh.h"
#include "Texture.h"

#include "MD3Model.h"
#include "OBJModel.h"

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
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

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
    VertexShader*     vertexShader = VertexShader::CompileFromSource(vertexShaderSource);
    FragmentShader* fragmentShader = FragmentShader::CompileFromSource(fragmentShaderSource);

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

Mesh* MakeAxisMesh(Program* program) {
    float data[] = {
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
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

    Mesh* mesh = new Mesh(PrimitiveType::LinesPrimitive, vertFmt, 2);
    mesh->SetVertexData(8, sizeof(data), data);
    mesh->SetIndexData(IndexType::UnsignedByteIndex, 6, sizeof(indices), indices);

    return mesh;
}
Mesh* MakeAABBMesh(Program* program) {
    float data[] = {
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f
    };

    GLubyte indices[] = {
        0,1, 0,2, 0,4,
        1,3, 1,5, 2,3, 
        2,6, 3,7, 4,5,
        4,6, 5,7, 6,7
    };

    GLsizei stride = 3*sizeof(GLfloat);
    VertexAttributeBinding_t vertFmt[] = {
        {program->GetAttributeID("coord"), 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0)}
    };

    Mesh* mesh = new Mesh(PrimitiveType::LinesPrimitive, vertFmt, 1);
    mesh->SetVertexData(8, sizeof(data), data);
    mesh->SetIndexData(IndexType::UnsignedByteIndex, 24, sizeof(indices), indices);

    return mesh;
}

Mesh* LoadMD3Mesh(Program* program, MD3Model* model) {
    MeshVertex_t* vertexData = NULL;
    GLushort* indexData = NULL;
    size_t vertexCount, triangleCount, indexCount;

    model->GetVertices(0, 0, vertexData, vertexCount);
    model->GetIndices(0,     indexData,  triangleCount);

    indexCount = triangleCount * 3;

    GLsizei stride = 8*sizeof(GLfloat);
    VertexAttributeBinding_t vertFmt[] = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0)},
        {1, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3*sizeof(GLfloat))},
        {2, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(6*sizeof(GLfloat))}
    };

    Mesh* mesh = new Mesh(PrimitiveType::TrianglesPrimitive, vertFmt, 3);
    mesh->SetVertexData(vertexCount, vertexCount*stride, vertexData);
    mesh->SetIndexData(IndexType::UnsignedShortIndex, indexCount, indexCount*sizeof(GLushort), indexData);

    delete[] vertexData;
    delete[] indexData;

    return mesh;
}
Mesh* LoadOBJMesh(Program* program, OBJModel* model) {
    MeshVertex_t* vertexData = NULL;
    GLushort* indexData = NULL;
    size_t vertexCount, triangleCount, indexCount;

    model->GetVertices(0, vertexData, vertexCount);
    model->GetIndices(0,  indexData,  triangleCount);
    indexCount = triangleCount * 3;

    GLsizei stride = 8*sizeof(GLfloat);
    VertexAttributeBinding_t vertFmt[] = {
        {0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0)},
        {1, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3*sizeof(GLfloat))},
        {2, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(6*sizeof(GLfloat))}
    };

    Mesh* mesh = new Mesh(PrimitiveType::TrianglesPrimitive, vertFmt, 3);
    mesh->SetVertexData(vertexCount, vertexCount*stride, vertexData);
    mesh->SetIndexData(IndexType::UnsignedShortIndex, indexCount, indexCount*sizeof(GLushort), indexData);

    delete[] vertexData;
    delete[] indexData;

    return mesh;
}

void setup(int width, int height) {
    printf("GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    if (!acquireContext(width, height)) exit(EXIT_FAILURE);
    if (!acquireFunctions()) exit(EXIT_FAILURE);
    
    setupOpenGL();

    // Print some info about OpenGL
    printf("\nOpenGL:\n");
    printf("  [GL_VENDOR]:     %s;\n", glGetString(GL_VENDOR));
    printf("  [GL_RENDERER]:   %s;\n", glGetString(GL_RENDERER));
    printf("  [GL_VERSION]:    %s;\n", glGetString(GL_VERSION));

}

int main(int argc, char** argv) {
    int width = 800, height = 600;
    setup(width, height);

    // Load shaders
    Program* textureShader = MakeProgram(
        readFile("glsl/textured.vert"),
        readFile("glsl/textured.frag")
    );

    Program* flatShader = MakeProgram(
        readFile("glsl/flatShade.vert"),
        readFile("glsl/flatShade.frag")
    );

    if (textureShader == NULL || flatShader == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Load textures
    Texture* texture0 = Texture::LoadFromFile("textures/railgun1.tga");
    Texture::Bind(0, texture0);
    
    // Setup uniforms that are constant over lifetime of shader
    textureShader->Bind();
    Program::SetUniform(textureShader->GetUniform("diffuseSampler"), (GLuint)0);

    // Setup objects
    MD3Model* model = MD3Model::LoadFromFile("models/railgun.md3");

    if (model == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    float cameraDistance = 2.0f * model->GetFrame(0)->radius;
    Mesh* ammoBox = LoadMD3Mesh(textureShader, model);

    delete model;

    Mesh* axis    = MakeAxisMesh(flatShader);

    // Setup trackball interface
    Trackball trackball(width, height, 1.0f, glm::mat4());
    
    glm::mat4 project = glm::perspectiveFov(70.0f, (float) width, (float) height, 1.0f, 1024.0f);
    glm::mat4 viewTranslate = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -cameraDistance));
    glm::mat4 viewRotate = glm::mat4();
    
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
            glm::mat4 viewClip = project * viewTranslate * viewRotate;
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render model
            textureShader->Bind();
            Program::SetUniform(textureShader->GetUniform("transform"), viewClip);
            ammoBox->Render();

            flatShader->Bind();
            Program::SetUniform(flatShader->GetUniform("transform"), project * viewRotate);
            axis->Render();

            glfwSwapBuffers();
            lastTime = time;
        }
    } while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

    // Cleanup
    delete ammoBox;
    delete axis;

    delete textureShader;
    delete flatShader;

    delete texture0;
    
    glfwTerminate();
    return EXIT_SUCCESS;
}