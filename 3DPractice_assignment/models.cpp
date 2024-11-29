#include "models.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gtc/matrix_transform.hpp>


//#define TINYOBJLOADER_IMPLEMENTATION
#include "../extern/tiny_obj_loader.h"

int Model::slices = 5;

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
		vertices.push_back(normals[i].x);
		vertices.push_back(normals[i].y);
		vertices.push_back(normals[i].z);
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

	//Gen VAO
	
	//Assign Coordinates
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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
	//TODO: Points

	//TODO: UVs

	//TODO: Normals
}

void Model::CreateModelCube()
{
 //TODO: Points
 
 //TODO: UVs

 //TODO: Normals
}

void Model::CreateModelCone(int slices)
{
	//TODO: Points

	//TODO: UVs

	//TODO: Normals
}

void Model::CreateModelCylinder(int slices)
{
	//TODO: Points

	//TODO: UVs

	//TODO: Normals
}

void Model::CreateModelSphere(int slices)
{
	//TODO: Points

	//TODO: UVs

	//TODO: Normals
}
