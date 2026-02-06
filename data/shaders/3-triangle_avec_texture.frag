#version 300 es
precision mediump float;

in vec2 fragUv;
in vec3 fragColor;

uniform sampler2D texUv;
uniform float colorCoeff;

out vec4 outColor;

void main() {
    vec3 texColor = texture(texUv, fragUv).rgb;

    // Mix texture with vertex color (or use texture only)
    //vec3 finalColor = texColor * fragColor;      // multiply both
    vec3 finalColor = texColor;                // OR use texture only

    outColor = vec4(finalColor * colorCoeff, 1.0f);
}