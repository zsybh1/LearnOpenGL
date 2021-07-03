#version 330 core

in vec2 texcoord;

uniform sampler2D tex;

out vec4 FragColor;

uniform float granularity;

void main(){
	float offset = 1.0/granularity;
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset), vec2(0.0, offset), vec2(offset, offset),
		vec2(-offset, 0.0), vec2(0.0, 0.0), vec2(offset, 0.0),
		vec2(-offset, -offset), vec2(0.0, -offset), vec2(offset, -offset)
	);

	vec3 result = vec3(0.0);
	for (int i = 0; i < 9; ++i){
		result += vec3(texture(tex, texcoord + offsets[i])) / 9.0;
	}
	FragColor = vec4(result, 1.0);
}