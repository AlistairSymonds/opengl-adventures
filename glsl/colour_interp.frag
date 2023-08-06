#version 460 core
out vec4 FragColor;
in vec3 colour;
in vec2 texCoords;
uniform sampler2D frogeTex;

void main()
{
   
    //FragColor = vec4(colour, 1.0f);
    FragColor = texture(frogeTex, texCoords);
} 