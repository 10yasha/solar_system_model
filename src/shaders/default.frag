#version 330 core

// output colors in RGBA
out vec4 FragColor;

// texture coordinates from vertex shader
in vec2 texCoord;

// texture unit
uniform sampler2D tex0;

void main()
{
	// determines output fragment color here
	FragColor = texture(tex0, texCoord);
}