#include "models.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gtc/matrix_transform.hpp>


//#define TINYOBJLOADER_IMPLEMENTATION
#include "../extern/tiny_obj_loader.h"

int Model::slices = 100;


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
	
	Loadcheckboard();

	int s = points.size();	
	int n = normals.size();
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

	//normal vector
	if (transf.name == "cube")
	{
		for (int i = 0; i < normals.size(); i++)
		{
			glm::vec3 start = points[i];
			glm::vec3 normal = normals[i];

			glm::vec3 end = start + normal;

			normal_vertices.push_back(start);			
			normal_vertices.push_back(end);			
		}
	}
	


	//Sanity Check
	if (vertices.size() == 0)
		return;

	//Gen VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * (sizeof(float)), &vertices[0], GL_STATIC_DRAW);

	//Gen normalVBO
	if (transf.name == "cube"|| transf.name == "plane")
	{
		glGenBuffers(1, &normal_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
		glBufferData(GL_ARRAY_BUFFER, normal_vertices.size() * (sizeof(float)*3), normal_vertices.data(), GL_STATIC_DRAW);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Assign Coordinates	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	//normal vao
	if (transf.name == "cube")
	{
		glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
		glGenVertexArrays(1, &normal_VAO);
		glBindVertexArray(normal_VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}	




	//My EBO
	if (this->transf.name == "plane"||this->transf.name=="cube"|| this->transf.name == "cone" || this->transf.name == "cylinder" || this->transf.name == "sphere")
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicies.size(), &indicies[0], GL_STATIC_DRAW);
	}	
	

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
		{-0.5f, 0.5f, -0.5f},	//¿Þ À­ µÞ		0
		{-0.5f, 0.5f, 0.5f },	//¿Þ À­ ¾Õ		1
		{0.5f, 0.5f, 0.5f }, 	//¿À À­ ¾Õ		2
		{0.5f, 0.5f, -0.5f },	//¿À À­ µÞ		3
		{-0.5f, -0.5f, -0.5f},	//¿Þ ¹Ø µÞ		4
		{-0.5f, -0.5f, 0.5f},	//¿Þ ¹Ø ¾Õ		5
		{0.5f, -0.5f, 0.5f},	//¿À ¹Ø ¾Õ		6
		{0.5f, -0.5f, -0.5f }	//¿À ¹Ø µÞ		7
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
	   //00 10 11
	   //00 11 01
	for (int i = 0; i < indicies.size(); i += 3) 
	{		
		int idx1 = indicies[i];
		int idx2 = indicies[i + 1];
		int idx3 = indicies[i + 2];

	
		UV.push_back({ (points[idx1].x + 0.5f), (points[idx1].y + 0.5f) });
		UV.push_back({ (points[idx2].x + 0.5f), (points[idx2].y + 0.5f) });
		UV.push_back({ (points[idx3].x + 0.5f), (points[idx3].y + 0.5f) });
	}

	normals = {
		{0.0f, 1.0f, 0.0f},			// À­¸é
		{0.0f, 0.0f, 1.0f},			// ¾Õ¸é
		{1.0f, 0.0f, 0.0f},			// ¿ìÃø¸é	
		{-1.0f, 0.0f, 0.0f},		// ÁÂÃø¸é
		{0.0f, -1.0f, 0.0f},		// ¾Æ·§¸é
		{0.0f, 0.0f, -1.0f}			// µÞ¸é
	};
	 
	std::vector<glm::vec3> tempvertex(8, glm::vec3{ 0.0f });
	
	for (int i = 0; i < indicies.size(); i += 3) {

		glm::vec3 v0 = points[indicies[i]];
		glm::vec3 v1 = points[indicies[i + 1]];
		glm::vec3 v2 = points[indicies[i + 2]];
		
		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		tempvertex[indicies[i]] += normal;
		tempvertex[indicies[i + 1]] += normal;
		tempvertex[indicies[i + 2]] += normal;
	}

	normals = tempvertex;	
	for (int i = 0; i < normals.size(); i++)
	{
		normals[i] = glm::normalize(normals[i]);
	}
}

