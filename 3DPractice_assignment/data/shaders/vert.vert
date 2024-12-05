#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;

uniform mat4 model;
uniform mat4 modeltoworld;

out vec4 color;
out vec3 Normal;
out vec2 UV;


out vec2 fragTexCoord;
// ���� ���������� ����
out vec3 fragNormal;
// ���� ���������� ��ġ
out vec3 fragWorldPos;


void main()
{
   gl_Position = model * vPosition;   
   UV=vTextCoords;
   color = vec4(0.5,0.5,0.5,1.0);   
   
   fragWorldPos=(modeltoworld*vPosition).xyz;//�� ���� world ��ġ
   fragNormal=(modeltoworld*vec4(vNormals,0.0)).xyz;//�� ���� ������ world��ġ
   fragTexCoord=vTextCoords;
}