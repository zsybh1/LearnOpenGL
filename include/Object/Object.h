#pragma once

#include "../global.h"
#include <numeric>

namespace psi {

class Object {
public:
    unsigned VAO = 0;

    Object() = default;
    Object(unsigned vao, int vertNum) : VAO(vao), vertexNum(vertNum) {}
    Object(const std::vector<float>& vertices, const std::vector<int>& split, bool calcTangent = false)
        : Object(vertices.data(), vertices.size() * sizeof(float), split, calcTangent) {}
    // vertices : 顶点数据
    // verticeSize : 顶点数据占内存的总长，字节单位
    // split : 顶点数据的分割方法
    Object(const float* vertices, size_t verticeSize, const std::vector<int>& split, bool calcTangent = false) {
        // create object to save data
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);

        // load data to GPU memory and tell GPU how to interpret memory
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verticeSize, vertices, GL_STATIC_DRAW);

        int vertexSize = std::accumulate(split.begin(), split.end(), 0);

        int accum = 0;
        for (int i = 0; i < split.size(); ++i) {
            glVertexAttribPointer(i, *(split.begin() + i), GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void*)(accum * sizeof(float)));
            glEnableVertexAttribArray(i);
            accum += *(split.begin() + i);
        }

        vertexNum = verticeSize / sizeof(float) / vertexSize;

        if (calcTangent && vertexSize == 8) {   // 只在顶点数据按pos、normal、uv的方式排列时下列计算有效
            std::vector<float> tangentData(vertexNum * 3);
            for (int i = 0; i < vertexNum/3; ++i) {
                Vec2f uv1(vertices[24 * i + 6], vertices[24 * i + 7]);
                Vec2f uv2(vertices[24 * i + 8 + 6], vertices[24 * i + 8 + 7]);
                Vec2f uv3(vertices[24 * i + 16 + 6], vertices[24 * i + 16 + 7]);
                
                Vec3f pos1(vertices[24 * i], vertices[24 * i + 1], vertices[24 * i + 2]);
                Vec3f pos2(vertices[24 * i + 8], vertices[24 * i + 8 + 1], vertices[24 * i + 8 + 2]);
                Vec3f pos3(vertices[24 * i + 16], vertices[24 * i + 16 + 1], vertices[24 * i + 16 + 2]);

                Vec2f deltaUV1 = uv2 - uv1;
                Vec2f deltaUV2 = uv3 - uv2;
                Vec3f edge1 = pos2 - pos1;
                Vec3f edge2 = pos3 - pos2;

                float fact = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
                for (int j = 0; j < 3; ++j) {
                    tangentData[9 * i + 3 * j] = fact * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x());
                    tangentData[9 * i + 3 * j + 1] = fact * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y());
                    tangentData[9 * i + 3 * j + 2] = fact * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z());
                }
            }

            unsigned int tVBO;
            glGenBuffers(1, &tVBO);
            glBindBuffer(GL_ARRAY_BUFFER, tVBO);
            glBufferData(GL_ARRAY_BUFFER, tangentData.size() * sizeof(float), tangentData.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(split.size(), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
            glEnableVertexAttribArray(split.size());
        }

        // unbind to prevent unexpect change
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    virtual void Draw(GLenum primitive = GL_TRIANGLES) {
        glBindVertexArray(VAO);
        glDrawArrays(primitive, 0, vertexNum);
    }

    virtual ~Object() {
        glDeleteVertexArrays(1, &VAO);
    };

    int vertexNum = 0;
};

}