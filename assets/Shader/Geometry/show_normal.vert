#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;

out VS_OUT{
    vec3 normal;
} vs_out;

uniform mat4 model;

uniform mat4 normalTransform;

void main()
{
    gl_Position = model * vec4(aPos.xyz, 1.0);
    vs_out.normal = vec3(normalTransform * vec4(aNormal, 0.0));
}