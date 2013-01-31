#include "Program.h"

Program::~Program() {
    glDeleteProgram(programHandle);
    programHandle = 0;
}

void Program::Link() {
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkResult);
}

Program* Program::CreateFromShaders(Shader<VertexShader>* vertexShader, Shader<FragmentShader>* fragmentShader) {
    Program* program = new Program();

    glAttachShader(program->programHandle,   vertexShader->GetHandle());
    glAttachShader(program->programHandle, fragmentShader->GetHandle());

    program->Link();

    return program;
}

std::string Program::GetLinkLog() const {
    std::string log;
        
    GLint logLength;
    glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLength);

    log.resize(logLength);
    glGetProgramInfoLog(programHandle, logLength, NULL, &log[0]);

    return log;
}