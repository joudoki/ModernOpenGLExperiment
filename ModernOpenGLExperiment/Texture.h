#ifndef TEXTURE_H
#define TEXTURE_H

#include "Rendering.h"

class Texture {
private:
    GLuint id;

    Texture() : id(0) { 
        glGenTextures(1, &id); 
        Bind();
    }

    void SetFilters(GLenum magFilter, GLenum minFilter);
    void SetRepeat(GLenum wrapS, GLenum wrapT);

public:
    static Texture* LoadFromFile(const char* filename);

    /**
     * Binds the given texture to the specified texture unit
     */
    static void Bind(GLuint i, Texture* texture);

    ~Texture() { 
        glDeleteTextures(1, &id);
    }

    void Bind() const {
        glBindTexture(GL_TEXTURE_2D, id);
    }

    GLuint GetID() const { return id; }
};

#endif