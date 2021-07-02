#version 330 core
in vec3 normal;
in vec3 fragPos;
in vec2 texcoord;

out vec4 FragColor;

uniform sampler2D kd;
uniform sampler2D ks;
uniform vec3 ka;   // ambient coef
uniform float shininess;

struct PointLight {
    vec3 pos;
    vec3 color;
};
uniform PointLight pointLight;

struct SkyLight {
    vec3 dir;  // ÊäÈë¹âÓ¦µ±ÊÇÏòÏÂµÄ
    vec3 color;
};
uniform SkyLight skyLight;

struct SpotLight {
    vec3 pos;
    vec3 dir;
    vec3 color;
    float innerCos;
    float outerCos;
};
uniform SpotLight spotLight;
uniform bool enableSpotLight;

uniform vec3 viewPos;

vec3 CalcBlinnPhong(vec3 lightDir, vec3 norm, vec3 viewDir, vec3 lightIntensity, vec3 objectColor);
vec3 CalcPointLight(vec3 objectColor, vec3 viewDir, vec3 norm);
vec3 CalcSkyLight(vec3 objectColor, vec3 viewDir, vec3 norm);
vec3 CalcSpotLight(vec3 objectColor, vec3 viewDir, vec3 norm);

void main()
{
    vec3 objectColor = texture(kd, texcoord).xyz;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 norm = normalize(normal);

    vec3 point = CalcPointLight(objectColor, viewDir, norm);
    vec3 sky = CalcSkyLight(objectColor, viewDir, norm);
    vec3 finalColor = point + sky;
    if (enableSpotLight) {
        finalColor += CalcSpotLight(objectColor, viewDir, norm);
    }
    FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0);
}

// Ëæ¾àÀëË¥¼õ
vec3 CalcPointLight(vec3 objectColor, vec3 viewDir, vec3 norm) {
    vec3 lightDir = pointLight.pos - fragPos;
    float lightDist = length(lightDir);
    lightDir /= lightDist;
    vec3 lightIntensity = pow(pointLight.color, vec3(2.2)) / lightDist / lightDist;
    vec3 ambient = ka * lightIntensity * objectColor;
    return ambient + CalcBlinnPhong(lightDir, norm, viewDir, lightIntensity, objectColor);
}

// ²»Ëæ¾àÀëË¥¼õ
vec3 CalcSkyLight(vec3 objectColor, vec3 viewDir, vec3 norm) {
    vec3 lightDir = -normalize(skyLight.dir);
    vec3 lightIntensity = pow(skyLight.color, vec3(2.2));
    vec3 ambient = ka * lightIntensity * objectColor;
    return ambient + CalcBlinnPhong(lightDir, norm, viewDir, lightIntensity, objectColor);
}

// ¾Û¹â£¬Ëæ¾àÀëË¥¼õ
vec3 CalcSpotLight(vec3 objectColor, vec3 viewDir, vec3 norm) {
    vec3 lightDir = spotLight.pos - fragPos;
    float lightDist = length(lightDir);
    lightDir /= lightDist;
    vec3 lightIntensity = pow(spotLight.color, vec3(2.2)) / lightDist / lightDist;
    float dirCos = dot(lightDir, -normalize(spotLight.dir));
    dirCos = (dirCos - spotLight.outerCos) / (spotLight.innerCos - spotLight.outerCos);
    float coef = clamp(dirCos, 0.0, 1.0);
    vec3 ambient = ka * lightIntensity * objectColor;
    return ambient + coef * CalcBlinnPhong(lightDir, norm, viewDir, lightIntensity, objectColor);
}

vec3 CalcBlinnPhong(vec3 lightDir, vec3 norm, vec3 viewDir, vec3 lightIntensity, vec3 objectColor) {
    vec3 halfVec = normalize(lightDir + viewDir);

    float cosnl = dot(lightDir, norm);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if (cosnl > 0.0) {
        diffuse =  objectColor * lightIntensity * max(cosnl, 0.0);
        specular = texture(ks, texcoord).xyz * lightIntensity * pow(max(dot(norm, halfVec), 0.0), shininess);
    }

    return diffuse + specular;
}