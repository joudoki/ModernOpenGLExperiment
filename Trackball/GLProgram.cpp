#include "GLProgram.h"

GLProgram* GLProgram::Compile(
    const char* vertShader,
    const char* fragShader,
    std::vector<std::string> attributes
) {
    GLint compileResult = GL_FALSE, linkResult = GL_FALSE;
    GLuint vertID, fragID, progID;

    // Create and compile the vertex shader
    vertID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertID, 1, &vertShader, NULL);
    glCompileShader(vertID);

    // Check the compilation status of the vertex shader
    glGetShaderiv(vertID, GL_COMPILE_STATUS, &compileResult);

    if (!compileResult) {
        fprintf(stderr, "Error in vertex shader\n");
        //printLog(vertID);
        // TODO: free the shader?
        return NULL;
    }

    // Create and compile the fragment shader
    fragID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragID, 1, &fragShader, NULL);
    glCompileShader(fragID);

    // Check compilation of fragment shader
    glGetShaderiv(fragID, GL_COMPILE_STATUS, &compileResult);

    if (!compileResult) {
        fprintf(stderr, "Error in fragment shader\n");
        //printLog(fragID);
        // TODO: free the shader?
        return NULL;
    }

    // Link the two into a program
    progID = glCreateProgram();
    glAttachShader(progID, vertID);
    glAttachShader(progID, fragID);
    glLinkProgram(progID);

    // Check link status
    glGetProgramiv(progID, GL_LINK_STATUS, &linkResult);

    if (!linkResult) {
        fprintf(stderr, "Error during linking\n");
        //printLog(progID);
        // TODO: free the shader?
        return NULL;
    }

    GLProgram* program = new GLProgram();
        
    program->vertShaderID = vertID;
    program->fragShaderID = fragID;
    program->programID = progID;

    for (auto it = attributes.begin(); it != attributes.end(); ++it) {
        GLuint attribID = glGetAttribLocation(progID, (*it).c_str());

        if (attribID == -1) {
            fprintf(stderr, "Could not bind attribute %s\n", (*it));
            delete program;
            return NULL;
        }

        program->attribs[(*it)] = attribID;
    }

    return program;
}

void GLProgram::Activate() {
    glUseProgram(programID);
}