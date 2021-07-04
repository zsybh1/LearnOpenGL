#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT{
    vec3 normal;
} gs_in[];

uniform mat4 view;
uniform mat4 projection;

uniform float len;

void main() {
    for (int i = 0; i < 3; ++i){
        gl_Position = projection * view * gl_in[i].gl_Position;
        EmitVertex();
        gl_Position = projection * view * (gl_in[i].gl_Position + vec4(gs_in[i].normal * len, 0.0));
        EmitVertex();
        EndPrimitive();
    }
}
