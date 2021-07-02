#version 330 core
in vec3 normal;
in vec3 fragPos;
in vec2 texcoord;

out vec4 FragColor;

uniform vec3 ka;   // ambient coef
uniform float shininess;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D kd;
uniform sampler2D ks;

void main()
{
    vec3 lightDir = lightPos - fragPos;
    vec3 viewDir = normalize(viewPos - fragPos);
    float lightDist = length(lightDir);
    lightDir /= lightDist;
    vec3 lightIntensity = pow(lightColor, vec3(2.2)) / lightDist / lightDist;
    vec3 norm = normalize(normal);
    vec3 halfVec = normalize(lightDir + viewDir);
    vec3 objectColor = texture(kd, texcoord).xyz;

    float cosnl = dot(lightDir, norm);

    vec3 ambient = ka * objectColor;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if (cosnl > 0.0) {
        diffuse =  objectColor * lightIntensity * max(cosnl, 0.0);
        specular = texture(ks, texcoord).xyz * lightIntensity * pow(max(dot(norm, halfVec), 0.0), shininess);
    }
    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0);
}