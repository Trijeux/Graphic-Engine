#version 300 es
precision mediump float;

uniform vec3 objectColor;

// --- MRT OUTPUTS ---
layout (location = 0) out vec4 FragColor;   // Normal Scene
layout (location = 1) out vec4 BrightColor; // Bloom Color

void main() {
    FragColor = vec4(objectColor, 1.0);

    // Calculate brightness
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    // CHECK: Standard White (1.0, 1.0, 1.0) has a brightness of exactly 1.0.
    // If your threshold is > 1.0, the lights won't bloom!
    // We use > 0.9 here to ensure white light gizmos ALWAYS bloom.
    if(brightness > 0.9)
    BrightColor = vec4(objectColor * 100.0, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}