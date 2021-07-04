#pragma once
#include "global.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


namespace psi {

class Shader {
public:
    unsigned ID;

    Shader(const std::string& vertex, const std::string& fragment);
    Shader(const std::string& vertex, const std::string& geometry, const std::string& fragment);

    void Use() { glUseProgram(ID); }

    Shader& SetInt(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        return *this;
    }
    Shader& SetFloat(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        return *this;
    }
    Shader& SetVec3f(const std::string& name, const Vec3f& value) {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2]);
        return *this;
    }
    Shader& SetVec4f(const std::string& name, const Vec4f& value) {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2], value[3]);
        return *this;
    }
    Shader& SetMat4f(const std::string& name, const Mat4f& value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value.data());
        return *this;
    }

private:
    enum class Type {
        program,
        vertex,
        fragment,
        geometry
    };

    void CheckCompileError(unsigned id, Type type);
    unsigned GenShader(const std::string& path, Type type);
};

}

#include "shader.inl"