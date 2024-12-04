#include "Level.h"
#include "CS300Parser.h"
#include "Controls.h"
#include "models.h"
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "animations.h"
#include <chrono>
#include <vector>


Level* Level::ptr = nullptr;

int Level::Initialize()
{
	// Initialize GLFW
	if (!glfwInit()) {
		return -1;
	}

	// Create a window
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Assignment 1", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	//Set callbacks
	glfwSetKeyCallback(window, Controls::keyCallback);

	//Load Scene	
	parser.LoadDataFromFile("data/scenes/scene_A1.txt");	
		

	//int light_size = parser.lights.size();
	/*for (int i = 0; i < light_size; i++)
	{
		int time = glfwGetTime();
		parser.lights[i].anims[i].Update(parser.lights[i].pos, time * 0.1f);
	}*/


	//Convert from parser->obj to Model
	for (auto o : parser.objects)
	{
		allObjects.push_back(new Model(o));
	}
	for (auto light : parser.lights)
	{
		light.obj.sca = { 1.f,1.f,1.f };
		allObjects.push_back(new Model(light.obj));		
	}

	//Save camera
	cam.fovy = parser.fovy;
	cam.width = parser.width;
	cam.height = parser.height;
	cam.nearPlane = parser.nearPlane;
	cam.farPlane = parser.farPlane;
	cam.camPos = parser.camPos;
	cam.camTarget = parser.camTarget;
	cam.camUp = parser.camUp;	
	cam.camRight = glm::vec3(1,0,0);
	cam.camFront = glm::vec3(0,0,100);

	//Shader program
	ReloadShaderProgram();
	//glEnable(GL_CULL_FACE);

	//glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);

	return 0;
}

void Level::LightUpdate(float _dt)
{
	time += _dt;
	std::vector<CS300Parser::Light> light = parser.lights;
	for (int i = 0; i < light.size(); i++)
	{
		light[i].obj.pos = light[i].pos;
		if (light[i].anims.size() > 0)
		{
			for (int j = 0; j < light[i].anims.size(); j++)
			{				
				light[i].obj.pos = light[i].anims[j].Update(light[i].obj.pos, time);
			}
		}
		light[i].obj.sca = { 5.f,5.f,5.f };	
	}
	int idx = 0;
	for (int i = 12; i < allObjects.size(); i++)
	{
		allObjects[i]->transf = light[idx].obj;		
		idx++;
	}
}

int Level::GetType(std::string _str)
{
	if (_str == "POINT")
		return 1;
	else if (_str == "DIR")
		return 2;
	else if(_str=="SPOT")
		return 3;
}


void Level::Run()
{
	glClearColor(0, 0, 0, 0);
	float TLastFrame = 0;

	// Main loop
	while (!glfwWindowShouldClose(window)) 
	{
		float TCurrentFrame = 0;

		
		std::chrono::time_point<std::chrono::steady_clock> time = std::chrono::steady_clock::now();		
		// Render graphics here
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		 //Update objects pos
		 for (auto obj : allObjects)
			 obj->ModelUpdate(TLastFrame);
		 
		 
		 LightUpdate(TLastFrame);
		 

		//use shader program
		glUseProgram(shader->handle);

		//Calculate Camera Matrix
		glm::vec3 dir = glm::normalize(cam.camTarget - cam.camPos);
		dir = -dir;
		glm::vec3 r = glm::normalize(glm::cross(cam.camUp, dir));
		glm::mat4 V = glm::mat4(1);
		glm::vec3 up = glm::normalize(glm::cross(dir, r));

		V[0][0] = r.x;
		V[1][0] = r.y;
		V[2][0] = r.z;
		V[0][1] = up.x;
		V[1][1] = up.y;
		V[2][1] = up.z;
		V[0][2] = dir.x;
		V[1][2] = dir.y;
		V[2][2] = dir.z;
		V[3][0] = -dot(r,cam.camPos );
		V[3][1] = -dot(up, cam.camPos);
		V[3][2] = -dot(dir, cam.camPos);

		//cam.ViewMat = glm::lookAt(cam.camPos, cam.camTarget, up);
		cam.ViewMat = V;

		//The image is mirrored on X
		cam.ProjMat = glm::perspective(glm::radians(cam.fovy), cam.width / cam.height, cam.nearPlane, cam.farPlane);		

		//For each object in the level
		for (auto o : allObjects)
		{
			//Render the object
			Render(o);
		}										

		glUseProgram(0);


		glfwSwapBuffers(window);
		glfwPollEvents();

		std::chrono::time_point<std::chrono::steady_clock> endtime = std::chrono::steady_clock::now();
		
		TCurrentFrame = std::chrono::duration<float>(endtime - time).count();
		TLastFrame = TCurrentFrame;
	}
	return;
}

