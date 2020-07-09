#include <object_manager.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void Object_Manager::load(Object e) {
	switch (e) {
    case Object::rectangle:
    {
        float vertices[] = {
            0.5f,  0.5f, 0.0f,  //右上
           -0.5f,  0.5f, 0.0f,  //左上
           -0.5f, -0.5f, 0.0f,  //左下
            0.5f, -0.5f, 0.0f   //右下
        };
        unsigned VAO, VBO, EBO;
        unsigned index[] = {
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

        VAOs[(int)Object::rectangle] = VAO;
        break;
    }
    default:
    {
        std::cout << "Object_Manager::load()\nERROR: No such object to load!" << std::endl;
        break;
    }
	}
}

void Object_Manager::draw(Object e) {
    glBindVertexArray(VAOs[(int)Object::rectangle]);
    switch (e) {
    case Object::rectangle:
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        break;
    }
    default:
    {
        std::cout << "Object_Manager::draw()\nERROR: No such object to draw!" << std::endl;
        break;
    }
    }
    
}