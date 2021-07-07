#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;

out vec3 fragPos;
out vec3 normal;
out vec2 texcoord;

out vec4 light_space_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 normalTransform;

uniform mat4 lightMatrix;

void main()
{
    normal = vec3(normalTransform * vec4(aNormal, 0.0));
    fragPos = vec3(model * vec4(aPos, 1.0));
    texcoord = aTexcoord;
    gl_Position = projection * view * vec4(fragPos, 1.0);
    light_space_pos = lightMatrix * vec4(fragPos, 1.0);
}