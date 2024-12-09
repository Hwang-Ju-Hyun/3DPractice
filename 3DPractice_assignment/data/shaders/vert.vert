#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 modeltoworld;

out vec3 fragWorldPos;
out vec3 fragNormal;
out vec2 fragTexCoord;
out mat3 tbnMat;
out vec2 UV;
out vec3 temp;

void main()
{
    gl_Position = model * vPosition;
    fragWorldPos = (modeltoworld * vPosition).xyz; // World Space 위치
    fragNormal = normalize((modeltoworld * vec4(vNormals, 0.0)).xyz); // World Space 법선
    fragTexCoord = vTextCoords;
    UV=fragTexCoord;

    mat4 worldtomodel=inverse(modeltoworld);

    // Tangent Space 변환 행렬        
    vec3 Tan= normalize(tangent);    
    vec3 Bin = normalize(bitangent);   
    vec3 Nor =  normalize(cross(Tan,Bin));

    //goto object space matrix from tangents
    tbnMat = transpose(mat3(Tan, Bin, Nor));//columm major
}