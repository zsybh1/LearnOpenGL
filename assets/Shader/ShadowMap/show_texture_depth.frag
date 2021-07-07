#version 330 core

in vec2 texcoord;

uniform sampler2D tex;

out vec4 FragColor;

void main(){
	FragColor = vec4(vec3(texture(tex, texcoord).x), 1.0);
}