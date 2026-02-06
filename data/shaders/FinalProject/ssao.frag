#version 300 es
precision highp float;
out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;
uniform mat4 view;
uniform float radius;
uniform float bias;

void main() {
    // Convert World Pos to View Pos
    vec3 fragPosWorld = texture(gPosition, TexCoords).xyz;
    vec3 fragPos = (view * vec4(fragPosWorld, 1.0)).xyz;

    // Convert World Normal to View Normal
    vec3 normalWorld = normalize(texture(gNormal, TexCoords).rgb * 2.0 - 1.0);
    vec3 normal = mat3(view) * normalWorld;

    vec2 noiseScale = vec2(textureSize(gPosition, 0)) / 4.0;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for(int i = 0; i < 64; ++i) {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        vec3 samplePosWorld = texture(gPosition, offset.xy).xyz;
        float sampleDepth = (view * vec4(samplePosWorld, 1.0)).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    FragColor = 1.0 - (occlusion / 64.0);
}