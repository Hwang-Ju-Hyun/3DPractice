#version 400

out vec4 FragColor;
in vec2 UV;
uniform sampler2D myTextureSampler;
uniform bool hasTexture;
uniform bool normal;
uniform bool LightColorOn;
in vec3 Normal;

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

void main()
{   	 	
	vec3 Phong=vec3(0.0f,0.0f,0.0f);
	for(int i=0;i<uLightNum;i++)
	{
		vec3 N=normalize(fragNormal);   // 법선 벡터
		vec3 L=normalize(-uLight[i].dir);//표면에서 광원으로 향하는 벡터
		vec3 V=normalize(uCameraPos - fragWorldPos);//표면에서 카메라로 향하는 벡터
		vec3 R = normalize(reflect(-L, N)); //reflector 아마 specular에서 쓰일듯		

		float NdotL = dot(N, L);						
		vec3 Specular=vec3(0.f,0.f,0.f);
		vec3 light_Direction=vec3(0.f,0.f,0.f);

	
		
		//if(uLight[i].type == 1)
		//{
		//	light_Direction=normalize(uLight[i].positionWorld-fragWorldPos);
		//}
		//else
		//{
		//	light_Direction=L;
		//}
		light_Direction=L;
		
		vec3 reflection=2.0 * (dot(N, light_Direction))* N - light_Direction;
		float sp = pow(max(dot(reflection, V), 0.0), mp_shininess);
		Specular=uLight[i].col*sp;


		float dist=distance(fragWorldPos,uLight[i].positionWorld);
		float att=min(1.0/(uLight[i].att.x+uLight[i].att.y*dist+uLight[i].att.z*dist*dist),1);		

		vec3 Ambient  = uLight[i].col * uLight[i].amb  *    vec3(UV,0);


		///assignment3			
		vec3 normalMap_norm=normalize(2.0*texture(uNormalMap,UV).xyz-0.1);
		vec3 v_lightTS=tbnMat*normalize(light_Direction);
		vec3 v_viewTS=tbnMat*normalize(V);
		vec3 light =normalize(v_lightTS);
		vec3 view=normalize(v_viewTS);


		float diff=max(dot(normalMap_norm,v_lightTS),0.0);
		vec3 Diffuse  = uLight[i].col     *    vec3(UV,0)  *     diff;
		///assignment3			


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
		}
		else if(uLight[i].type==2)// DIR
		{
			att=1.f;			
			Phong += att*(Diffuse + Specular);
		}
		else //POINT
		{
			Phong += Ambient+att*(Diffuse + Specular);
		}						
	}	
	if(hasTexture)		
		FragColor=texture(myTextureSampler, UV) * vec4(Phong,1.0f);
	else
		FragColor = vec4(UV,0, 1.0)*vec4(Phong,1.0f);
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
}