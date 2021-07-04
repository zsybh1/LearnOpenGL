#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in mat4 model;

out vec2 texcoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
    texcoord = aTexcoord;
}