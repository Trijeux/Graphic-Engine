#version 300 es
precision highp float;
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 fragWorldPos;
in vec2 fragTexCoords;
in vec3 Normal;
in mat3 TBN;

uniform sampler2D material_diffuse;
uniform sampler2D material_specular;
uniform sampler2D material_normal;
uniform bool useNormalMap;

void main() {
    // 1. Position
    gPosition = vec4(fragWorldPos, 1.0);

    // 2. Normal (Apply Normal Map)
    vec3 N = normalize(Normal);
    if(useNormalMap) {
        vec3 normalMap = texture(material_normal, fragTexCoords).rgb;
        normalMap = normalMap * 2.0 - 1.0;
        N = normalize(TBN * normalMap);
    }
    gNormal = vec4(N * 0.5 + 0.5, 1.0);

    // 3. Color + Specular
    gAlbedoSpec.rgb = texture(material_diffuse, fragTexCoords).rgb;
    gAlbedoSpec.a = texture(material_specular, fragTexCoords).r;
}
