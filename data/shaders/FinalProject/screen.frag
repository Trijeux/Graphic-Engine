#version 300 es
precision mediump float;

out vec4 FragColor;
in vec2 TexCoords;

//uniform sampler2D screenTexture;
uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform sampler2D debugRawBrightness;

uniform bool bloom;
uniform float exposure;

uniform int mode; // 0=None, 1=Inverse, 2=Grayscale, 3=Sharpen, 4=Blur, 5=Edge

void main() {
    vec3 color = vec3(0.0);

    if (mode == 6) {
        vec3 raw = texture(debugRawBrightness, TexCoords).rgb;
        FragColor = vec4(raw, 1.0);
        return;
    }
    // =======================================================
    // PATH A: Convolution Kernels (Sharpen, Blur, Edge)
    // =======================================================
    if (mode >= 3 && mode <= 5) {
        const float offset = 1.0 / 300.0;
        vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
        );

        float kernel[9];

        // 3: Sharpen
        if (mode == 3) {
            kernel = float[](
            -1.0, -1.0, -1.0,
            -1.0,  9.0, -1.0,
            -1.0, -1.0, -1.0
            );
        }
        // 4: Blur
        else if (mode == 4) {
            float v = 1.0 / 16.0;
            kernel = float[](
            1.0*v, 2.0*v, 1.0*v,
            2.0*v, 4.0*v, 2.0*v,
            1.0*v, 2.0*v, 1.0*v
            );
        }
        // 5: Edge Detection
        else {
            kernel = float[](
            1.0,  1.0, 1.0,
            1.0, -8.0, 1.0,
            1.0,  1.0, 1.0
            );
        }

        // Apply Kernel to the Scene (ignoring Bloom to keep edges crisp)
        for(int i = 0; i < 9; i++) {
            // Note: We use 'scene' instead of 'screenTexture'
            vec3 sampleCol = texture(scene, TexCoords + offsets[i]).rgb;
            color += sampleCol * kernel[i];
        }
    }
    // =======================================================
    // PATH B: Standard HDR + Bloom (Modes 0, 1, 2)
    // =======================================================
    else {
        vec3 hdrColor = texture(scene, TexCoords).rgb;
        vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;

        if(bloom) {
            hdrColor += bloomColor;
        }
        color = hdrColor;
    }

    // =======================================================
    // POST-PROCESSING COMMON (Tone Mapping & Gamma)
    // =======================================================

    // 1. Tone Mapping (HDR -> LDR)
    // We apply this even to Kernel modes so the brightness matches
    //vec3 result = vec3(1.0) - exp(-color * exposure);
    vec3 result = color * exposure;

    // 2. Gamma Correction
    result = pow(result, vec3(1.0 / 2.2));

    // =======================================================
    // COLOR FILTERS (Inverse / Grayscale)
    // =======================================================

    // Mode 1: Inversion
    if (mode == 1) {
        result = vec3(1.0) - result;
    }
    // Mode 2: Grayscale
    else if (mode == 2) {
        float avg = 0.2126 * result.r + 0.7152 * result.g + 0.0722 * result.b;
        result = vec3(avg);
    }

    FragColor = vec4(result, 1.0);
}