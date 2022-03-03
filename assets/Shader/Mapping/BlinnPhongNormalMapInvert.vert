#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;
layout (location = 3) in vec3 aTangent;

out vec3 fragPos;
out vec2 texcoord;
out vec3 tangentLightPos;
out vec3 tangentViewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform mat4 normalTransform;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
    vec3 normal = normalize(vec3(normalTransform * vec4(aNormal, 0.0)));
    fragPos = vec3(model * vec4(aPos, 1.0));
    texcoord = aTexcoord;

    vec3 tangent = vec3(normalTransform * vec4(aTangent, 0.0));
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(normal, tangent);

    mat3 TBN = transpose(mat3(tangent, bitangent, normal));
    fragPos = TBN * fragPos;
    tangentLightPos = TBN * lightPos;
    tangentViewPos = TBN * viewPos;
}