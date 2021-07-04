#pragma once

#include <Texture/Texture.h>
#include <Texture/Cubemap.h>

namespace psi {

class TextureLoader {
public:
    static Ptr<Texture> LoadTexture(const std::string& path, bool sRGB = true, int warpSetting = GL_REPEAT) {
        stbi_set_flip_vertically_on_load(true);
        unsigned texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warpSetting);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warpSetting);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            if (nrChannels == 3) {
                if (sRGB) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
                else {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
            }
            else if (nrChannels == 4) {
                if (sRGB) {
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
            std::cout << "ERROR::Texture " << path << " not exists" << std::endl;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        return New<Texture>(texture);
    }


    // paths order:
    // right, left, top, buttom, back, front
    static Ptr<Texture> LoadCubemap(const std::vector<std::string>& paths, bool sRGB = true) {
        stbi_set_flip_vertically_on_load(false);
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        unsigned char* data;
        for (unsigned int i = 0; i < paths.size(); i++)
        {
            data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                int dest_type_option = GL_RGB;
                if (sRGB) dest_type_option = GL_SRGB;
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, dest_type_option, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
            }
            else {
                std::cout << "ERROR::Texture " << paths[i] << " not exists" << std::endl;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return New<Cubemap>(textureID);
    }

};

}