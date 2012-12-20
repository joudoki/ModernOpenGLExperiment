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

    GLProgram() {};

public:
    ~GLProgram() {};

    static GLProgram* Compile(
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