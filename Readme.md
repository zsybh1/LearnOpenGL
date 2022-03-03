# Readme

### 依赖

- glfw
- glad
- imgui (glfw_impl && opengl3_impl)
- stb_image
- eigen3
- OBJ_Loader (included)



### 操作方式

shift下降，空格键上升，wasd移动，鼠标移动转动视角

按Esc可以退出鼠标操控视角模式，可以调整各项参数。再按下Esc返回鼠标操控模式



### 各示例简介

Blending/transparent.cpp ：实现半透明效果以及模型的读取

FrameBuffer/post_processing.cpp ：在FrameBuffer中进行后处理达成的灰阶效果

CubeMap/skybox.cpp ：天空盒，以及基于天空盒的折射（方块）和反射（地板）效果。参数ior调整折射率

Geometry/show_normal.cpp ：使用Geometry Shader完成的显示模型法线的效果。参数normal_len调整法线长度

Instancing/asteroid_field.cpp ：使用实例化技术达到渲染大量相同物体（30万小行星）而不明显卡顿的效果

![image-20220303125350705](https://raw.githubusercontent.com/zsybh1/ImageBed/master/img/20220303125402.png)

ShadowMap/origin.cpp ：（完成阴影效果前的代码备份）

ShadowMap/shadow_map ：使用正面剔除的方式完成的阴影效果，并使用PCF对阴影边缘进行了柔化

ShadowMap/cube_shadow_map ：在上面的基础上使用cubemap作为深度贴图，使得光源靠近方块时阴影不会失真

![image-20220303125517545](https://raw.githubusercontent.com/zsybh1/ImageBed/master/img/20220303125538.png)

Mapping/normal_mapping ：法线贴图

Mapping/parallax_mapping ：视差贴图，从右往左分别为普通的视差贴图，陡峭视差贴图，视差遮挡。参数depth_scale调整视差的深度，num_layers调整陡峭视差贴图和视差遮挡中的层级数量

![image-20220303125828681](https://raw.githubusercontent.com/zsybh1/ImageBed/master/img/20220303125831.png)

