#version 400

out vec4 FragColor;
out vec4 normalColor;

uniform sampler2D ourTexture;

in vec4 color;
in vec2 UV;
in vec3 Normal;
in vec2 TexCoord;

void main()
{                                           
    FragColor = texture(ourTexture,TexCoord);
}                                           