#version 330 core

in vec2 texcoord;

uniform sampler2D tex;

out vec4 FragColor;

void main(){
	vec3 texcolor = vec3(texture(tex, texcoord));
	float greyscale = texcolor.r * 0.2126 + texcolor.g * 0.7152 + texcolor.b * 0.0722;
	FragColor = vec4(vec3(greyscale), 1.0);
}