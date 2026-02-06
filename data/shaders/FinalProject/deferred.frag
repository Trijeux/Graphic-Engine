#version 300 es
precision highp float;

layout (location = 0) out vec4 FragColor;   // Output 0: Scene Color
layout (location = 1) out vec4 BrightColor; // Output 1: Brightness (For Bloom)

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// --- Shadow Map Inputs ---
uniform highp sampler2DShadow shadowMap;
uniform mat4 lightSpaceMatrix;

uniform sampler2D ssao;
uniform bool useSSAO;

uniform vec3 viewPos;
uniform float specularPow;
uniform int debugMode; // 0=None, 1=Pos, 2=Norm, 3=Alb, 4=Spec


#define MAX_POINT_SHADOWS 4
uniform highp samplerCube pointShadowMaps[MAX_POINT_SHADOWS];
uniform float pointFarPlane;


// --- Light Structures ---
struct DirLight {
    vec3 direction;
    vec3 color;
    float ambientStrength;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

// --- Uniform Arrays ---
#define MAX_POINT_LIGHTS 50
#define MAX_SPOT_LIGHTS 50

uniform bool hasDirLight;
uniform DirLight dirLight;

uniform bool hasFlashLight;
uniform SpotLight flashLight;

uniform int pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform int spotLightCount;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

// --- Function Prototypes ---
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float specular, vec3 fragPos);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specular, int index);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specular);
float ShadowCalculation(vec3 fragPosWorld, vec3 normal, vec3 lightDir);
float PointShadowCalculation(highp samplerCube shadowMap, vec3 fragPos, vec3 fragPos);

// Workaround function to select the correct cube map based on index
float CalcPointShadowSelect(int index, vec3 fragPos, vec3 lightPos) {
    if (index == 0) return PointShadowCalculation(pointShadowMaps[0], fragPos, lightPos);
    if (index == 1) return PointShadowCalculation(pointShadowMaps[1], fragPos, lightPos);
    if (index == 2) return PointShadowCalculation(pointShadowMaps[2], fragPos, lightPos);
    if (index == 3) return PointShadowCalculation(pointShadowMaps[3], fragPos, lightPos);
    return 0.0; // No shadow if index >= 4
}

void main() {
    // 1. Retrieve data from G-Buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;

    vec3 encodedNormal = texture(gNormal, TexCoords).rgb;
    vec3 Normal = normalize(encodedNormal * 2.0 - 1.0);

    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // --- DEBUG MODES ---
    if (debugMode == 1) { FragColor = vec4(abs(FragPos) / 20.0, 1.0); BrightColor = vec4(0.0); return; }
    if (debugMode == 2) { FragColor = vec4(encodedNormal, 1.0); BrightColor = vec4(0.0); return; }
    if (debugMode == 3) { FragColor = vec4(Albedo, 1.0); BrightColor = vec4(0.0); return; }
    if (debugMode == 4) { FragColor = vec4(vec3(Specular), 1.0); BrightColor = vec4(0.0); return; }

    // --- LIGHTING CALCULATION ---
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    // A. Directional Light (Sun) + SHADOWS
    if(hasDirLight) {
        // We pass FragPos now to calculate shadows
        result += CalcDirLight(dirLight, Normal, viewDir, Albedo, Specular, FragPos);
    }

    // B. Point Lights
    for(int i = 0; i < pointLightCount; i++) {
        result += CalcPointLight(pointLights[i], Normal, FragPos, viewDir, Albedo, Specular, i);
    }

    // C. Spot Lights
    for(int i = 0; i < spotLightCount; i++) {
        result += CalcSpotLight(spotLights[i], Normal, FragPos, viewDir, Albedo, Specular);
    }

    // D. Flashlight
    if(hasFlashLight) {
        result += CalcSpotLight(flashLight, Normal, FragPos, viewDir, Albedo, Specular);
    }

    // 1. Output Final Scene Color
    FragColor = vec4(result, 1.0);

    // 2. Output Brightness (BLOOM LOGIC)
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.9)
    BrightColor = vec4(result, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

// --- Shadow Calculation Function ---
float ShadowCalculation(vec3 fragPosWorld, vec3 normal, vec3 lightDir)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPosWorld, 1.0);

    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;

    // Bias
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.00005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));

    // Use a tighter 3x3 loop (Radius 1) because hardware filtering handles the micro-blur
    // increasing performance and quality simultaneously.
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            // We subtract bias from the Z comparison value directly
            float pcfDepth = projCoords.z - bias;

            // The hardware does the comparison and blending for us!
            shadow += texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, pcfDepth));
        }
    }
    shadow /= 9.0;

    // The texture returns 1.0 for "Lit" and 0.0 for "Shadow"
    // So we invert it to match your logic (1.0 = In Shadow)
    return 1.0 - shadow;
}

// Point Shadow (Perspective, Cube Map)
float PointShadowCalculation(highp samplerCube shadowMap, vec3 fragPos, vec3 lightPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15; // Higher bias needed for perspective shadows

    // Sample the linear depth we wrote in shadow_point.frag
    float closestDepth = texture(shadowMap, fragToLight).r;
    closestDepth *= pointFarPlane; // Remap [0,1] back to [0, far]

    if(currentDepth - bias > closestDepth)
    shadow = 1.0;

    return shadow;
}

// ------------------------------------------------------------------
//                      Lighting Calculations
// ------------------------------------------------------------------

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float specular, vec3 fragPos) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), specularPow);

    // --- SHADOW CALCULATION ---
    // 0.0 = Shadow, 1.0 = Lit (Or partial for PCF)
    float shadow = ShadowCalculation(fragPos, normal, lightDir);

    // Ambient Occlusion
    float ao = useSSAO ? texture(ssao, TexCoords).r : 1.0;
    ao *= ao;

    // Combine
    vec3 ambient = light.ambientStrength * light.color * albedo * ao;

    // Apply (1.0 - shadow) to Diffuse and Specular only
    vec3 diffuse = (1.0 - shadow) * (light.color * diff * albedo);
    vec3 specColor = (1.0 - shadow) * (light.color * spec * specular);

    return (ambient + diffuse + specColor);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specular, int index) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), specularPow);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Ambient Occlusion
    float ao = useSSAO ? texture(ssao, TexCoords).r : 1.0;
    ao *= ao;

    // --- POINT SHADOW LOGIC ---
    float shadow = 0.0;
    if(index < MAX_POINT_SHADOWS) {
        shadow = CalcPointShadowSelect(index, fragPos, light.position);
    }

    vec3 diffuse = (1.0 - shadow) * (light.color * diff * albedo);
    vec3 specColor = (1.0 - shadow) * (light.color * spec * specular);
    vec3 ambient = 0.05 * light.color * albedo * ao;

    return (ambient + diffuse + specColor) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specular) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), specularPow);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spot Intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Ambient Occlusion
    float ao = useSSAO ? texture(ssao, TexCoords).r : 1.0;
    ao *= ao;

    // Combine
    vec3 ambient = 0.0 * light.color * albedo * ao;
    vec3 diffuse = light.color * diff * albedo;
    vec3 specColor = light.color * spec * specular;

    return (ambient + diffuse + specColor) * attenuation * intensity;
}