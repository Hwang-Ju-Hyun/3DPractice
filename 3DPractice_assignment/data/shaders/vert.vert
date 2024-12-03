#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;

uniform mat4 model;

struct Light
{
    int  type;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 position;
    //  other variables 
    //  ...
};

out vec4 color;
out vec3 Normal;
out vec2 UV;

void main()
{
   gl_Position = model * vPosition; 
   UV=vTextCoords;
   color = vec4(0.5,0.5,0.5,1);      
}