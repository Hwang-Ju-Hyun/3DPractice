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
        glm::vec3 dir;//∫˚¿« πÊ«‚(SPOT,DIRECTIONAL)
        glm::vec3 col;
        glm::vec3 att;     //c1,c2,c3
        float     amb     = 0.0f;
        float     inner   = 0.0f;
        float     outer   = 30.0f;
        float     falloff = 1.0f;
        float     bias    = 0.0f;
        unsigned  pcf     = 0;

        glm::vec4 ambientColor = { 0.f,0.f,0.f  ,0.f };//Intensity ¡÷∫Ø±§
        glm::vec4 diffuseColor = { 0.f,0.f,0.f ,0.f }; //diffuse ±§
        glm::vec4 specularColor = { 0.f,0.f,0.f,0.f }; //specular ±§
        
        std::string type = "POINT";

        float spotExponent;//
        float spotCutoff;
        // (range: [0.0,90.0], 180.0)
        float spotCosCutoff; // (range: [1.0,0.0], -1.0)
        float constantAttenuation;
        float linearAttenuation;
        float quadraticAttenuation;

        std::vector<Animations::Anim> anims;
    };
    std::vector<Light> lights;

    std::array<std::string, 6> environmentMap;

    struct MaterialParameters
    {
        glm::vec4 ambient = { 1.f,1.f,1.f  ,1.f };//coefficient ambient
        glm::vec4 diffuse = { /*≈ÿΩ∫√ƒƒÆ∂Û*/};
        glm::vec4 specular = { 1.f,1.f,1.f,1.f };//coefficient  specular
        float shininess = 0.f; //ns in specular
    };
    

  private:
    static float     ReadFloat(std::ifstream & f);
    static int       ReadInt(std::ifstream & f);
    static glm::vec3 ReadVec3(std::ifstream & f);
};