#version 300 es
precision mediump float;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in vec3 inTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragWorldPos;
out vec2 fragTexCoords;
out vec3 Normal;
out mat3 TBN;

void main() {
    vec4 worldPos = model * vec4(inPos, 1.0);
    fragWorldPos = worldPos.xyz;
    fragTexCoords = inTexCoords;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal = normalize(normalMatrix * inNormal);

    vec3 T = normalize(normalMatrix * inTangent);
    vec3 N = normalize(normalMatrix * inNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    gl_Position = projection * view * worldPos;
}