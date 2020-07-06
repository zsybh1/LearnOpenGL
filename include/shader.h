#ifndef SHADER_H_
#define SHADER_H_

#include<glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<sstream>
#include<fstream>
#include<string>

class Shader {
private:
	unsigned int transShader(const std::string &Path, int type);

public:
	unsigned int ID;

	Shader(const std::string &vPath, const std::string &fPath);
	void use();

	//uniform函数工具
    template <int num>
	void setf(const std::string &name, float value, ...);
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setInt(const std::string &name, int value);
    void setVec3(const std::string &name, const glm::vec3 &vec) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
    }
};

unsigned int Shader::transShader(const std::string &Path, int type) {
    // 1. 从文件路径中获取顶点/片段着色器
    std::string Code;
    std::ifstream ShaderFile;
    // 保证ifstream对象可以抛出异常：
    ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // 打开文件
        ShaderFile.open(Path);
        std::stringstream ShaderStream;
        // 读取文件的缓冲内容到数据流中
        ShaderStream << ShaderFile.rdbuf();
        // 关闭文件处理器
        ShaderFile.close();
        // 转换数据流到string
        Code = ShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char *ShaderCode = Code.c_str();

    unsigned int shader;
    int success;
    char infoLog[512];

    // 顶点着色器
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &ShaderCode, NULL);
    glCompileShader(shader);
    // 打印编译错误（如果有的话）
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
    return shader;
}
Shader::Shader(const std::string &vPath, const std::string &fPath) {
    unsigned int vertex = transShader(vPath, GL_VERTEX_SHADER);
    unsigned int fragment = transShader(fPath, GL_FRAGMENT_SHADER);
    int success;
    char infoLog[512];

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // 打印连接错误（如果有的话）
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
void Shader::use() {
    glUseProgram(ID);
}
template <int num>
void Shader::setf(const std::string &name, float value, ...) {
    va_list a;
    float para[4];
    para[0] = value;
    va_start(a, value);
    for (int i = 1; i < num; ++i) {
        para[i] = va_arg(a, double);
    }
    va_end(a);
    if(num == 1)glUniform1f(glGetUniformLocation(ID, name.c_str()), para[0]);
    else if(num == 2)glUniform2f(glGetUniformLocation(ID, name.c_str()), para[0], para[1]);
    else if (num == 3)glUniform3f(glGetUniformLocation(ID, name.c_str()), para[0], para[1], para[2]);
    else if (num == 4)glUniform4f(glGetUniformLocation(ID, name.c_str()), para[0], para[1], para[2], para[3]);
    else {
        std::cout << "ERROR:NUMBER OF PARAMENTS OF SET UNIFORM IS WRONG." << std::endl;
    }
}
void Shader::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
#endif