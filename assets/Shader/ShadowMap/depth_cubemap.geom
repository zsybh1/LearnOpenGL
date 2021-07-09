#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec3 objPos;

uniform mat4 views[6];
uniform mat4 projection;

void main() {
	for (int i = 0; i < 6; ++i) {
		gl_Layer = i;
		for (int j = 0; j < 3; ++j){
			objPos = vec3(gl_in[j].gl_Position);
			gl_Position = projection * views[i] * gl_in[j].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}