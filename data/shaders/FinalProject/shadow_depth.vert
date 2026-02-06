#version 300 es
layout (location = 0) in vec3 aPos;
// Instancing Matrix (takes slots 5, 6, 7, 8)
layout (location = 5) in mat4 aInstanceMatrix;

uniform mat4 lightSpaceMatrix;

void main()
{
    // Combine light projection with instance model matrix
    gl_Position = lightSpaceMatrix * aInstanceMatrix * vec4(aPos, 1.0);
}