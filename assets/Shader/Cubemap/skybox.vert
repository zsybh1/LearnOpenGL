#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 texcoord;

void main(){
	gl_Position = (projection * mat4(mat3(view)) * vec4(aPos, 1.0));
	texcoord = aPos;
}