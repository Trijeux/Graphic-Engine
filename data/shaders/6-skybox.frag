#version 300 es
precision mediump float;

in vec3 fragTexCoords;

uniform samplerCube skybox;

out vec4 outColor;

void main(){
    outColor = texture(skybox, fragTexCoords);
}