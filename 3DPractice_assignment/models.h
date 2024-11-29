#pragma once

#include "CS300Parser.h"

struct Model
{
	CS300Parser::Transform transf;

	//TODO
	glm::mat4x4 ComputeMatrix();

	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UV;

	std::vector<float> vertices;

	std::vector<int> pointIndeces;
	std::vector<int> normalIndeces;
	unsigned int VBO;
	unsigned int VAO;

	void LoadModel();

	Model(const CS300Parser::Transform& _transform);
	~Model();
	
	static int slices;

private:
	//TODO
	void CreateModelPlane();
	void CreateModelCube();
	void CreateModelCone(int slices);
	void CreateModelCylinder(int slices);
	void CreateModelSphere(int slices);

};