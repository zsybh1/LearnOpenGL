#version 330 core
in vec3 normal;
in vec3 fragPos;
in vec2 texcoord;

out vec4 FragColor;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
uniform float Ni;
uniform float d;
uniform int illum;

uniform float uKa = 1.0;

uniform bool UseKaMap = false;
uniform bool UseKdMap = false;
uniform bool UseKsMap = false;
uniform bool UseNsMap = false;
uniform bool UseDMap = false;
uniform bool UseBumpMap = false;

uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform sampler2D map_Ks;
uniform sampler2D map_Ns;
uniform sampler2D map_d;
uniform sampler2D map_bump;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    vec3 ka = UseKaMap? texture(map_Ka, texcoord).xyz : Ka;
    vec3 kd = UseKdMap? texture(map_Kd, texcoord).xyz : Kd;
    vec3 ks = UseKsMap? texture(map_Ks, texcoord).xyz : Ks;
    float ns = UseNsMap? texture(map_Ns, texcoord).x : Ns;

    vec3 lightDir = lightPos - fragPos;
    vec3 viewDir = normalize(viewPos - fragPos);
    float lightDist = length(lightDir);
    lightDir /= lightDist;
    vec3 lightIntensity = pow(lightColor, vec3(2.2)) / lightDist / lightDist;
    vec3 norm = normalize(normal);
    vec3 halfVec = normalize(lightDir + viewDir);
    vec3 objectColor = pow(kd, vec3(2.2));

    float cosnl = dot(lightDir, norm);

    vec3 ambient = uKa * ka * objectColor;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if (cosnl > 0.0) {
        diffuse =  objectColor * lightIntensity * max(cosnl, 0.0);
        specular = ks * lightIntensity * pow(max(dot(norm, halfVec), 0.0), ns);
    }
    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0);
}