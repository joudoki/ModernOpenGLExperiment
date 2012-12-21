#include "GLProgram.h"

void GLProgram::PrintLog(GLuint object) {
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

GLuint GLProgram::CompileShader(const char* source, GLenum type) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);

    GLint compileResult;
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileResult);

    if (!compileResult) {
        fprintf(stderr, "Error in vertex shader\n");

        PrintLog(id);
        glDeleteShader(id);

        return 0;
    }

    return id;
}

GLuint GLProgram::LinkProgram(GLuint vertID, GLuint fragID) {
    GLuint id = glCreateProgram();
    glAttachShader(id, vertID);
    glAttachShader(id, fragID);

    GLint linkResult;
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &linkResult);

    if (!linkResult) {
        fprintf(stderr, "Error during linking\n");
        
        PrintLog(id);
        glDeleteProgram(id);

        return 0;
    }

    return id;
}

GLProgram* GLProgram::Create(
    const char* vertShader,
    const char* fragShader,
    std::vector<std::string> attributes
) {
    GLuint vertID, fragID, progID;

    // Compile and link the shaders
    if (!(vertID = CompileShader(vertShader, GL_VERTEX_SHADER))) return NULL;
    if (!(fragID = CompileShader(fragShader, GL_FRAGMENT_SHADER))) return NULL;
    if (!(progID = LinkProgram(vertID, fragID))) return NULL;
    
    GLProgram* program = new GLProgram(progID, vertID, fragID);

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

GLProgram::~GLProgram() {
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);
    glDeleteProgram(programID);
}