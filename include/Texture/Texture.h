#pragma once

#include <global.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace psi {

class Texture {
public:
    unsigned texture;

    Texture() = default;
    explicit Texture(unsigned tex) : texture(tex) {}

    virtual void Use(int index = 0) {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    virtual ~Texture() {
        glDeleteTextures(1, &texture);
    }
};
}