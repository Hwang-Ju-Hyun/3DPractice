#version 400

out vec4 FragColor;
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform bool hasTexture;
uniform bool normal;

void main()
{   	
	
	if(normal)
	{
		FragColor=vec4(1.0,0.0,0.0,1.0);
	}
	else
	{
		if(hasTexture)
			FragColor = texture(myTextureSampler, UV);
		else
			FragColor = vec4(UV,0, 1.0);
	}
	
}