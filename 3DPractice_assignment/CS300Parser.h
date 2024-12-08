#pragma once

#include <glm.hpp>

#include <string>
#include <vector>
#include <array>

#include "animations.h"

class CS300Parser
{
  public:
    void LoadDataFromFile(const char * filename);

    float     fovy      = 60.0f;
    float     width     = 16.0f;
    float     height    = 9.0f;
    float     nearPlane = 1.0f;
    float     farPlane  = 500.0f;
    glm::vec3 camPos;
    glm::vec3 camTarget;
    glm::vec3 camUp;

    struct Transform
    {
        std::string name;

        std::string mesh;
        std::string normalMap = "data/textures/default_normal.png";

        glm::vec3 pos;        
        glm::vec3 StartPos;
        glm::vec3 rot;
        glm::vec3 sca;
        float     ns        = 10.0f;
        float     ior       = 1.33f;
        bool      reflector = false;

        std::vector<Animations::Anim> anims;
    };

    std::vector<Transform> objects;

    struct Light
    {        
        Transform obj;
        glm::vec3 pos;
        glm::vec3 startPos;
        glm::vec3 dir;//빛의 방향(SPOT,DIRECTIONAL)
        glm::vec3 col;//모든 intensity from light
        glm::vec3 att;     //c1,c2,c3
        float amb;
        float     inner   = 0.0f;
        float     outer   = 30.0f;
        float     falloff = 1.0f;

        std::string type = "POINT";

        std::vector<Animations::Anim> anims;
    };
    std::vector<Light> lights;

    std::array<std::string, 6> environmentMap;

    struct MaterialParameters
    {
        glm::vec3 ambient = {1.f,1.f,1.f  };//coefficient ambient
        glm::vec3 diffuse = { 1.f,1.f,1.f,};//텍스쳐 칼라
        glm::vec3 specular = { 1.f,1.f,1.f };//coefficient  specular
        float shininess = 1.f; //ns in specular
    };
  private:
    static float     ReadFloat(std::ifstream & f);
    static int       ReadInt(std::ifstream & f);
    static glm::vec3 ReadVec3(std::ifstream & f);
};