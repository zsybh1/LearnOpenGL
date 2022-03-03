#pragma once

#include "Object.h"
#include <numeric>

namespace psi {

class IndexObject : public Object {
public:
    IndexObject() = default;
    IndexObject(const std::vector<float>& vertices, const std::vector<int>& split, const std::vector<unsigned>& indices, bool calcTangent = false)
        : IndexObject(vertices.data(), vertices.size()*sizeof(float), split, indices.data(), indices.size() * sizeof(unsigned), calcTangent) {}
    // vertices : 顶点数据
    // verticeSize : 顶点数据占内存的总长，字节单位
    // split : 顶点数据的分割方法
    // indices : 索引数据
    // indiceSize : 索引数据占内存的总长，字节单位
    IndexObject(const float* vertices, size_t verticeSize, const std::vector<int>& split, const unsigned* indices, size_t indiceSize, bool calcTangent = false) {
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        unsigned int EBO;
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verticeSize, vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceSize, indices, GL_STATIC_DRAW);

        int vertexSize = std::accumulate(split.begin(), split.end(), 0);

        int accum = 0;
        for (int i = 0; i < split.size(); ++i) {
            glVertexAttribPointer(i, *(split.begin() + i), GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void*)(accum * sizeof(float)));
            glEnableVertexAttribArray(i);
            accum += *(split.begin() + i);
        }

        vertexNum = indiceSize / sizeof(unsigned);

        if (calcTangent && vertexSize == 8) {   // 只在顶点数据按pos、normal、uv的方式排列时下列计算有效
            int pointNum = verticeSize / sizeof(float) / 8;
            std::vector<bool> flags(pointNum);    // 标记哪些顶点已经被计算
            std::vector<float> tangentData(pointNum * 3);
            for (int i = 0; i < vertexNum / 3; ++i) {
                Vec2f uv1(vertices[8 * indices[3 * i] + 6], vertices[8 * indices[3 * i] + 7]);
                Vec2f uv2(vertices[8 * indices[3 * i + 1] + 6], vertices[8 * indices[3 * i + 1] + 7]);
                Vec2f uv3(vertices[8 * indices[3 * i + 2] + 6], vertices[8 * indices[3 * i + 2] + 7]);

                Vec3f pos1(vertices[8 * indices[3 * i]], vertices[8 * indices[3 * i] + 1], vertices[8 * indices[3 * i] + 2]);
                Vec3f pos2(vertices[8 * indices[3 * i + 1]], vertices[8 * indices[3 * i + 1] + 1], vertices[8 * indices[3 * i + 1] + 2]);
                Vec3f pos3(vertices[8 * indices[3 * i + 2]], vertices[8 * indices[3 * i + 2] + 1], vertices[8 * indices[3 * i + 2] + 2]);

                Vec2f deltaUV1 = uv2 - uv1;
                Vec2f deltaUV2 = uv3 - uv2;
                Vec3f edge1 = pos2 - pos1;
                Vec3f edge2 = pos3 - pos2;

                Vec3f tangent;

                float fact = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
                for (int j = 0; j < 3; ++j) {
                    tangent.x() = fact * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x());
                    tangent.y() = fact * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y());
                    tangent.z() = fact * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z());
                }

                

                for (int j = 0; j < 3; ++j) {
                    if (!flags[indices[3 * i + j]]) {
                        flags[indices[3 * i + j]] = true;
                        Vec3f normal(vertices[8 * indices[3 * i + j] + 3], vertices[8 * indices[3 * i + j] + 4], vertices[8 * indices[3 * i + j] + 5]);
                        Vec3f vertexTangent = (tangent - tangent.dot(normal) * normal).normalized();
                        tangentData[3 * indices[3 * i + j]] = vertexTangent.x();
                        tangentData[3 * indices[3 * i + j] + 1] = vertexTangent.y();
                        tangentData[3 * indices[3 * i + j] + 2] = vertexTangent.z();
                    }
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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    virtual void Draw(GLenum primitive = GL_TRIANGLES) override {
        glBindVertexArray(VAO);
        glDrawElements(primitive, vertexNum, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    virtual ~IndexObject() = default;
};

}