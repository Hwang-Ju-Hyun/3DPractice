#include "Controls.h"
#include "Level.h"
#include <GLFW/glfw3.h>
#include <iostream>

void Controls::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    //ESC - CLOSE WINDOW
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(pWindow, GL_TRUE);

    //SPACE - RELOAD SHADER
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_SPACE)
            Level::GetPtr()->ReloadShaderProgram();


    //TODO: ADD CAMERA CONTROLS 
    glm::vec3 camUp = Level::GetPtr()->GetCam();
    if (action == GLFW_PRESS|| action == GLFW_REPEAT)
    {        
        if (key == GLFW_KEY_W)        
            Level::GetPtr()->RotateCamX(-5.0f);                    
        if (key == GLFW_KEY_A)     
            Level::GetPtr()->RotateCamY(-5.0f);
        if (key == GLFW_KEY_D)
            Level::GetPtr()->RotateCamY(5.0f);
        if (key == GLFW_KEY_S)
            Level::GetPtr()->RotateCamX(5.0f);
        if (key == GLFW_KEY_Q)
            Level::GetPtr()->RotateCamZ(5.0f);
        if (key == GLFW_KEY_E)
            Level::GetPtr()->RotateCamZ(-5.0f);
    }
    //TODO: ADD/DECRESE SLICES
    
    //TODO: TRIGGER WIREFRAME
    //TODO: TRIGGER TEXTURE
    //TODO: TRIGGER NORMALS RENDER
    //TODO: TRIGGER NORMALS AVERAGE
}
