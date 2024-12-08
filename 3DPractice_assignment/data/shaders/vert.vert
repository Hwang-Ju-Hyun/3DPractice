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

void main()
{
    gl_Position = model * vPosition;
    fragWorldPos = (modeltoworld * vPosition).xyz; // World Space 위치
    fragNormal = normalize((modeltoworld * vec4(vNormals, 0.0)).xyz); // World Space 법선
    fragTexCoord = vTextCoords;
    UV=fragTexCoord;
    // Tangent Space 변환 행렬
    vec3 Nor = normalize(transpose(inverse(mat3(modeltoworld))) * vNormals);
    vec3 Tan = normalize(transpose(inverse(mat3(modeltoworld))) * tangent);
    vec3 Bin = normalize(cross(Nor, Tan));
    tbnMat = transpose(mat3(Tan, Bin, Nor));
}