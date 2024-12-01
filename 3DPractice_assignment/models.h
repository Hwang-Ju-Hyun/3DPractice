#pragma once

#include "CS300Parser.h"

struct Model
{
	CS300Parser::Transform transf;

	//TODO
	glm::mat4x4 ComputeMatrix();

	std::vector<glm::vec3> points;
	std::vector<unsigned int> indicies;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UV;

	std::vector<float> vertices;

	std::vector<int> pointIndeces;
	std::vector<int> normalIndeces;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO=-1;

	void LoadModel();

	Model(const CS300Parser::Transform& _transform);
	~Model();
	
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
	void GenerateCylinder(int slices);
	void GenerateCone(int slices);
public:
	glm::vec3 calculateNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) 
	{
		glm::vec3 v1 = p2 - p1;
		glm::vec3 v2 = p3 - p1;
		return glm::normalize(glm::cross(v1, v2));  // 외적을 계산하여 법선 벡터를 얻음
	}
};