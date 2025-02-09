#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in mat4 modelInstanceMatrix;

uniform mat4 projection;

void main()
{
    gl_Position = projection * modelInstanceMatrix * vec4(aPos, 0.0, 1.0);
}