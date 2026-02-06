#version 300 es
precision highp float;

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

out vec4 FragColor; // We write to the GL_R32F texture

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);

    // Normalize distance to [0, 1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;

    // Write this as color
    FragColor = vec4(lightDistance, 0.0, 0.0, 1.0);
}