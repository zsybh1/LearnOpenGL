#pragma once

#include "Object.h"

namespace psi {

class IndexObject : public Object {
public:
    IndexObject() = default;
    // vertices : 顶点数据
    // verticeSize : 顶点数据占内存的总长，字节单位
    // split : 顶点数据的分割方法
    // indices : 索引数据
    // indiceSize : 索引数据占内存的总长，字节单位
    IndexObject(const float* vertices, size_t verticeSize, std::initializer_list<int> split, const unsigned* indices, size_t indiceSize) {
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

        int vertexSize = 0;
        std::for_each(split.begin(), split.end(), [&](int num) {vertexSize += num; });

        int accum = 0;
        for (int i = 0; i < split.size(); ++i) {
            glVertexAttribPointer(i, *(split.begin() + i), GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void*)(accum * sizeof(float)));
            glEnableVertexAttribArray(i);
            accum += *(split.begin() + i);
        }

        // unbind to prevent unexpect change
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        vertexNum = indiceSize / sizeof(unsigned);
    }

    virtual void Draw(GLenum primitive = GL_TRIANGLES) override {
        glBindVertexArray(VAO);
        glDrawElements(primitive, vertexNum, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    virtual ~IndexObject() = default;
};

}