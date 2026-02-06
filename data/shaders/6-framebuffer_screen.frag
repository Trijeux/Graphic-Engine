#version 300 es
precision mediump float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int postProces;// 0 = normal / 1 = gris / 2 = inverstion / 3 = Sharpen / 4 = blur / 5 = edge detection

void main()
{
    if (postProces == 0){
        FragColor = texture(screenTexture, TexCoords);
    }
    else if (postProces == 1) {
        FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    }
    else if (postProces == 2) {
        FragColor = texture(screenTexture, TexCoords);
        float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(average, average, average, 1.0);
    }
    else {
        const float offset = 1.0 / 300.0;// Adjust based on resolution
        vec2 offsets[9] = vec2[](
        vec2(-offset, offset), // top-left
        vec2(0.0f, offset), // top-center
        vec2(offset, offset), // top-right
        vec2(-offset, 0.0f), // center-left
        vec2(0.0f, 0.0f), // center-center
        vec2(offset, 0.0f), // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0f, -offset), // bottom-center
        vec2(offset, -offset)// bottom-right
        );

        float kernel[9];

        if (postProces == 3){
            kernel = float[](
            -1.0, -1.0, -1.0,
            -1.0, 9.0, -1.0,
            -1.0, -1.0, -1.0
            );
        }
        else if (postProces == 4) {
            float v = 1.0 / 16.0;
            kernel = float[](
            1.0*v, 2.0*v, 1.0*v,
            2.0*v, 4.0*v, 2.0*v,
            1.0*v, 2.0*v, 1.0*v);
        }
        else if(postProces == 5) {
            kernel = float[](
            1.0, 1.0, 1.0,
            1.0, -8.0, 1.0,
            1.0, 1.0, 1.0
            );
        }


        vec3 sampleTex[9];
        for (int i = 0; i < 9; i++) {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
        FragColor = vec4(col, 1.0);
    }
}
