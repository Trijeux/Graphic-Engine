#version 300 es
precision mediump float;

in vec2 fragTexCoords;
in vec3 fragWorldPos;
in mat3 TBN;

uniform sampler2D material_diffuse;
uniform sampler2D material_specular;
uniform sampler2D material_normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

out vec4 outColor;

void main()
{
    float specularPow = 32.0;
    vec3 diffuseColor  = texture(material_diffuse, fragTexCoords).rgb;
    vec3 specularMap  = texture(material_specular, fragTexCoords).rgb;

    vec3 normalMap = texture(material_normal, fragTexCoords).rgb;
    normalMap = normalMap * 2.0 - 1.0;
    vec3 normal = normalize(TBN * normalMap);

    vec3 lightDir = normalize(lightPos - fragWorldPos);
    vec3 viewDir  = normalize(viewPos - fragWorldPos);

    // Ambient
    vec3 ambient = 0.1 * diffuseColor;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * lightColor;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPow);
    vec3 specular = spec * specularMap * lightColor;

    outColor = vec4(ambient + diffuse + specular, 1.0);
}