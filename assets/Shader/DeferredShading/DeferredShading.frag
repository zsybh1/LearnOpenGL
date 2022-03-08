#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in vec3 fragPos;
in vec3 normal;
in vec2 texcoord;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;

void main(){
	gNormal = normalize(normal);
	gPosition = fragPos;
	gColorSpec.rgb = texture(diffuseMap, texcoord).rgb;
	gColorSpec.a = texture(specularMap, texcoord).r;
}