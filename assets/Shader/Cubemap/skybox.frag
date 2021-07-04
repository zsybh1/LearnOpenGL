#version 330 core

in vec3 texcoord;

uniform samplerCube skybox;

out vec4 FragColor;

void main(){
	FragColor = vec4(pow(vec3(texture(skybox, texcoord)), vec3(0.45)), 1.0);
}