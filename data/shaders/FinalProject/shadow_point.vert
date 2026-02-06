#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 aInstanceMatrix; // Instancing support

uniform mat4 shadowMatrices; // View * Projection for the specific face
uniform mat4 model;          // Fallback if not using instancing (optional)

out vec4 FragPos; // Pass world position to fragment

void main()
{
    // If using instancing, use aInstanceMatrix, else use uniform model
    // Assuming your engine always uses instancing based on scene.cc:
    vec4 worldPos = aInstanceMatrix * vec4(aPos, 1.0);
    FragPos = worldPos;
    gl_Position = shadowMatrices * worldPos;
}