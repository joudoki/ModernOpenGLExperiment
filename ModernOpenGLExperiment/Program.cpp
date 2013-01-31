#include "Program.h"

Program::~Program() {
    glDeleteProgram(programHandle);
    programHandle = 0;

    if (uniforms != NULL) {
        delete[] uniforms;
        uniforms = NULL;
    }
}

void Program::Link() {
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkResult);
}

void Program::AcquireUniforms() {
    GLint activeUniforms, maxNameLength;
    glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &activeUniforms);
    glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

    if (activeUniforms > 0 && maxNameLength > 0) {
        uniforms = new Uniform_t[activeUniforms];

        GLchar* uniformName = new char[maxNameLength];
        GLint uniformSize;
        GLenum uniformType;

        for (int i=0; i<activeUniforms; ++i) {
            glGetActiveUniform(programHandle, i, maxNameLength, NULL, &uniformSize, &uniformType, uniformName);

            Uniform_t uniform = {
                std::string(uniformName),
                glGetUniformLocation(programHandle, uniformName),
                uniformType,
                uniformSize
            };

            // Add to uniform map
            uniforms[i] = uniform;
        }

        delete[] uniformName;
    }
}

/*
void Program::AcquireAttributes() {

}
*/

Program* Program::CreateFromShaders(Shader<VertexShader>* vertexShader, Shader<FragmentShader>* fragmentShader) {
    Program* program = new Program();

    glAttachShader(program->programHandle,   vertexShader->GetHandle());
    glAttachShader(program->programHandle, fragmentShader->GetHandle());

    program->Link();

    if (program->IsValid()) {
        program->AcquireUniforms();
    }

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