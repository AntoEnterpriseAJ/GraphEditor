#version 430 core

out vec4 fragColor;

uniform vec4 color = vec4(1.0f, 0.0f, 0.0f, 1.0f);

void main()
{
	fragColor = color;
}