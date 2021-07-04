#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D Texture;

void main()
{
    vec4 texColor = texture(Texture, TexCoord);
    FragColor = vec4(pow(texColor.xyz, vec3(0.45)), texColor.a);
    if (texColor.a < 0.1) {
        discard;
    }
}