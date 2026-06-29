#include "Texture.h"

#include <iostream>

void Texture::Load(const char* path) {
    int width, height, channels;
    unsigned char* data = stbi_load(
        path, &width, &height, &channels, 4
    );
    if (data == nullptr) {
        std::cerr << "Unable to load image from: " << path << '\n';
        return;
    }

    std::cout << "Loaded texture: "
        << path
        << " [width: " << width
        << ", height: " << height
        << ", channels: " << channels << ']'
        << '\n';

    //// Cause of some troubles
    // GLenum format = GL_RGB;
    // if (channels == 4) {
    //     format = GL_RGBA;
    // } else if (channels == 1) {
    //     format = GL_RED;
    // }

    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    stbi_image_free(data);
}

GLuint Texture::GetId() const {
    return m_TextureId;
}
