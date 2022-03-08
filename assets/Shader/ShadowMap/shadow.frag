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
    // handle the points out of frustum
    if (cur_depth >= 1.0 || light_ndc_pos.x >= 1.0 || light_ndc_pos.y >= 1.0) {
        return 0.0;
    }

    ivec2 texSize = textureSize(map_shadow, 0);
    vec2 texel = 1.0 / texSize;

    float shadow_count = 0.0;
    for (int i = -1; i <= 1; ++i){
        for (int j = -1; j <= 1; ++j){
            float shadow_depth = texture(map_shadow, light_ndc_pos.xy + vec2(i, j) * texel).x;
            shadow_count += cur_depth - shadow_bias > shadow_depth ? 1.0 : 0.0;
        }
    }
    return shadow_count / 9.0;
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
    vec3 color = vec3(0.0);
    if (cosnl > 0.0) {
        diffuse =  vec3(max(cosnl, 0.0));
        specular = ks * pow(max(dot(norm, halfVec), 0.0), ns);
        color = (1 - calc_shadow()) * (diffuse + specular);
    }



    vec3 finalColor = (ambient + color) * objectColor * lightIntensity;
    FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), 1.0);
}