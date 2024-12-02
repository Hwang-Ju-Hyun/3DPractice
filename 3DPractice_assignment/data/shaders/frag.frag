#version 400

out vec4 FragColor;
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform bool hasTexture;

void main()
{   
	if(hasTexture)
		FragColor = texture(myTextureSampler, UV);
	else
		FragColor = vec4(UV,0.0, 1.0);
}