void Model::CreateModelCone(int slices)
{		
	points.clear();
	normals.clear();
	UV.clear();
	float angle_triangle = 360.0f / slices;
	
	glm::vec3 top = { 0.0f, 0.5f, 0.0f }; 
	points.push_back(top);
	normals.push_back({0.0f, 1.0f, 0.0f });
	UV.push_back({ 0.5f, 1.0f });
	      
	for (int i = 0; i < slices; i++) 
	{
		float angle = glm::radians(i * angle_triangle);
		float x = 0.5f * std::cos(angle);  
		float z = 0.5f * std::sin(angle);		 
		points.push_back({x,-0.5f,z});
		normals.push_back({ 0.0f, -1.0f, 0.0f });

		//TODO: UVÁÂÇ¥ ¼öÁ¤ÇØ¾ßµÊ
		UV.push_back({ 0.f,0.f });
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

	//std::vector<glm::vec3> tempvertex(normals.size(), glm::vec3(0.0f));

	//for (size_t i = 0; i < indicies.size(); i += 3) {

	//	glm::vec3 v0 = points[indicies[i]];
	//	glm::vec3 v1 = points[indicies[i + 1]];
	//	glm::vec3 v2 = points[indicies[i + 2]];

	//	glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);

	//	tempvertex[indicies[i]] += normal;
	//	tempvertex[indicies[i + 1]] += normal;
	//	tempvertex[indicies[i + 2]] += normal;
	//}

	//normals = tempvertex;


}

void Model::CreateModelCylinder(int slices)
{
	points.clear();
	normals.clear();
	UV.clear();
	float angle_slice = 360.f / slices; 

	// 1. À­¸é , ¾Æ·§¸é
	for (int i = 0; i < slices; ++i) {
		float angle = glm::radians(i * angle_slice);
		float x = 0.5f * std::cos(angle);  
		float z = 0.5f * std::sin(angle);  
				
		points.push_back({x,0.5f,z});				
		points.push_back({x,-0.5f,z});

		
		normals.push_back({ 0.0f, 1.0f, 0.0f });
		normals.push_back({ 0.0f, -1.0f, 0.0f });

	}

	//¿·¸é ¹ý¼±º¤ÅÍ
	for (int i = 0; i < slices; i++) 
	{		
		points;
		float angle = glm::radians(i * angle_slice);
		float x = 0.5f * cos(angle);
		float z = 0.5f * sin(angle);

		
		glm::vec3 side_normal_vertex = { x, 0.0f, z };
		side_normal_vertex = glm::normalize(side_normal_vertex); 
		
		normals.push_back(side_normal_vertex); 		

		
		UV.push_back({0.f, 1.0f});  
		UV.push_back({0.f, 0.0f });
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


	std::vector<glm::vec3> tempvertex(points.size(), glm::vec3{ 0.0f });

	for (size_t i = 0; i < indicies.size(); i += 3) {

		glm::vec3 v0 = points[indicies[i]];
		glm::vec3 v1 = points[indicies[i + 1]];
		glm::vec3 v2 = points[indicies[i + 2]];

		glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
		
		tempvertex[indicies[i]] += normal;
		tempvertex[indicies[i + 1]] += normal;
		tempvertex[indicies[i + 2]] += normal;
	}

	normals = tempvertex;

}

void Model::CreateModelSphere(int slices)
{	
}













void Model::UpdateSlices()
{
	if (transf.name == "cylinder")
	{
		MyCylinder(slices); 
	}
	else if (transf.name == "cone")
	{
		MyCone(slices);
	}

	// VAO ¹ÙÀÎµù
	glBindVertexArray(VAO);

	// VBO °»½Å
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

	// EBO °»½Å
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), indicies.data(), GL_STATIC_DRAW);	
}

