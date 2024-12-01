#include "models.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gtc/matrix_transform.hpp>


//#define TINYOBJLOADER_IMPLEMENTATION
#include "../extern/tiny_obj_loader.h"

int Model::slices = 4;


glm::mat4x4 Model::ComputeMatrix()
{	
	glm::mat4 translate = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);

	glm::mat4 rotateYXZ = glm::mat4(1.0f);

	glm::mat4 temp = glm::mat4(1.0f);
	glm::mat4 a = glm::mat4(1.0f);
	glm::mat4 b = glm::mat4(1.0f);
	glm::mat4 c = glm::mat4(1.0f);

	translate = glm::translate(translate, transf.pos);
	scale = glm::scale(scale, transf.sca);

	a = glm::rotate(a, glm::radians(transf.rot.y), { 0,1,0 });
	b = glm::rotate(b, glm::radians(transf.rot.x), { 1,0,0 });
	c = glm::rotate(c, glm::radians(transf.rot.z), { 0,0,1 });

	rotateYXZ = a * b * c;

	temp = translate * rotateYXZ * scale;

	return temp;
}

void Model::LoadModel()
{
	//If exception. use one of our functions
	if (transf.mesh == "PLANE")
		CreateModelPlane();
	else if (transf.mesh == "CUBE")
		CreateModelCube();
	else if (transf.mesh == "SPHERE")
		CreateModelSphere(slices);
	else if (transf.mesh == "CYLINDER")
		CreateModelCylinder(slices);
	else if (transf.mesh == "CONE")
		CreateModelCone(slices);
	else
	{
		//Else, use the general
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, transf.mesh.c_str());

		std::vector<glm::vec3> temp;
		std::vector<glm::vec3> tempN;
		std::vector<glm::vec2> tempUV;

		//Save mesh points
		for (int i = 0; i < attrib.vertices.size(); i += 3)
		{
			temp.push_back({ attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2] });
		}

		//Save mesh normals
		for (int i = 0; i < attrib.normals.size(); i += 3)
		{
			tempN.push_back({ attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2] });
		}

		//Save UV
		for (int i = 0; i < attrib.texcoords.size(); i += 2)
		{
			tempUV.push_back({ attrib.texcoords[i], attrib.texcoords[i + 1]});
		}

		//Load Shapes (vertex indexes)
		for (auto s : shapes)
		{
			for (auto p : s.mesh.indices)
			{
				//Load vertexes
				points.push_back(temp[p.vertex_index]);
				//Load Normals
				normals.push_back(tempN[p.normal_index]);
				//Load Indexes
				UV.push_back(tempUV[p.texcoord_index]);
			}
		}
	}
}

Model::Model(const CS300Parser::Transform& _transform) : transf(_transform), VBO(0), VAO(0)
{
	//load points
	LoadModel();
	
	int s = points.size();
	//vertices
	for (int i = 0; i < s; i++)
	{
		//points
		vertices.push_back(points[i].x);
		vertices.push_back(points[i].y);
		vertices.push_back(points[i].z);
		//normals
		if (this->transf.name == "cube")
		{
			vertices.push_back(normals[i].x);
			vertices.push_back(normals[i].y);
			vertices.push_back(normals[i].z);
		}
		else
		{
			vertices.push_back(normals[i].x);
			vertices.push_back(normals[i].y);
			vertices.push_back(normals[i].z);
		}
		
		//UV
		vertices.push_back(UV[i].x);
		vertices.push_back(UV[i].y);
	}

	//Sanity Check
	if (vertices.size() == 0)
		return;

	//Gen VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(float)), &vertices[0], GL_STATIC_DRAW);

	//Assign Coordinates
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//My EBO
	if (this->transf.name == "plane"||this->transf.name=="cube"|| this->transf.name == "cone"||this->transf.name=="cylinder"||this->transf.name=="sphere")
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicies.size(), &indicies[0], GL_STATIC_DRAW);
	}	
	


	//Assign Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	//Assign UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);




	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

Model::~Model()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

//TODO:
void Model::CreateModelPlane()
{
	points = {
		{-0.5f,0.5f,0.0f},			// 0 : 왼쪽 상단 
		{-0.5f,-0.5f,0.0f},			// 1 : 왼쪽 하단
		{0.5f,-0.5f,0.f},			// 2 : 우측 하단		
		{0.5f,0.5f,0.f}				// 3 : 우측 상단
	};

	indicies = { 0,1,2,0,3,2 };
			
	UV = {
		{0.0f, 1.0f},				// 0 : 왼쪽 상단 
		{0.0f, 0.0f},				// 1 : 왼쪽 하단
		{1.0f, 0.0f},				// 2 : 우측 하단
		{0.0f, 1.0f},				// 0 : 왼쪽 상단
		{1.0f, 0.0f},				// 2 : 우측 하단
		{1.0f, 1.0f}				// 3 : 우측 상단
	};
	
	normals = {
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}, 
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}, 
		{0.0f, 0.0f, 1.0f}, 
		{0.0f, 0.0f, 1.0f}
	};
}

