namespace psi {

inline Shader::Shader(const std::string& vertex, const std::string& fragment) {
    unsigned vertexShader = GenShader(vertex, Type::vertex);
    unsigned fragmentShader = GenShader(fragment, Type::fragment);

    // link shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    CheckCompileError(shaderProgram, Type::program);

    // delete object after link
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    ID = shaderProgram;
}

inline Shader::Shader(const std::string& vertex, const std::string& geometry, const std::string& fragment) {
    unsigned vertexShader = GenShader(vertex, Type::vertex);
    unsigned fragmentShader = GenShader(fragment, Type::fragment);
    unsigned geometryShader = GenShader(geometry, Type::geometry);

    // link shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geometryShader);
    glLinkProgram(shaderProgram);

    CheckCompileError(shaderProgram, Type::program);

    // delete object after link
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);

    ID = shaderProgram;
}

inline void Shader::CheckCompileError(unsigned id, Type type) {
    int  success;
    char infoLog[512];
    if (type == Type::program) {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }
    else {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            if (type == Type::vertex) {
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            else if (type == Type::fragment) {
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            else if (type == Type::geometry) {
                std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
        }
    }
}

unsigned Shader::GenShader(const std::string& path, Type type) {
    int target = 0;
    std::string name;
    switch (type)
    {
    case psi::Shader::Type::vertex:
        target = GL_VERTEX_SHADER;
        name = "VERTEX";
        break;
    case psi::Shader::Type::fragment:
        target = GL_FRAGMENT_SHADER;
        name = "FRAGMENT";
        break;
    case psi::Shader::Type::geometry:
        target = GL_GEOMETRY_SHADER;
        name = "GEOMETRY";
        break;
    default:
        std::cout << "ERROR::SHADER::UNKNOWN_ERROR\n" << std::endl;
        break;
    }

    std::ifstream shaderFile(path);
    if (!shaderFile.is_open()) {
        std::cout << "ERROR::SHADER::" << name << "::NOT_EXIST" << std::endl;
    }
    std::string shaderStr;
    std::stringstream shaderss;
    shaderss << shaderFile.rdbuf();
    shaderStr = shaderss.str();

    unsigned int shader;
    shader = glCreateShader(target);
    const char* cStr = shaderStr.c_str();
    glShaderSource(shader, 1, &cStr, NULL);
    glCompileShader(shader);
    CheckCompileError(shader, type);

    return shader;
}

}