void Level::Cleanup()
{
	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();

	DeletePtr();
}

Level* Level::GetPtr()
{
	if (!ptr)
		ptr = new Level;
	return ptr;
}

void Level::DeletePtr()
{
	if (ptr)
	{
		delete ptr;
	}
	
	ptr = nullptr;
}

#include <fstream>
#include <sstream>

void Level::ReloadShaderProgram()
{
	glUseProgram(0);

	if (shader)
		delete shader;
	
	std::stringstream v;
	std::stringstream f;

	std::ifstream file("data/shaders/vert.vert");

	if (file.is_open())
	{
		v << file.rdbuf();
	}

	file.close();
	file.open("data/shaders/frag.frag");
	f << file.rdbuf();
	file.close();

	shader = new cg::Program(v.str().c_str(), f.str().c_str());
}

void Level::RotateCamY(float angle)
{
	cam.camPos = cam.camTarget - glm::vec3(glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), cam.camUp) * glm::vec4(cam.camTarget - cam.camPos, 1));
}

void Level::RotateCamX(float angle)
{
	glm::vec3 right = glm::cross(cam.camUp, cam.camPos - cam.camTarget);
	glm::vec3 rotVec = glm::vec3(glm::rotate(glm::identity<glm::mat4>(), glm::radians(-angle), right) * glm::vec4(cam.camTarget - cam.camPos, 1));

	if (abs(rotVec.z) > 0.1f)
		cam.camPos = cam.camTarget - rotVec;
}

void Level::RotateCamZ(float angle)
{	
	cam.camPos += angle * (cam.camTarget - cam.camPos);
}



Model* Level::FindModel(std::string _name)
{
	int size = allObjects.size();
	for (int i = 0; i < size; i++)
	{
		if (allObjects[i]->transf.name == _name)
			return allObjects[i];
	}
	return nullptr;
}

bool showNormals = true;

void Level::Render(Model* obj)
{
	if(render_normal)
		RenderNormal(obj);

	//use obj VBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
	//use obj VAO
	glBindVertexArray(obj->VAO);

	//Send model matrix to the shader
	glm::mat4x4 m2w = obj->ComputeMatrix();

	//Send view matrix to the shader
	shader->setUniform("model", cam.ProjMat * cam.ViewMat * m2w);	

	glBindTexture(GL_TEXTURE_2D, obj->textureID);
	shader->setUniform("myTextureSampler", 0);	
	shader->setUniform("hasTexture", b_tex);
	shader->setUniform("normal", b_normal);		

	std::vector<CS300Parser::Light> all_lights=parser.lights;
	int light_size = all_lights.size();
	shader->setUniform("uLightNum", light_size);
	shader->setUniform("uCameraPos", Level::GetPtr()->cam.camPos);	
		

	for (int i = 0; i < light_size; i++)
	{		
		shader->setUniform("uLight[" + std::to_string(i) + "].type", GetType(all_lights[i].type));
		shader->setUniform("uLight[" + std::to_string(i) + "].ambient", all_lights[i].ambientColor);
		shader->setUniform("uLight[" + std::to_string(i) + "].diffuse", all_lights[i].diffuseColor);
		shader->setUniform("uLight[" + std::to_string(i) + "].specular",all_lights[i].specularColor);
		shader->setUniform("uLight[" + std::to_string(i) + "].positionWorld", all_lights[i].pos);		
	}

	CS300Parser::MaterialParameters mp;	
	shader->setUniform("material.ambient",  mp.ambient);
	shader->setUniform("material.diffuse",  mp.diffuse);
	shader->setUniform("material.specular", mp.specular);
	shader->setUniform("material.shininess",mp.shininess);


	//draw
	if ( obj->transf.name == "cylinder" || obj->transf.name == "sphere")
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->EBO);
		glDrawElements(GL_TRIANGLES, obj->indicies.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, obj->points.size());
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Level::RenderNormal(Model* _obj)
{	
	glBindBuffer(GL_ARRAY_BUFFER, _obj->normal_VBO);
	glBindVertexArray(_obj->normal_VAO);

	glm::mat4x4 m2w = _obj->ComputeMatrix();
	shader->setUniform("model", cam.ProjMat * cam.ViewMat * m2w);		
	glDrawArrays(GL_LINES, 0, _obj->points.size() *2);	
}

Level::Level(): window (nullptr), shader(nullptr)
{

}

Level::~Level()
{
	for (auto m : allObjects)
		delete m;

	allObjects.clear();
}
