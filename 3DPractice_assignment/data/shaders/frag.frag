#version 400

out vec4 FragColor;
out vec4 normalColor;
in vec4 color;
in vec2 UV;
in vec3 Normal;

void main()
{   
    FragColor = color;
    normalColor=vec4(Normal,1);
}