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
	// ambient lighting
	float ambient = 0.07f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosition - FragPosition);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	FragColor = (texture(tex0, texCoord) * (diffuse + ambient));
}