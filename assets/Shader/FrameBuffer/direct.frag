#version 330 core

in vec2 texcoord;

uniform sampler2D tex;

out vec4 FragColor;

void main(){
	FragColor = vec4(texture(tex, texcoord).xyz, 1.0);
}