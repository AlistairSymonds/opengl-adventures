#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

uniform float time;
out vec3 colour;
void main()
{
    vec3 rates = vec3(1.0, 5.0, 8.0);
    vec3 pos = sin(time*rates/2) * aPos;
    gl_Position = vec4(pos, 1.0);
    colour = sin(time*rates) * aColour;
}