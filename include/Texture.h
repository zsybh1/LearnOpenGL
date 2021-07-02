#pragma once

#include "global.h"
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace psi {

class Texture {
public:
    unsigned texture;

    Texture() = default;
    Texture(const std::string& path, bool SRGB = true) {
        stbi_set_flip_vertically_on_load(true);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            if (nrChannels == 3) {
                if (SRGB) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
                else {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
            }
            else if (nrChannels == 4) {
                if (SRGB) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                }
                else {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

                }
            }
            else if (nrChannels == 1) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else {
            std::cout << "Texture " << path << " not exists" << std::endl;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Use(int index = 0) {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    ~Texture() {
        glDeleteTextures(1, &texture);
    }
};
}

#undef STB_IMAGE_IMPLEMENTATION