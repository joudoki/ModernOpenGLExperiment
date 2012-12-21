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

    std::map<std::string, GLuint> attribs;

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

    GLuint operator[] (std::string attrib) {
        auto it = attribs.find(attrib);
        return it == attribs.end()
            ? 0
            : it->second;
    }
};

#endif