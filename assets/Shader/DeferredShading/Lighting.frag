#version 330 core

in vec2 texcoord;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

uniform vec3 viewPos;

uniform vec3 ka;	// ambient coef
uniform float shininess;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;

void main(){
	vec3 normal = texture(gNormal, texcoord).xyz;
	vec3 fragPos = texture(gPosition, texcoord).xyz;
	vec3 color = pow((texture(gColorSpec, texcoord).xyz), vec3(2.2));
	float ks = texture(gColorSpec, texcoord).a;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 lightDir = normalize(lightPos - fragPos);
	
	float lightDistance = length(lightPos - fragPos);
	vec3 light = pow(lightColor, vec3(2.2)) * lightIntensity / lightDistance / lightDistance;



	vec3 ambient = ka * color;
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	float cosnl = dot(normal, lightDir);
	if (cosnl > 0) {
		diffuse = color * light * cosnl;
		vec3 halfVec = normalize(lightDir + viewDir);
		specular = ks * light * pow(max(dot(normal, halfVec), 0.0), shininess);
	}
	vec3 finalColor = ambient + diffuse + specular;
	FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0);
}