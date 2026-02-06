#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord) * 0.8;
    if(TexCoord == vec2(0)){
        FragColor = vec4(1.0);
    }
}
