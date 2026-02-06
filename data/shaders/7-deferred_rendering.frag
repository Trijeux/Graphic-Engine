#version 300 es
precision mediump float;
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;
uniform float specularPow;

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

uniform int pointLightCount;
uniform PointLight pointLights[1];

void main() {
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec4 albedoSpec = texture(gAlbedoSpec, TexCoords);
    vec3 albedo = albedoSpec.rgb;
    float specFactor = albedoSpec.a;

    if (texture(gPosition, TexCoords).a < 0.1) discard;

    vec3 lighting = albedo * 0.1; // Ambient
    vec3 viewDir = normalize(viewPos - fragPos);

    for(int i = 0; i < pointLightCount; i++) {
        vec3 lightDir = normalize(pointLights[i].position - fragPos);
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * pointLights[i].color;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), specularPow);
        vec3 specular = pointLights[i].color * spec * specFactor;

        float d = length(pointLights[i].position - fragPos);
        float atten = 1.0 / (pointLights[i].constant + pointLights[i].linear * d + pointLights[i].quadratic * (d * d));
        lighting += (diffuse + specular) * atten;
    }

    FragColor = vec4(lighting, 1.0);
}