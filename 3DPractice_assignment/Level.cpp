#include "Level.h"
#include "CS300Parser.h"
#include "Controls.h"
#include "models.h"
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


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
	CS300Parser parser;
	parser.LoadDataFromFile("data/scenes/scene_A0.txt");

	//Convert from parser->obj to Model
	for (auto o : parser.objects)
	{
		allObjects.push_back(new Model(o));
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


void Level::Run()
{
	glClearColor(0, 0, 0, 0);
	// Main loop
	while (!glfwWindowShouldClose(window)) 
	{
		// Render graphics here
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	glm::vec3 camDirection = cam.camTarget - cam.camPos;
	glm::vec3 rotateDir = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(angle), cam.camRight) * glm::vec4(camDirection, 1.0f));
	cam.camPos = cam.camTarget - rotateDir;

	cam.camUp = glm::normalize(glm::cross(cam.camRight, glm::normalize(cam.camTarget - cam.camPos)));
}

void Level::RotateCamZ(float angle)
{	
	cam.camPos.z -= angle;
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



