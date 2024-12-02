#pragma once

#define W_WIDTH 1280.0
#define W_HEIGHT 720.0

class GLFWwindow;
class Model;

#include <vector>
#include "program.h"
#include <iostream>

//Singleton level
struct Level
{
public:
	int Initialize();
	void Run();
	void Cleanup();

	static Level* GetPtr();
	static void DeletePtr();
public:
	void ReloadShaderProgram();
	void RotateCamY(float angle);
	void RotateCamX(float angle);
	void RotateCamZ(float angle);
public:
	glm::vec3 GetCam()const { return cam.camUp; }
	void printCamPos()const { std::cout << (int)cam.camPos.x<< "," << (int)cam.camPos.y << "," << (int)cam.camPos.z << std::endl; }
	Model* FindModel(std::string _name);
private:
	void Render(Model*);
	Level();
	~Level();
	Level(const Level&) = delete;
	const Level& operator=(const Level&) = delete;

	static Level* ptr;

	GLFWwindow* window;

	std::vector<Model*> allObjects;



	//camera 
	struct Camera
	{
		float     fovy = 60.0f;
		float     width = 16.0f;
		float     height = 9.0f;
		float     nearPlane = 1.0f;
		float     farPlane = 500.0f;
		glm::vec3 camPos;
		glm::vec3 camTarget;
		glm::vec3 camUp;
		glm::vec3 camRight;
		glm::vec3 camFront;

		glm::mat4 ViewMat;
		glm::mat4 ProjMat;
	};

	Camera cam;

	//shaders
	cg::Program* shader;	
public:
	void RenderNormal(Model* _obj);
	bool b_tex = false;
	bool render_normal = false;
};

