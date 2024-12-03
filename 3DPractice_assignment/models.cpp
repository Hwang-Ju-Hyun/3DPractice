#include "models.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gtc/matrix_transform.hpp>


//#define TINYOBJLOADER_IMPLEMENTATION
#include "../extern/tiny_obj_loader.h"

int Model::slices = 19;


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
	if (transf.name != "cone")
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


	glGenBuffers(1, &normal_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
	glBufferData(GL_ARRAY_BUFFER, normal_vertices.size() * (sizeof(float) * 3), normal_vertices.data(), GL_STATIC_DRAW);

	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Assign Coordinates	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);




	///////////////////
	glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
	glGenVertexArrays(1, &normal_VAO);
	glBindVertexArray(normal_VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/////////////////




	//My EBO
	if (  this->transf.name == "cylinder" || this->transf.name == "sphere")
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicies.size(), &indicies[0], GL_STATIC_DRAW);
	}	
	glBindVertexArray(VAO);

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
	{ - 0.5 ,- 0.5 ,0},
	{ 0.5, 0.5, 0}	  ,
	{ - 0.5, 0.5, 0}  ,
	{ - 0.5, - 0.5, 0},
	{ 0.5, - 0.5, 0}  ,
	{ 0.5, 0.5, 0 }
	};	
			
	UV = {
		{0.0f, 0.0f},				// 0 : 왼쪽 상단 
		{1.0f,1.0f},				// 1 : 왼쪽 하단
		{0.0f, 1.0f},				// 2 : 우측 하단
		{0.0f, 0.0f},				// 0 : 왼쪽 상단
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
	points = {
		{0.5, 0.5, 0.5},
		{0.5, 0.5, - 0.5},
		{- 0.5, 0.5, - 0.5	},
		{0.5, 0.5, 0.5		},
		{- 0.5, 0.5, - 0.5	},
		{- 0.5, 0.5, 0.5		},
		{- 0.5, - 0.5, 0.5	},
		{0.5 ,- 0.5, 0.5		},
		{0.5, 0.5, 0.5		},
		{- 0.5, - 0.5, 0.5	},
		{0.5 ,0.5 ,0.5		},
		{- 0.5, 0.5 ,0.5		},
		{0.5 ,- 0.5, 0.5		},
		{0.5, - 0.5, - 0.5	},
		{0.5 ,0.5 ,0.5		},
		{0.5 ,- 0.5, - 0.5	},
		{0.5 ,0.5 ,- 0.5		},
		{0.5, 0.5 ,0.5		},
		{0.5 ,- 0.5, - 0.5	},
		{- 0.5 ,- 0.5 ,- 0.5	},
		{- 0.5, 0.5, - 0.5	},
		{0.5 ,- 0.5, - 0.5	},
		{- 0.5, 0.5, - 0.5	},
		{0.5 ,0.5 ,- 0.5		},
		{- 0.5 ,- 0.5, - 0.5	},
		{- 0.5, - 0.5, 0.5	},
		{- 0.5, 0.5, - 0.5	},
		{- 0.5, - 0.5, 0.5	},
		{- 0.5 ,0.5, 0.5		},
		{- 0.5 ,0.5, - 0.5	},
		{- 0.5 ,- 0.5, 0.5	},
		{- 0.5 ,- 0.5, - 0.5	},
		{0.5, - 0.5, - 0.5	},
		{- 0.5, - 0.5, 0.5	},
		{0.5, - 0.5, - 0.5	},
		{0.5, - 0.5, 0.5		},
	};


	//TODO: UVs
	UV = {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 0},
		{1, 1},
		{0, 1},
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 0},
		{1, 1},
		{0, 1},
		{0, 0},
		{1, 0},
		{0, 1},
		{1, 0},
		{1, 1},
		{0, 1},
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 0},
		{1, 1},
		{0, 1},
		{0, 0},
		{1, 0},
		{0, 1},
		{1, 0},
		{1, 1},
		{0, 1},
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 0},
		{1, 1},
		{0, 1 }
	};
	

	normals = {
		 {0, 1, 0	 },
		 {0, 1, 0	 },
		 {0, 1, 0	 },
		 {0, 1, 0	 },
		 {0, 1, 0	 },
		 {0, 1, 0	 },
		 {0, 0, 1	 },
		 {0, 0, 1	 },
		 {0, 0, 1	 },
		 {0, 0, 1	 },
		 {0, 0, 1	 },
		 {0, 0, 1	 },
		 {1, 0, 0	 },
		 {1, 0, 0	 },
		 {1, 0, 0	 },
		 {1, 0, 0	 },
		 {1, 0, 0	 },
		 {1, 0, 0	 },
		 {0, 0, - 1}  ,
		 {0, 0, - 1}  ,
		 {0, 0, - 1}  ,
		 {0, 0, - 1}  ,
		 {0, 0, - 1}  ,
		 {0, 0, - 1}  ,
		 {- 1, 0, 0}  ,
		 {- 1, 0, 0}  ,
		 {- 1, 0, 0}  ,
		 {- 1, 0, 0}  ,
		 {- 1, 0, 0}  ,
		 {- 1, 0, 0}  ,
		 {0, - 1, 0}  ,
		 {0, - 1, 0}  ,
		 {0, - 1, 0}  ,
		 {0, - 1, 0}  ,
		 {0, - 1, 0}  ,
		 {0, - 1, 0}
	};
}

