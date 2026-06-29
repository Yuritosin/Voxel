#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GL/gl.h>

#include "stb_image/stb_image.h"

class Texture {
public:
    ~Texture() = default;
    Texture() = default;

    void Load(const char* path);

    GLuint GetId() const;
private:
    GLuint m_TextureId = 0;
    // int m_Width = 0;
    // int m_Height = 0;
    // int m_Channels = 0;
};

#endif
