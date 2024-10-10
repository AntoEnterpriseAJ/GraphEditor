#version 330 core
layout (location = 0) in vec2 vertex;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vec2(vertex.x, vertex.y);
}