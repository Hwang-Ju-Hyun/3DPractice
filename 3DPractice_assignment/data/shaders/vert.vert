#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;

uniform mat4 model;

out vec4 color;
out vec3 Normal;

void main()
{
   gl_Position = model * vPosition; 
   color = vec4(0.5,0.5,0.5,1);   

    // ���� ���ʹ� ���������� �ð�ȭ
    if (length(vNormals) > 0.0) {
        color = vec4(1.0, 0.0, 0.0, 1.0);  // ������
    } else {
        color = vec4(0.0, 0.0, 1.0, 1.0);  // �⺻�� �Ķ���
    }
}