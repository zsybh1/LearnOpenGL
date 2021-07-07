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

uniform sampler2D map_shadow;
uniform float shadow_bias;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

in vec4 light_space_pos;

float calc_shadow() {
    vec3 light_ndc_pos = (light_space_pos.xyz / light_space_pos.w) * 0.5 + 0.5;
    float cur_depth = light_ndc_pos.z;
    if (cur_depth >= 1.0 || light_ndc_pos.x >= 1.0 || light_ndc_pos.y >= 1.0) {
        return 0.0;
    }
    float shadow_depth = texture(map_shadow, light_ndc_pos.xy).x;
    return cur_depth - shadow_bias > shadow_depth ? 1.0 : 0.0;
}

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

    vec3 ambient = uKa * ka;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if (cosnl > 0.0) {
        diffuse =  vec3(max(cosnl, 0.0));
        specular = ks * pow(max(dot(norm, halfVec), 0.0), ns);
    }
    vec3 finalColor = (ambient + (1 - calc_shadow()) * (diffuse + specular)) * objectColor * lightIntensity;
    FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0);
}