void Model::CreateModelCube()
{	
 //TODO: Points
	points = {
		{-0.5f, 0.5f, -0.5f},	//왼 윗 뒷		0
		{-0.5f, 0.5f, 0.5f },	//왼 윗 앞		1
		{0.5f, 0.5f, 0.5f }, 	//오 윗 앞		2
		{0.5f, 0.5f, -0.5f },	//오 윗 뒷		3
		{-0.5f, -0.5f, -0.5f},	//왼 밑 뒷		4
		{-0.5f, -0.5f, 0.5f},	//왼 밑 앞		5
		{0.5f, -0.5f, 0.5f},	//오 밑 앞		6
		{0.5f, -0.5f, -0.5f }	//오 밑 뒷		7
	};
	indicies = {
		{0,1,2,
		0,2,3,		//윗면

		1,5,6,
		1,6,2,		//앞면

		2,6,7,
		2,7,3,		//우측면

		0,4,5,
		0,5,1,		//좌측면

		5,4,6,
		4,7,6,		//아랫면

		0,7,4,		//뒷면
		0,3,7}
	};
 
 //TODO: UVs
	UV = {
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // Top
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // Front
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // Right
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // Left
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, // Bottom
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}  // Back
	};

	normals = {
		{0.0f, 1.0f, 0.0f},			// 윗면
		{0.0f, 0.0f, 1.0f},			// 앞면
		{1.0f, 0.0f, 0.0f},			// 우측면	
		{-1.0f, 0.0f, 0.0f},		// 좌측면
		{0.0f, -1.0f, 0.0f},		// 아랫면
		{0.0f, 0.0f, -1.0f}			// 뒷면
	};

	 // 법선 벡터를 각 정점에 대해 계산하기 위한 벡터
	std::vector<glm::vec3> vertexNormals(8, glm::vec3(0.0f));

	// 면의 법선 벡터 계산
	for (size_t i = 0; i < indicies.size(); i += 3) {
		// 각 삼각형의 세 정점
		glm::vec3 v0 = points[indicies[i]];
		glm::vec3 v1 = points[indicies[i + 1]];
		glm::vec3 v2 = points[indicies[i + 2]];

		// 면의 법선 벡터 계산
		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		// 해당 면에 속한 각 정점에 법선 벡터 추가
		vertexNormals[indicies[i]] += normal;
		vertexNormals[indicies[i + 1]] += normal;
		vertexNormals[indicies[i + 2]] += normal;
	}

	// 각 정점의 법선 벡터 정규화
	/*for (auto& normal : vertexNormals) {
		normal = glm::normalize(normal);
	}*/

	// 법선 벡터를 모델의 normals 배열에 저장
	normals = vertexNormals;
}

void Model::CreateModelCone(int slices)
{		
	points.clear();
	normals.clear();
	UV.clear();
	float angle_triangle = 360.0f / slices;
	
	glm::vec3 topVertex = { 0.0f, 0.5f, 0.0f }; 
	points.push_back(topVertex);
	normals.push_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	UV.push_back(glm::vec2(0.5f, 1.0f));  
	      
	for (int i = 0; i < slices; i++) 
	{
		float angle = glm::radians(i * angle_triangle);
		float x = 0.5f * std::cos(angle);  
		float z = 0.5f * std::sin(angle);		 
		points.push_back({x,-0.5f,z});
		normals.push_back(glm::vec3{ 0.0f, -1.0f, 0.0f });

		// UV mapping for the base
		float u = (x + 0.5f) / 1.0f;  // Convert x to the [0,1] range
		float v = (z + 0.5f) / 1.0f;  // Convert z to the [0,1] range
		UV.push_back(glm::vec2(u, v));
	}

	
	for (int i = 1; i < slices; ++i) 
	{
		indicies.push_back(0); 
		indicies.push_back(i);
		indicies.push_back(i + 1);
	}

	indicies.push_back(0);
	indicies.push_back(slices);
	indicies.push_back(1);

	
	for (int i = 1; i < slices - 1; ++i) 
	{
		indicies.push_back(1);
		indicies.push_back(i + 1);
		indicies.push_back(i);
	}
}

