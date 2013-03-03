#include "Program.h"

Program::~Program() {
    glDeleteProgram(programHandle);
    programHandle = 0;

    /*
    if (uniforms != NULL) {
        delete[] uniforms;
        uniforms = NULL;
    }

    if (attributes != NULL) {
        delete[] attributes;
        attributes = NULL;
    }
    */
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
        //uniforms = new Uniform_t[activeUniforms];

        GLchar* uniformName = new GLchar[maxNameLength];
        GLint uniformSize;
        GLenum uniformType;

        for (int i=0; i<activeUniforms; ++i) {
            glGetActiveUniform(programHandle, i, maxNameLength, NULL, &uniformSize, &uniformType, uniformName);

            // At some point, when I'm finally dealing with textures, I'll have to deal with samplers here

            Uniform_t uniform = {
                std::string(uniformName),
                glGetUniformLocation(programHandle, uniformName),
                uniformType,
                uniformSize
            };

            // Add to uniform map
            uniforms[uniformName] = uniform;
        }

        delete[] uniformName;
    }
}

const Uniform_t* Program::GetUniform(const std::string& uniformName) const {
    auto it = uniforms.find(uniformName);
    
    return (it == uniforms.end())
        ? NULL
        : &((*it).second);
}

GLuint Program::GetUniformID(const std::string& name) const {
    const Uniform_t* uniform = GetUniform(name);
    return uniform == NULL
        ? 0
        : uniform->location;
}

void Program::AcquireAttributes() {
    GLint activeAttributes, maxNameLength;
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);

    if (activeAttributes > 0 && maxNameLength > 0) {
        //attributes = new VertexAttribute_t[activeAttributes];

        GLchar* attribName = new GLchar[maxNameLength];
        GLint attribSize;
        GLenum attribType;

        for (int i=0; i<activeAttributes; ++i) {
            glGetActiveAttrib(programHandle, i, maxNameLength, NULL, &attribSize, &attribType, attribName);

            VertexAttribute_t attribute = {
                std::string(attribName),
                glGetAttribLocation(programHandle, attribName),
                attribType,
                attribSize
            };

            attributes[attribName] = attribute;
        }

        delete[] attribName;
    }
}

const VertexAttribute_t* Program::GetAttribute(const std::string& attrName) const {
    auto it = attributes.find(attrName);
    
    return (it == attributes.end())
        ? NULL
        : &((*it).second);
}

GLuint Program::GetAttributeID(const std::string& attrName) const {
    const VertexAttribute_t* attr = GetAttribute(attrName);
    return attr == NULL
        ? 0
        : attr->location;
}

Program* Program::CreateFromShaders(Shader<VertexShader>* vertexShader, Shader<FragmentShader>* fragmentShader) {
    Program* program = new Program();

    glAttachShader(program->programHandle,   vertexShader->GetHandle());
    glAttachShader(program->programHandle, fragmentShader->GetHandle());

    program->Link();

    if (program->IsValid()) {
        program->AcquireUniforms();
        program->AcquireAttributes();
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