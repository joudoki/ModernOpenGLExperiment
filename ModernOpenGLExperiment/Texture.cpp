#include "Texture.h"

void Texture::Bind(GLuint i, Texture* texture) {
    assert(texture != NULL);

    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void Texture::SetFilters(GLenum magFilter, GLenum minFilter) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter); 
}

void Texture::SetRepeat(GLenum wrapS, GLenum wrapT) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

Texture* Texture::LoadFromFile(const char* filename) {
    // I'm just going to assume for now that the texture is a TGA
    // which GLFW has a builtin procedure for reading

    GLFWimage image;

    if (!glfwReadImage(filename, &image, GLFW_ORIGIN_UL_BIT))
        return NULL;

    Texture* tex = new Texture();
    tex->SetFilters(GL_LINEAR, GL_LINEAR);
    tex->SetRepeat(GL_REPEAT, GL_REPEAT);
    
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        image.Format,
        image.Width,
        image.Height,
        0,
        image.Format,
        GL_UNSIGNED_BYTE,
        (void*)image.Data
    );

    glfwFreeImage(&image);

    return tex;
}
