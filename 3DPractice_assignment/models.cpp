#include "models.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gtc/matrix_transform.hpp>


//#define TINYOBJLOADER_IMPLEMENTATION
#include "../extern/tiny_obj_loader.h"

int Model::slices = 6;

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
			vertices.push_back(normals[i%6].x);
			vertices.push_back(normals[i%6].y);
			vertices.push_back(normals[i%6].z);
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



	//My EBO
	if (this->transf.name == "plane"||this->transf.name=="cube"|| this->transf.name == "cone"||this->transf.name=="cylinder"||this->transf.name=="sphere")
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicies.size(), &indicies[0], GL_STATIC_DRAW);
	}	


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
	points = {
		{-0.5f,0.5f,0.0f},			// 0 : ¿ÞÂÊ »ó´Ü 
		{-0.5f,-0.5f,0.0f},			// 1 : ¿ÞÂÊ ÇÏ´Ü
		{0.5f,-0.5f,0.f},			// 2 : ¿ìÃø ÇÏ´Ü		
		{0.5f,0.5f,0.f}				// 3 : ¿ìÃø »ó´Ü
	};

	indicies = { 0,1,2,0,3,2 };
			
	UV = {
		{0.0f, 1.0f},				// 0 : ¿ÞÂÊ »ó´Ü 
		{0.0f, 0.0f},				// 1 : ¿ÞÂÊ ÇÏ´Ü
		{1.0f, 0.0f},				// 2 : ¿ìÃø ÇÏ´Ü
		{0.0f, 1.0f},				// 0 : ¿ÞÂÊ »ó´Ü
		{1.0f, 0.0f},				// 2 : ¿ìÃø ÇÏ´Ü
		{1.0f, 1.0f}				// 3 : ¿ìÃø »ó´Ü
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
		{-0.5f, 0.5f, -0.5f},	
		{-0.5f, 0.5f, 0.5f },	
		{0.5f, 0.5f, 0.5f }, 	
		{0.5f, 0.5f, -0.5f },	
		{-0.5f, -0.5f, -0.5f},	
		{-0.5f, -0.5f, 0.5f},	
		{0.5f, -0.5f, 0.5f},	
		{0.5f, -0.5f, -0.5f }	
	};
	indicies = {
		{0,1,2,
		0,2,3,		//À­¸é

		1,5,6,
		1,6,2,		//¾Õ¸é

		2,6,7,
		2,7,3,		//¿ìÃø¸é

		0,4,5,
		0,5,1,		//ÁÂÃø¸é

		5,4,6,
		4,7,6,		//¾Æ·§¸é

		0,7,4,		//µÞ¸é
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
 //TODO: Normals
	normals = {
		{0.0f, 1.0f, 0.0f},			// À­¸é
		{0.0f, 0.0f, 1.0f},			// ¾Õ¸é
		{1.0f, 0.0f, 0.0f},			// ¿ìÃø¸é	
		{-1.0f, 0.0f, 0.0f},		// ÁÂÃø¸é
		{0.0f, -1.0f, 0.0f},		// ¾Æ·§¸é
		{0.0f, 0.0f, -1.0f}			// µÞ¸é
	};
}

void Model::CreateModelCone(int slices)
{				
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

	
	for (int i = 1; i < slices - 1; ++i) {
		indicies.push_back(1);
		indicies.push_back(i + 1);
		indicies.push_back(i);
	}
}

void Model::CreateModelCylinder(int slices)
{
	
	float angle_slice = 360.f / slices; 

	// 1. À­¸é , ¾Æ·§¸é
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

	//¿·¸é ¹ý¼±º¤ÅÍ
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

		float u = (i / float(slices));  // u ÁÂÇ¥´Â 0~1 »çÀÌ·Î ±Õµî ºÐ¹è
		UV.push_back(glm::vec2(u, 1.0f));  // »ó´Ü ¿ø ÅØ½ºÃ³ ÁÂÇ¥
		UV.push_back(glm::vec2(u, 0.0f));  // ÇÏ´Ü ¿ø ÅØ½ºÃ³ ÁÂÇ¥
	}

	// ¿·¸é ÀÎµ¦½º 
	for (int i = 0; i < slices; i++) 
	{
		int next = (i + 1) % slices;  
		
		// Ã¹ ¹øÂ° »ï°¢Çü (»ó´Ü-ÇÏ´Ü-»ó´Ü)
		indicies.push_back(i * 2);      // »ó´Ü Á¡
		indicies.push_back(i * 2 + 1);  // ÇÏ´Ü Á¡
		indicies.push_back(next * 2);   // ´ÙÀ½ »ó´Ü Á¡

		// µÎ ¹øÂ° »ï°¢Çü (»ó´Ü-ÇÏ´Ü-´ÙÀ½ ÇÏ´Ü)
		indicies.push_back(next * 2);   // ´ÙÀ½ »ó´Ü Á¡
		indicies.push_back(i * 2 + 1);  // ÇÏ´Ü Á¡
		indicies.push_back(next * 2 + 1); // ´ÙÀ½ ÇÏ´Ü Á¡
	}
}

void Model::CreateModelSphere(int slices)
{	
}
