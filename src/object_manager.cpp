#include <object_manager.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void Object_Manager::load(Object e) {
    unsigned VAO, VBO, EBO;
	switch (e) {
    case Object::rectangle:
    {
        float vertices[] = {
            0.5f,  0.5f, 0.0f,  //右上
           -0.5f,  0.5f, 0.0f,  //左上
           -0.5f, -0.5f, 0.0f,  //左下
            0.5f, -0.5f, 0.0f   //右下
        };
        
        unsigned indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

        break;
    }
    case Object::sphere_normal:
    {
        int edges = 20;

        glGenVertexArrays(1, &VAO); 
        glGenBuffers(1, &EBO);  
        glGenBuffers(1, &VBO);  
        glBindVertexArray(VAO);

        float *vertices = new float[12 * edges * (edges - 1) + 12];
        unsigned int *indices = new unsigned int[12 * edges * (edges - 1)];
        vertices[0] = vertices[2] = vertices[3] = vertices[5] = 0.0f;
        vertices[1] = 1.0f;
        vertices[4] = 1.0f;
        vertices[6] = vertices[8] = vertices[9] = vertices[11] = 0.0f;
        vertices[7] = -1.0f;
        vertices[10] = -1.0f;
        for (int i = 0; i < edges - 1; ++i) {
            float theta = glm::radians(180.0f / edges * (i + 1));
            for (int j = 0; j < edges * 2; ++j) {
                float gamma = glm::radians(180.0f / edges * j);
                vertices[6 * (2 + i * 2 * edges + j)] = glm::sin(theta) * glm::cos(gamma);
                vertices[6 * (2 + i * 2 * edges + j) + 1] = glm::cos(theta);
                vertices[6 * (2 + i * 2 * edges + j) + 2] = glm::sin(theta) * glm::sin(gamma);
                vertices[6 * (2 + i * 2 * edges + j) + 3] = glm::sin(theta) * glm::cos(gamma);
                vertices[6 * (2 + i * 2 * edges + j) + 4] = glm::cos(theta);
                vertices[6 * (2 + i * 2 * edges + j) + 5] = glm::sin(theta) * glm::sin(gamma);
            }
        }
        for (int i = 0; i < edges - 1; ++i) {
            for (int j = 0; j < 2 * edges; ++j) {
                indices[6 * (i * 2 * edges + j)] = i * 2 * edges + j + 3;
                indices[6 * (i * 2 * edges + j) + 1] = i * 2 * edges + j + 2;
                if (i == 0) indices[6 * (i * 2 * edges + j) + 2] = 0;
                else indices[6 * (i * 2 * edges + j) + 2] = (i - 1) * 2 * edges + j + 2;

                indices[6 * (i * 2 * edges + j) + 3] = (i + 1) * 2 * edges + j + 3;
                indices[6 * (i * 2 * edges + j) + 4] = indices[6 * (i * 2 * edges + j) + 1];
                indices[6 * (i * 2 * edges + j) + 5] = indices[6 * (i * 2 * edges + j)];
                if (j == 2 * edges - 1) {
                    indices[6 * (i * 2 * edges + j) + 5] = indices[6 * (i * 2 * edges + j)] = indices[6 * (i * 2 * edges) + 1];
                    indices[6 * (i * 2 * edges + j) + 3] = (i + 1) * 2 * edges + 2;
                }
                if (i == edges - 2) indices[6 * (i * 2 * edges + j) + 3] = 1;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (12 * edges * (edges - 1) + 12) * sizeof(float), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * edges * (edges - 1) * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        break;
    }
    case Object::cube_normal_texture:
    {
        float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
      //  glEnableVertexAttribArray(1);
      //  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
      //  glEnableVertexAttribArray(2);
      //  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        break;
    }
    default:
    {
        std::cout << "Object_Manager::load()\nERROR: No such object to load!" << std::endl;
        break;
    }
	}
    VAOs[(int)e] = VAO;
}

void Object_Manager::draw(Object e) {
    glBindVertexArray(VAOs[(int)e]);
    switch (e) {
    case Object::rectangle:
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        break;
    }
    case Object::sphere_normal:
    {
        int edges = 20;
        glDrawElements(GL_TRIANGLES, 12 * edges * (edges - 1), GL_UNSIGNED_INT, 0);
        break;
    }
    case Object::cube_normal_texture:
    {
        glDrawArrays(GL_TRIANGLES, 0, 36);
        break;
    }
    default:
    {
        std::cout << "Object_Manager::draw()\nERROR: No such object to draw!" << std::endl;
        break;
    }
    }
}