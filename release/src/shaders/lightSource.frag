#version 330 core

// output colors in RGBA
out vec4 FragColor;

// uvs from vertex shader
in vec2 texCoord;

// texture unit
uniform sampler2D tex0;

void main()
{
	FragColor = texture(tex0, texCoord);
}