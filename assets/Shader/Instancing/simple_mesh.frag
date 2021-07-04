#version 330 core
in vec2 texcoord;

out vec4 FragColor;

uniform sampler2D map_Kd;

void main()
{
    FragColor = vec4(texture(map_Kd, texcoord).xyz, 1.0);
}