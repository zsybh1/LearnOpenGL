#pragma once

#include <Texture/Texture.h>

namespace psi {

class Cubemap : public Texture {
public:
    Cubemap() = default;
    explicit Cubemap(unsigned tex) : Texture(tex) {}
    virtual ~Cubemap() = default;

    virtual void Use(int index = 0) override {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    }
};

}