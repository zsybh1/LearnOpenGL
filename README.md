# LearnOpenGL
环境：vs2019

第三方库：glfw，glad，glm，stb_image

文件结构：

include存放头文件

src存放头文件的实现

shaders存放着色器文件

main.cpp为入口文件

类：

Camera类用于控制摄像机的位置和方向变化

shader类用于从文件中加载着色器，并提供传输变量到着色器的功能

Object_Manager类管理所有VAO的加载及绘制