void Model::CreateModelCylinder(int slices)
{
	points.clear();
	normals.clear();
	UV.clear();
	float angle_slice = 360.f / slices; 

	// 1. 윗면 , 아랫면
	for (int i = 0; i < slices; ++i) {
		float angle = glm::radians(i * angle_slice);
		float x = 0.5f * std::cos(angle);  
		float z = 0.5f * std::sin(angle);  
				
		points.push_back({x,0.5f,z});				
		points.push_back({x,-0.5f,z});

		
		normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));		
		normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));


		float u = (x + 0.5f) / 1.0f;   
		float v = (z + 0.5f) / 1.0f;   
		UV.push_back(glm::vec2{u, v}); 
		UV.push_back(glm::vec2{u, v}); 
	}

	//옆면 법선벡터
	for (int i = 0; i < slices; i++) 
	{		
		points;
		float angle = glm::radians(i * angle_slice);
		float x = 0.5f * cos(angle);
		float z = 0.5f * sin(angle);

		
		glm::vec3 sideNormal(x, 0.0f, z);
		sideNormal = glm::normalize(sideNormal); 
		
		normals.push_back(sideNormal); 
		normals.push_back(sideNormal); 

		float u = (i / float(slices));  
		UV.push_back(glm::vec2(u, 1.0f));  
		UV.push_back(glm::vec2(u, 0.0f));  
	}

	// 옆면 인덱스 
	for (int i = 0; i < slices; i++) 
	{
		int next = (i + 1) % slices;  
		
		// 첫 번째 삼각형 (상단-하단-상단)
		indicies.push_back(i * 2);      // 상단 점
		indicies.push_back(i * 2 + 1);  // 하단 점
		indicies.push_back(next * 2);   // 다음 상단 점

		// 두 번째 삼각형 (상단-하단-다음 하단)
		indicies.push_back(next * 2);   // 다음 상단 점
		indicies.push_back(i * 2 + 1);  // 하단 점
		indicies.push_back(next * 2 + 1); // 다음 하단 점
	}
}

void Model::CreateModelSphere(int slices)
{	
}













void Model::UpdateSlices()
{
	if (transf.name == "cylinder")
	{
		GenerateCylinder(slices); 
	}
	else if (transf.name == "cone")
	{
		GenerateCone(slices);
	}

	// VAO 바인딩
	glBindVertexArray(VAO);

	// VBO 갱신
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

	// EBO 갱신
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), indicies.data(), GL_STATIC_DRAW);	
}

void Model::GenerateCylinder(int slices)
{
	points.clear();
	indicies.clear();

	float angle_slice = 360.f / slices;

	// 1. 윗면 , 아랫면
	for (int i = 0; i < slices; ++i) {
		float angle = glm::radians(i * angle_slice);
		float x = 0.5f * std::cos(angle);
		float z = 0.5f * std::sin(angle);

		points.push_back({ x,0.5f,z });
		points.push_back({ x,-0.5f,z });


		normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));


		float u = (x + 0.5f) / 1.0f;
		float v = (z + 0.5f) / 1.0f;
		UV.push_back(glm::vec2{ u, v });
		UV.push_back(glm::vec2{ u, v });
	}

	//옆면 법선벡터
	for (int i = 0; i < slices; i++)
	{
		points;
		float angle = glm::radians(i * angle_slice);
		float x = 0.5f * cos(angle);
		float z = 0.5f * sin(angle);


		glm::vec3 sideNormal(x, 0.0f, z);
		sideNormal = glm::normalize(sideNormal);

		normals.push_back(sideNormal);
		normals.push_back(sideNormal);

		float u = (i / float(slices));
		UV.push_back(glm::vec2(u, 1.0f));
		UV.push_back(glm::vec2(u, 0.0f));
	}

	// 옆면 인덱스 
	for (int i = 0; i < slices; i++)
	{
		int next = (i + 1) % slices;

		// 첫 번째 삼각형 (상단-하단-상단)
		indicies.push_back(i * 2);      // 상단 점
		indicies.push_back(i * 2 + 1);  // 하단 점
		indicies.push_back(next * 2);   // 다음 상단 점

		// 두 번째 삼각형 (상단-하단-다음 하단)
		indicies.push_back(next * 2);   // 다음 상단 점
		indicies.push_back(i * 2 + 1);  // 하단 점
		indicies.push_back(next * 2 + 1); // 다음 하단 점
	}
}

void Model::GenerateCone(int slices)
{
	points.clear();
	indicies.clear();

	float angle_triangle = 360.0f / slices;

	glm::vec3 topVertex = { 0.0f, 0.5f, 0.0f };
	points.push_back(topVertex);
	normals.push_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	UV.push_back(glm::vec2(0.5f, 1.0f));

	for (int i = 0; i < slices; i++)
	{
		float angle = glm::radians(i * angle_triangle);
		float x = 0.5f * std::cos(angle);
		float z = 0.5f * std::sin(angle);
		points.push_back({ x,-0.5f,z });
		normals.push_back(glm::vec3{ 0.0f, -1.0f, 0.0f });

		// UV mapping for the base
		float u = (x + 0.5f) / 1.0f;  // Convert x to the [0,1] range
		float v = (z + 0.5f) / 1.0f;  // Convert z to the [0,1] range
		UV.push_back(glm::vec2(u, v));
	}


	for (int i = 1; i < slices; ++i)
	{
		indicies.push_back(0);
		indicies.push_back(i);
		indicies.push_back(i + 1);
	}

	indicies.push_back(0);
	indicies.push_back(slices);
	indicies.push_back(1);


	for (int i = 1; i < slices - 1; ++i) {
		indicies.push_back(1);
		indicies.push_back(i + 1);
		indicies.push_back(i);
	}
}