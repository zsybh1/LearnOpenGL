#pragma once

#include "../global.h"

namespace psi {

class Object {
public:
    unsigned VAO = 0;

    Object() = default;
    Object(unsigned vao, int vertNum) : VAO(vao), vertexNum(vertNum) {}
    Object(const std::vector<float>& vertices, const std::initializer_list<int>& split)
        : Object(vertices.data(), vertices.size() * sizeof(float), split) {}
    // vertices : 顶点数据
    // verticeSize : 顶点数据占内存的总长，字节单位
    // split : 顶点数据的分割方法
    Object(const float* vertices, size_t verticeSize, const std::initializer_list<int>& split) {
        // create object to save data
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);

        // load data to GPU memory and tell GPU how to interpret memory
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verticeSize, vertices, GL_STATIC_DRAW);

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

        vertexNum = verticeSize / sizeof(float) / vertexSize;
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