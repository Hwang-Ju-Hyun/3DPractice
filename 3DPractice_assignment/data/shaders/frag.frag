#version 400

out vec4 FragColor;
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform bool hasTexture;
uniform bool normal;
uniform bool LightColorOn;

uniform sampler2D uNormalMap;

uniform vec3 mp_ambient;
uniform vec3 mp_diffuse;
uniform vec3 mp_specular;
uniform float mp_shininess;


struct Light
{
    int  type;
    vec3 col;
	vec3 positionWorld;
	float amb;
	vec3 att;
	vec3 dir;	    	
	
    float     inner;
    float     outer;
    float     falloff;       
};
 

uniform int   uLightNum;
uniform Light uLight[5];
  
uniform vec3 uCameraPos;

 
 
in vec2 fragTexCoord;
// 세계 공간에서의 법선
in vec3 fragNormal;
// 세계 공간에서의 위치
in vec3 fragWorldPos;

in mat3 tbnMat;//tanget space로 가는 행렬

uniform mat4 modeltoworld;

in vec3 temp;

void main()
{   	 					
	vec3 Phong=vec3(0.0f,0.0f,0.0f);	
	//tangent space
	vec3 normalMap_norm = normalize(2.0*texture(uNormalMap,fragTexCoord).xyz-1.0);
		
	//tangent space -> model space
	normalMap_norm = tbnMat * normalMap_norm;

	//model space->world space
	normalMap_norm = normalize(modeltoworld * vec4(normalMap_norm,0.f)).xyz;

	for(int i=0;i<uLightNum;i++)
	{
		vec3 L=normalize(vec3(uLight[i].positionWorld) - fragWorldPos);//표면에서 광원으로 향하는 벡터		

		//light is worldspace
		vec3 light_Direction=L;


		vec3 view=normalize((uCameraPos - fragWorldPos));//표면에서 카메라로 향하는 벡터																	


		//Att | Ambient
		float dist    = length(uLight[i].positionWorld-fragWorldPos);
		float att     = min(1.0f/(uLight[i].att.x+(uLight[i].att.y*dist)+(uLight[i].att.z*dist*dist)),1.0f);		
		vec3 Ambient  = mp_ambient* uLight[i].col * uLight[i].amb  * texture(myTextureSampler,UV).rgb;

		//Diffuse
		float diff=max(dot(normalMap_norm,light_Direction),0.0);
		vec3 Diffuse  = uLight[i].col     *    texture(myTextureSampler,UV).rgb  *     diff;


		//Specular
		vec3 reflection=2.0 *dot(normalMap_norm, light_Direction)*normalMap_norm - light_Direction;
		float sp = pow(max(dot(reflection, view), 0.0), mp_shininess);		
		vec3 Specular=mp_specular*sp;
		


		//Specular = vec3(0,0,0);

		if(uLight[i].type==3)//SPOT
		{
			vec3 D = normalize(uLight[i].dir);
			vec3 lightToTarget=normalize(fragWorldPos-uLight[i].positionWorld);
			float LdotD=dot(lightToTarget,D);
			float EffectAngle= LdotD/(length(lightToTarget)*length(D));
			
			float Phi=cos(radians(uLight[i].outer));
			float Theta=cos(radians(uLight[i].inner));
				
			float SpotLightEffect=pow((EffectAngle-Phi)/(Theta-Phi),uLight[i].falloff);

			SpotLightEffect=clamp(SpotLightEffect,0,1);
			
			Phong += att*(SpotLightEffect*(Diffuse + Specular));			
			FragColor=texture(uNormalMap,UV);
			return;
		}
		else if(uLight[i].type==2)// DIR
		{
			att=1.f;
			Phong += att*(Diffuse + Specular);		
			FragColor=texture(uNormalMap,UV);
			return;
		}
		else //POINT
		{
			

			Phong += Ambient + att*(Diffuse+ Specular);																			
		}		
		
	}		
	

	if(LightColorOn)
	{
		FragColor=vec4(1.0,1.0,1.0,1.0);			
	}
	else
	{
		if(hasTexture)		
			FragColor=texture(myTextureSampler, UV) * vec4(Phong,1.0f);
		else
		{
			//FragColor = vec4(UV,0, 1.0)*vec4(Phong,1.0f);
			FragColor = vec4(Phong,1.0f);
		}
			
	}




		



	//if(hasTexture)		
	//	FragColor=texture(myTextureSampler, UV) * vec4(Phong,1.0f);
	//else
	//	FragColor = vec4(UV,0, 1.0)*vec4(Phong,1.0f);
	//FragColor=vec4(Phong,1.0);
	//FragColor=vec4(fragNormal,1);
	//FragColor = vec4((normalize(fragNormal)+vec3(1,1,1))/2,1.0f);
	//if(normal)
	//{
	//	FragColor=vec4(1.0,0.0,0.0,1.0);
	//}
	//else
	//{
	//	if(LightColorOn)
	//	{
	//		FragColor=vec4(1.0,1.0,1.0,1.0);			
	//	}
	//	else
	//	{
	//		if(hasTexture)		
	//			FragColor=texture(myTextureSampler, UV) * vec4(Phong,1.0f);
	//		else
	//			FragColor = vec4(UV,0, 1.0)*vec4(Phong,1.0f);
	//	}
	//	
	//}	


	//DEBUG
	//FragColor = texture(myTextureSampler,UV);
	//FragColor = texture(uNormalMap,UV);
	//FragColor = vec4((normalMap_norm + vec3(1,1,1))/2,1);
	//FragColor = vec4((fragNormal + vec3(1,1,1))/2,1);
	//FragColor = vec4((temp + vec3(1,1,1))/2,1);
	//FragColor = vec4(mp_shininess/10,0,0,1);
}