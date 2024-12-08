#pragma once

#include "CS300Parser.h"
#include "stb_image.h"
struct Model
{
	CS300Parser::Transform transf;

	//TODO
	glm::mat4x4 ComputeMatrix();
	std::vector<glm::vec3>tangents;
	std::vector<glm::vec3>bitangents;
	std::vector<glm::vec3> points;
	std::vector<unsigned int> indicies;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> normals_avg;
	std::vector<glm::vec2> UV;

	std::vector<float> vertices;
	std::vector<glm::vec3> normal_vertices;

	std::vector<int> pointIndeces;
	std::vector<int> normalIndeces;
	unsigned int VBO;
	unsigned int normal_VBO;
	unsigned int VAO;
	unsigned int normal_VAO;
	unsigned int EBO=-1;

	void LoadModel();
	unsigned int textureID;
	Model(const CS300Parser::Transform& _transform);
	~Model();
	
	void ModelUpdate(float dt);
	
	static int slices;

public:
	//TODO
	void CreateModelPlane();
	void CreateModelCube();
	void CreateModelCone(int slices);
	void CreateModelCylinder(int slices);
	void CreateModelSphere(int slices);	
public:
	void UpdateSlices();
	void MyCylinder(int slices);
	void MyCone(int slices);
public:
	glm::vec3 calculateNormal(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3) 
	{
		glm::vec3 v1 = _p2 - _p1;
		glm::vec3 v2 = _p3 - _p1;
		return glm::normalize(glm::cross(v1, v2));  
	}
public:
	void Loadcheckboard();
	void MyLoadTexture();
	unsigned char* m_cData = nullptr;
	unsigned int m_iTextureID=0;
	float t = 0;
};