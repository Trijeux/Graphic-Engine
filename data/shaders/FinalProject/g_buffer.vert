#version 300 es
precision highp float;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

// A mat4 takes up 4 attribute slots (Locations 5, 6, 7, 8)
layout (location = 5) in mat4 aInstanceMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec2 fragTexCoords;
out vec3 fragWorldPos;
out vec3 Normal;
out mat3 TBN;

void main() {
    // We use the attribute directly
    mat4 model = aInstanceMatrix;

    vec4 worldPos = model * vec4(inPos, 1.0);
    gl_Position = projection * view * worldPos;

    fragTexCoords = inTexCoords;
    fragWorldPos = worldPos.xyz;

    // --- Calculate TBN Matrix ---
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal = normalize(normalMatrix * inNormal);

    vec3 T = normalize(normalMatrix * inTangent);
    vec3 N = normalize(normalMatrix * inNormal);

    // Re-orthogonalize T with respect to N (Gram-Schmidt)
    T = normalize(T - dot(T, N) * N);

    // Calculate Bitangent
    vec3 B = normalize(normalMatrix * inBitangent);

    TBN = mat3(T, B, N);
}