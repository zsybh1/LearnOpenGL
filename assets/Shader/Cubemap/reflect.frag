#version 330 core
in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main(){
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-viewDir, normalize(normal));
	FragColor = vec4(pow(texture(skybox, reflectDir).xyz, vec3(0.45)), 1.0);
}