void Model::CreateModelCone(int slices)
{		
	points.clear();
	normals.clear();
	UV.clear();
	float angle = 360.0f / slices;
	
	glm::vec3 top = { 0.f, 0.5f, 0.0f }; 
	glm::vec3 bottom_center = { 0.f,-0.5f,0.f };		
	int i = 0;
	for (i = 0; i < slices; i++) 
	{
		float angle_triangle = glm::radians(i * angle);
		float x1 = 0.5f * std::cos(angle_triangle);
		float z1 = 0.5f * std::sin(angle_triangle);
		points.push_back({ x1,-0.5f,z1});
		points.push_back({ top });				

		angle_triangle = glm::radians((i + 1) * angle);
		float x2 = 0.5f * std::cos(angle_triangle);
		float z2 = 0.5f * std::sin(angle_triangle);
		points.push_back({ x2,-0.5f,z2 });


		points.push_back({ x1,-0.5f,z1 });
		points.push_back({ x2,-0.5f,z2 });
		points.push_back({ bottom_center });					

		normals.push_back({ 0.0f, -1.0f, 0.0f }); normals.push_back({ 0.0f, -1.0f, 0.0f });
		normals.push_back({ 0.0f, -1.0f, 0.0f }); normals.push_back({ 0.0f, -1.0f, 0.0f });
		normals.push_back({ 0.0f, -1.0f, 0.0f }); normals.push_back({ 0.0f, -1.0f, 0.0f });		
		//TODO: UV좌표 수정해야됨
			 
		float uvx = (float)i / float(slices);
		float uvx2 = float(i + 1) / float(slices);
		float uvx1 = (uvx + uvx2) / 2.f;
		UV.push_back({ uvx,0.f }); UV.push_back({ uvx1,1.f }); UV.push_back({ uvx2,0.f }); 
		UV.push_back({ uvx,0.f }); UV.push_back({ uvx2,0.f }); UV.push_back({ uvx1,0.f });
		
	}		
	int a = 0;
	//float angle_triangle = glm::radians(i - 2 * angle);
	//float x1 = 0.5f * std::cos(angle_triangle);
	//float z1 = 0.5f * std::sin(angle_triangle);
	//points.push_back({ x1,-0.5f,z1 });
	//points.push_back({ top });
	//angle_triangle = glm::radians(i-1 * angle);

	//float x2 = 0.5f * std::cos(angle_triangle);
	//float z2 = 0.5f * std::sin(angle_triangle);
	//points.push_back({ x2,-0.5f,z2 });

	//points.push_back({ x1,-0.5f,z1 });
	//points.push_back({ x2,-0.5f,z2 });
	//points.push_back({ bottom_center });

	//normals.push_back({ 0.0f, -1.0f, 0.0f }); normals.push_back({ 0.0f, -1.0f, 0.0f });
	//normals.push_back({ 0.0f, -1.0f, 0.0f }); normals.push_back({ 0.0f, -1.0f, 0.0f });
	//normals.push_back({ 0.0f, -1.0f, 0.0f }); normals.push_back({ 0.0f, -1.0f, 0.0f });
	//normals.push_back({ 0.0f, -1.0f, 0.0f }); normals.push_back({ 0.0f, -1.0f, 0.0f });
	////TODO: UV좌표 수정해야됨
	//UV.push_back({ 0.f,0.f }); UV.push_back({ 0.f,0.f });
	//UV.push_back({ 0.f,0.f }); UV.push_back({ 0.f,0.f });
	//UV.push_back({ 0.f,0.f }); UV.push_back({ 0.f,0.f });
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

		
		normals.push_back({ 0.0f, 1.0f, 0.0f });
		normals.push_back({ 0.0f, -1.0f, 0.0f });

	}

	//옆면 법선벡터
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

	// VAO 바인딩
	glBindVertexArray(VAO);

	// VBO 갱신
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

	// EBO 갱신
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), indicies.data(), GL_STATIC_DRAW);	
}

void Model::MyCylinder(int slices)
{
	points.clear();
	indicies.clear();

	float angle_slice = 360.f / slices;

	// 1. 윗면 , 아랫면
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

	//옆면 법선벡터
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
			{  // 세로 반복
				for (int px = 0; px < 6; px++)
				{  // 가로 반복
					int idx = ((y * 6 + py) * width * 6 + (x * 6 + px)) * 3;
					data[idx] = colors[colorIdx].r * 255;
					data[idx + 1] = colors[colorIdx].g * 255;
					data[idx + 2] = colors[colorIdx].b * 255;
				}
			}
		}
	}
	//Texture 생성
	glGenTextures(1, &textureID);
	//TextureID에 바인딩
	glBindTexture(GL_TEXTURE_2D, textureID);
	//TextureData 삽입
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width * 6, height * 6, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//Data 삽입 glTexParameteri
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
