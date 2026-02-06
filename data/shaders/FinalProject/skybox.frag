#version 300 es
precision mediump float;

in vec3 fragTexCoords;

uniform samplerCube skybox;

// --- MRT OUTPUTS ---
layout (location = 0) out vec4 FragColor;   // Normal Scene
layout (location = 1) out vec4 BrightColor; // Bloom Color

void main() {
    vec4 color = texture(skybox, fragTexCoords);

    // 1. Output the sky color to the main screen
    FragColor = color;

    // 2. FORCE BLACK for the Bloom buffer
    // This ensures the skybox NEVER glows, no matter how bright it is.
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}