#include <cstdio>
#include <cstdlib>

#include "Rendering.h"

#include "Shader.h"
#include "Program.h"
#include "Mesh.h"
#include "Texture.h"

#include "MD3Model.h"

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
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);

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
#define COORD(x,y,z,u,v) {glm::vec3(x,y,z), glm::vec2(u,v)}

    struct {
        glm::vec3 coord;
        glm::vec2 tex;
    } data[] = {
        COORD(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f),
        COORD(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
        COORD(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
        COORD(-1.0f,  1.0f,  1.0f, 1.0f, 1.0f),
        COORD( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f),
        COORD( 1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
        COORD( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
        COORD( 1.0f,  1.0f,  1.0f, 1.0f, 1.0f)
    };

#undef COORD

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

    Mesh* mesh = new Mesh(LinesPrimitive, vertFmt, 2);
    mesh->SetVertexData(8, sizeof(data), data);
    mesh->SetIndexData(UnsignedByteIndex, 6, sizeof(indices), indices);

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

    Mesh* mesh = new Mesh(LinesPrimitive, vertFmt, 1);
    mesh->SetVertexData(8, sizeof(data), data);
    mesh->SetIndexData(UnsignedByteIndex, 24, sizeof(indices), indices);

    return mesh;
}

Mesh* LoadMD3Mesh(Program* program, MD3Model* model) {
    MD3Model::Vertex_t* vertexData = NULL;
    GLushort* indexData = NULL;
    size_t vertexCount, triangleCount;

    model->GetVertices(0, vertexData, vertexCount);
    model->GetIndices(0, indexData, triangleCount);

    for (size_t i=0; i<vertexCount; ++i) {
        printf("%3d %10f %10f %10f | %10f %10f %10f | %10f %10f\n", i,
            vertexData[i].coord.x,    vertexData[i].coord.y,    vertexData[i].coord.z,
            vertexData[i].normal.x,   vertexData[i].normal.y,   vertexData[i].normal.z,
            vertexData[i].texCoord.x, vertexData[i].texCoord.y
        );
    }

    for (size_t i=0; i<triangleCount; ++i) {
        printf("%3d %6hd %6hd %6hd\n", i,
            indexData[3*i],
            indexData[3*i+1],
            indexData[3*i+2]
        );
    }

    GLsizei stride = 8*sizeof(GLfloat);
    VertexAttributeBinding_t vertFmt[] = {
        {program->GetAttributeID("coord"),  3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0)},
        {program->GetAttributeID("norm"),   3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3*sizeof(GLfloat))},
        {program->GetAttributeID("tex"),    2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(6*sizeof(GLfloat))}
    };

    Mesh* mesh = new Mesh(TrianglesPrimitive, vertFmt, 3);
    mesh->SetVertexData(vertexCount, vertexCount*sizeof(MD3Model::Vertex_t), vertexData);
    mesh->SetIndexData(UnsignedByteIndex, triangleCount*3, triangleCount*3*sizeof(GLushort), indexData);

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
    float cameraDistance = 64.0f;

    setup(width, height);

    // Load textures
    Texture* tex = Texture::LoadFromFile("assets/rockammo2.tga");
    Texture::Bind(0, tex);

    // Load shaders
    Program* flatShade = MakeProgram(
        readFile("glsl/flatShade.vert"),
        readFile("glsl/flatShade.frag")
    );

    Program* textured = MakeProgram(
        readFile("glsl/textured.vert"),
        readFile("glsl/textured.frag")
    );

    if (flatShade == NULL || textured == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Setup uniforms that are constant over lifetime of shader
    textured->Bind();
    Program::SetUniform(textured->GetUniform("tex"), 0);

    // Setup objects
    Mesh* axis = MakeAxisMesh(flatShade);
    glm::mat4 axisModel = glm::scale(glm::mat4(), glm::vec3(16.0f));

    MD3Model* model = MD3Model::LoadFromFile("assets/rocketam.md3");
    if (model == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    Mesh* ammoBox = LoadMD3Mesh(textured, model);

    Mesh* aabb = MakeAABBMesh(flatShade);
    glm::mat4 aabbModel = model->GetFrame(0).GetTransform();

    delete model;

    // Setup trackball interface
    Trackball trackball(width, height, 1.0f, glm::mat4());
    

    glm::mat4 viewTranslate = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -cameraDistance));
    glm::mat4 viewRotate = glm::mat4();

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
            glm::mat4 viewClip = project * viewTranslate * viewRotate;
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Render axis
            flatShade->Bind();
            Program::SetUniform(flatShade->GetUniform("transform"), viewClip * axisModel);
            axis->Render();

            Program::SetUniform(flatShade->GetUniform("transform"), viewClip * aabbModel);
            aabb->Render();

            // Render model
            textured->Bind();
            Program::SetUniform(textured->GetUniform("transform"), viewClip * glm::scale(glm::mat4(), glm::vec3(10.0f)));
            ammoBox->Render();

            glfwSwapBuffers();
            lastTime = time;
        }
    } while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

    // Cleanup
    delete axis;
    delete aabb;
    delete ammoBox;

    delete flatShade;
    delete textured;

    delete tex;
    
    glfwTerminate();
    return EXIT_SUCCESS;
}