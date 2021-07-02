namespace psi {

inline Shader::Shader(const std::string& vertex, const std::string& fragment) {
    std::ifstream vertFile(vertex);
    std::ifstream fragFile(fragment);
    if (!vertFile.is_open()) {
        std::cout << "ERROR::SHADER::VERTEX::NOT_EXIST" << std::endl;
    }
    if (!fragFile.is_open()) {
        std::cout << "ERROR::SHADER::FRAGMENT::NOT_EXIST" << std::endl;
    }
    std::string vertStr;
    std::string fragStr;
    std::stringstream vertss;
    std::stringstream fragss;
    vertss << vertFile.rdbuf();
    fragss << fragFile.rdbuf();
    vertStr = vertss.str();
    fragStr = fragss.str();

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* cVertStr = vertStr.c_str();
    glShaderSource(vertexShader, 1, &cVertStr, NULL);
    glCompileShader(vertexShader);
    CheckCompileError(vertexShader, Type::vertex);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* cFragStr = fragStr.c_str();
    glShaderSource(fragmentShader, 1, &cFragStr, NULL);
    glCompileShader(fragmentShader);
    CheckCompileError(fragmentShader, Type::fragment);

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
        }
    }
}

}