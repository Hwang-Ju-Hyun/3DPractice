#include "Controls.h"
#include "Level.h"
#include "models.h"
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


    
    glm::vec3 camUp = Level::GetPtr()->GetCam();
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
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
    static bool wireframeMode = false;

    
    if (action == GLFW_PRESS && key == GLFW_KEY_M)
    {
        wireframeMode = !wireframeMode;
        glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
    }
    
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_Z)
        {          
            Model* cylinder = Level::GetPtr()->FindModel("cylinder");
            Model* cone = Level::GetPtr()->FindModel("cone");

            if (cylinder)
            {
                cylinder->slices++;
                cylinder->UpdateSlices();
            }
            if (cone)
            {
                cone->slices++;
                cone->UpdateSlices();
            }
        }
        else if (key == GLFW_KEY_X)
        {            
            Model* cone = Level::GetPtr()->FindModel("cone");
            Model* cylinder = Level::GetPtr()->FindModel("cylinder");

            if (cylinder)
            {
                cylinder->slices--;
                cylinder->UpdateSlices();
            }
            if (cone)
            {
                cone->slices--;
                cone->UpdateSlices();
            }
        }
    }
    //TODO: TRIGGER TEXTURE
    if (action == GLFW_PRESS && key == GLFW_KEY_T)
    {
        Level::GetPtr()->b_tex = !(Level::GetPtr()->b_tex);
    }
    //TODO: TRIGGER NORMALS RENDER
    if (action == GLFW_PRESS && key == GLFW_KEY_N)
    {        
        Level::GetPtr()->render_normal = !(Level::GetPtr()->render_normal);
        Level::GetPtr()->b_normal = !(Level::GetPtr()->b_normal);
    }
    //TODO: TRIGGER NORMALS AVERAGE
}
