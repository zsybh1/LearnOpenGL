#version 330 core

in vec3 objPos;

uniform vec3 lightPos;
uniform float farPlane;

void main(){
	float dis = length(objPos - lightPos);
	dis /= farPlane;
	gl_FragDepth = dis;
}