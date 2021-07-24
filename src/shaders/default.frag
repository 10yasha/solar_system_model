#version 330 core

// output colors in RGBA
out vec4 FragColor;

// texture coordinates from vertex shader
in vec2 texCoord;

// texture unit
uniform sampler2D tex0;

in vec3 Normal;
in vec3 FragPosition;

uniform vec3 lightColor;
uniform vec3 lightPosition;

void main()
{
    // only have diffuse lighting for now
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - FragPosition);
    float diff = max(dot(norm, lightDirection), 0.07);
    vec3 diffuse = lightColor * diff * vec3(texture(tex0, texCoord));

    FragColor = vec4(diffuse, 1.0f);
}