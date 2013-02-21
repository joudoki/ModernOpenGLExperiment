#ifndef TEXTURE_H
#define TEXTURE_H

#include "Rendering.h"

class Texture {
private:
    GLuint id;

    Texture();

public:
    static Texture* LoadFromFile(const char* filename);

    ~Texture();
};

#endif