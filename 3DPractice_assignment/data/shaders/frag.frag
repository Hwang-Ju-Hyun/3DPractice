#version 400

out vec4 FragColor;

in vec4 color;
in vec2 UV;

void main()
{   
    FragColor = color;
}