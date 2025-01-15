#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

uniform mat4 model;
uniform mat4 projection;

out vec4 color;

void main()
{
	gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	color = aColor;
}