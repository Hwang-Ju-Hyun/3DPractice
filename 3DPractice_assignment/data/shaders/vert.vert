#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;
layout(location=3)in vec3 tangent;
layout(location=3)in vec3 bitangent;

uniform mat4 model;
uniform mat4 modeltoworld;

out vec4 color;
out vec3 Normal;
out vec2 UV;


out vec2 fragTexCoord;
// 세계 공간에서의 법선
out vec3 fragNormal;
// 세계 공간에서의 위치
out vec3 fragWorldPos;


out mat3 tbnMat;

void main()
{      
   gl_Position = model * vPosition;   
   UV=vTextCoords;
   color = vec4(0.5,0.5,0.5,1.0);   
   
   fragWorldPos=(modeltoworld*vPosition).xyz;//각 모델의 world 위치
   fragNormal=(modeltoworld*vec4(vNormals,0.0)).xyz;//각 모델의 법선의 world위치
   
   vec3 Nor=normalize(transpose(inverse(mat3(modeltoworld)))*vNormals);//각 법선벡터의 world위치
   vec3 Tan=normalize(transpose(inverse(mat3(modeltoworld)))*tangent);//각 모델의 tanget의 world위치
   vec3 Bin=cross(Nor,Tan);
   tbnMat=transpose(mat3(Tan,Bin,Nor));   


   fragTexCoord=vTextCoords;
}