void Model::MyCylinder(int slices)
{
	points.clear();
	indicies.clear();

	float angle_slice = 360.f / slices;

	// 1. À­¸é , ¾Æ·§¸é
	for (int i = 0; i < slices; i++) 
	{
		float angle = glm::radians(i * angle_slice);
		float x = 0.5f * std::cos(angle);
		float z = 0.5f * std::sin(angle);

		points.push_back({ x,0.5f,z });
		points.push_back({ x,-0.5f,z });


		normals.push_back({0.0f, 1.0f, 0.0f});
		normals.push_back({ 0.0f, -1.0f, 0.0f });

		
		UV.push_back({ 0.f,1.f });
		UV.push_back({ 0.f,1.f });
	}

	//¿·¸é ¹ý¼±º¤ÅÍ
	for (int i = 0; i < slices; i++)
	{
		points;
		float angle = glm::radians(i * angle_slice);
		float x = 0.5f * cos(angle);
		float z = 0.5f * sin(angle);


		glm::vec3 side_normal = { x, 0.f, z };
		side_normal = glm::normalize(side_normal);
		
		normals.push_back(side_normal);
		
		UV.push_back({0.f, 1.0f });
		UV.push_back({0.f, 0.f});
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

void Model::MyCone(int slices)
{
	points.clear();
	indicies.clear();

	float angle_triangle = 360.f / slices;

	glm::vec3 topVertex = { 0.f, 0.5f, 0.f };
	points.push_back(topVertex);
	normals.push_back({ 0.f, 1.f, 0.f });
	UV.push_back({ 0.5f, 1.f });

	for (int i = 0; i < slices; i++)
	{
		float angle = glm::radians(i * angle_triangle);
		float x = 0.5f * std::cos(angle);
		float z = 0.5f * std::sin(angle);
		points.push_back({ x,-0.5f,z });
		normals.push_back({0.f, -1.f, 0.f});
				
		UV.push_back({ 0.f, 1.f });
	}


	for (int i = 1; i < slices; i++)
	{
		indicies.push_back(0);
		indicies.push_back(i);
		indicies.push_back(i + 1);
	}

	indicies.push_back(0);
	indicies.push_back(slices);
	indicies.push_back(1);


	for (int i = 1; i < slices - 1; i++) 
	{
		indicies.push_back(1);
		indicies.push_back(i + 1);
		indicies.push_back(i);
	}
}

void Model::Loadcheckboard()
{
	const int width = 6;
	const int height = 6;

	glm::vec3 colors[] =
	{
		glm::vec3(0, 0, 1),   // Blue
		glm::vec3(0, 1, 1),   // Cyan
		glm::vec3(0, 1, 0),   // Green
		glm::vec3(1, 1, 0),   // Yellow
		glm::vec3(1, 0, 0),   // Red
		glm::vec3(1, 0, 1)    // Purple
	};

	unsigned char* data = new unsigned char[width * height * 3 * 36];
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int colorIdx = (x + (height - 1 - y)) % 6;
			for (int py = 0; py < 6; py++)
			{  // ¼¼·Î ¹Ýº¹
				for (int px = 0; px < 6; px++)
				{  // °¡·Î ¹Ýº¹
					int idx = ((y * 6 + py) * width * 6 + (x * 6 + px)) * 3;
					data[idx] = colors[colorIdx].r * 255;
					data[idx + 1] = colors[colorIdx].g * 255;
					data[idx + 2] = colors[colorIdx].b * 255;
				}
			}
		}
	}
	//Texture »ý¼º
	glGenTextures(1, &textureID);
	//TextureID¿¡ ¹ÙÀÎµù
	glBindTexture(GL_TEXTURE_2D, textureID);
	//TextureData »ðÀÔ
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width * 6, height * 6, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//Data »ðÀÔ glTexParameteri
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
