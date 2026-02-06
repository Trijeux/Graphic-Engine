#version 300 es
precision mediump float;

in vec3 fragColor;
out vec4 outColor;

uniform float colorCoeff;

void main() {
    outColor = vec4(fragColor * colorCoeff, 1.0);
}