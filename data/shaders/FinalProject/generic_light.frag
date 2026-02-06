#version 300 es
precision mediump float;

in vec2 fragTexCoords;
in vec3 fragNormal;
in vec3 fragWorldPos;

uniform float opacity;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform int lightType;
uniform int enableAttenuation;

uniform vec3 lightPos;
uniform vec3 lightDir;

uniform float cutOff;
uniform float outerCutOff;

uniform float ambientStrength;
uniform float specularPow;

uniform sampler2D material_diffuse;
uniform sampler2D material_specular;

out vec4 outColor;

void main() {
    vec3 diffuseColor = vec3(texture(material_diffuse, fragTexCoords));
    vec3 specularMap = vec3(texture(material_specular, fragTexCoords));

    vec3 ambient = ambientStrength * lightColor * diffuseColor;

    vec3 norm = normalize(fragNormal);
    vec3 L;

    float attenuation = 1.0;
    float intensity = 1.0;

    if(lightType == 1)// Directional Light
    {
        L = normalize(-lightDir);
    }
    else// Point Light
    {
        L = normalize(lightPos - fragWorldPos);

        if (enableAttenuation == 1) { // Attenuation
            float distance = length(lightPos - fragWorldPos);
            // Constantes pour une portée d'environ 100 unités
            float constant = 1.0;
            float linear = 0.045;
            float quadratic = 0.0075;

            attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
        }

        if (lightType == 2) { // Spot Light
            float theta = dot(L, normalize(-lightDir));
            float epsilon = cutOff - outerCutOff;
            intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
        }
    }

    float diff = max(dot(norm, L), 0.0);
    vec3 diffuse =  diff * diffuseColor * lightColor;

    vec3 V = normalize(viewPos - fragWorldPos);
    vec3 R = reflect(-L, norm);

    float spec = pow(max(dot(V, R), 0.0), float(specularPow));

    vec3 specular = spec * specularMap * lightColor;

    vec3 lighting = (ambient + diffuse + specular) * intensity * attenuation;

    outColor = vec4(lighting * opacity, 1.0);
    //outColor = vec4(diffuseColor, 1.0f);
}

