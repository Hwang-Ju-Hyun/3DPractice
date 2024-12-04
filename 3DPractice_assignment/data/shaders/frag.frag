#version 400

out vec4 FragColor;
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform bool hasTexture;
uniform bool normal;
in vec3 Normal;

struct MaterialParmeters
{
	vec4 emission;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

struct Light
{
    int  type;    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
	 float     amb     ;
     float     inner   ;
     float     outer   ;
     float     falloff ;
     float     bias    ;
     unsigned  pcf     

    vec3 position;
    float spotExponent;
	float spotCutoff; 
	float spotCosCutoff; // (range: [1.0,0.0], -1.0)
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

uniform MaterialParmeters material;
uniform int   uLightNum;
uniform Light uLight[5];
uniform vec3 uCameraPos;


in vec2 fragTexCoord;
// 세계 공간에서의 법선
in vec3 fragNormal;
// 세계 공간에서의 위치
in vec3 fragWorldPos;


void main()
{   	    

	vec3 N=normalize(fragNormal);
	vec3 L=normalize(uLight[0].position);
	vec3 V=normalize(uCameraPos - fragWorldPos);
	vec3 R = normalize(reflect(-L, N));


	

	vec3 Phong = uLight[0].ambient;
	float NdotL = dot(N, L);
	if(NdotL > 0)
	{
		vec3 Diffuse = uLight[0].diffuse * NdotL;
		vec3 Specular = uLight[0].specular * 
						pow(max(0.0, dot(R, V)), uLight[0].spotExponent);
		Phong += Diffuse + Specular;
	}	


	if(normal)
	{
		FragColor=vec4(1.0,0.0,0.0,1.0);
	}
	else
	{
		if(hasTexture)		
			texture(myTextureSampler, UV) * vec4(Phong,1.0f);					
		else
			FragColor = vec4(UV,0, 1.0);
	}
	
}