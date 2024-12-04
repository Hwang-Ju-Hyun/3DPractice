#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;

uniform mat4 model;

out vec4 color;
out vec3 Normal;
out vec2 UV;


out vec2 fragTexCoord;
// 세계 공간에서의 법선
out vec3 fragNormal;
// 세계 공간에서의 위치
out vec3 fragWorldPos;


void main()
{
   gl_Position = model * vPosition;   
   UV=vTextCoords;
   color = vec4(0.5,0.5,0.5,1.0);   
   
   fragWorldPos=(model*vPosition).xyz;
   fragNormal=(vec4(vNormals,0.0)*model).xyz;
   fragTexCoord=vTextCoords;
}