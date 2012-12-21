#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include "gl_core_3_3.h"

#include <map>
#include <string>
#include <vector>

class GLProgram
{
private:
    GLuint programID;
    GLuint vertShaderID;
    GLuint fragShaderID;

    //std::map<std::string, GLuint> attribs;

    GLProgram(GLuint progID, GLuint vertID, GLuint fragID) :
        programID(progID), vertShaderID(vertID), fragShaderID(fragID) {};
    
    static void PrintLog(GLuint object);
    static GLuint CompileShader(const char* source, GLenum type);
    static GLuint LinkProgram(GLuint vertID, GLuint fragID);

public:
    ~GLProgram();

    static GLProgram* Create(
        const char* vertShader,
        const char* fragShader,
        std::vector<std::string> attributes
    );

    void Activate();

    GLuint GetAttribute(std::string name) {
        GLuint id = glGetAttribLocation(programID, name.c_str());

        if (id == -1)
            fprintf(stderr, "Could not bind attribute %s\n", name);

        return id;
    }

    GLuint GetUniform(std::string name) {
        GLuint id = glGetUniformLocation(programID, name.c_str());

        if (id == -1)
            fprintf(stderr, "Could not bind attribute %s\n", name);

        return id;
    }
};

#endif