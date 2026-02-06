#version 300 es
precision mediump float;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fragTexCoords;
out vec3 fragNormal;
out vec3 fragWorldPos;

void main() {
    vec4 worldPos = model * vec4(inPos, 1.0);

    gl_Position = projection * view * worldPos;

    fragTexCoords = inTexCoords;

    fragNormal = mat3(transpose(inverse(model))) * inNormal;

    fragWorldPos = worldPos.xyz;
}