#version 330 core
in vec3 fragPos;
in vec2 texcoord;
in vec3 tangentLightPos;
in vec3 tangentViewPos;

out vec4 FragColor;

uniform vec3 ka;   // ambient coef
uniform vec3 ks;   // specular coef
uniform float shininess;
uniform vec3 lightColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main()
{
    vec3 kd = texture(diffuseMap, texcoord).xyz;
    vec3 normal = texture(normalMap, texcoord).xyz;
    normal = normal * 2.0 - 1.0;

    vec3 lightDir = tangentLightPos - fragPos;
    vec3 viewDir = normalize(tangentViewPos - fragPos);
    float lightDist = length(lightDir);
    lightDir /= lightDist;
    vec3 lightIntensity = pow(lightColor, vec3(2.2)) / lightDist / lightDist;
    vec3 norm = normalize(normal);
    vec3 halfVec = normalize(lightDir + viewDir);
    vec3 objectColor = pow(kd, vec3(2.2));

    float cosnl = dot(lightDir, norm);

    vec3 ambient = ka * objectColor;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if (lightDir.z > 0.0) {
        diffuse =  objectColor * lightIntensity * max(cosnl, 0.0);
        specular = ks * lightIntensity * pow(max(dot(norm, halfVec), 0.0), shininess);
    }
    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0);
}