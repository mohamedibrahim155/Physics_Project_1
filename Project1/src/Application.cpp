


#include "Shader.h"
#include <glad/glad.h>


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Application.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Camera.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "VertexArray.h"
#include  "model.h"
#include "material.h"
#include "Light.h"
#include "Transform.h"
#include"LightManager.h"
#include <random>

#include"PhysicsEngine.h"

bool switchCamera = false;
bool switchToPointLight = false;

glm::vec3 currentLight(1.0f, 0.0f, 1.0f);
glm::vec3 spotlightPos(1.0f, 0.0f, 1.0f);
glm::vec3 PointLightPos(-1.0f, 0.0f, 1.0f);

bool isAnimationKeyPressed = false;
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        switchCamera = !switchCamera;
        std::cout << "Value of swtich camera : " << switchCamera << std::endl;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {

        switchToPointLight = !switchToPointLight;
        std::cout << "Value of swtich pointLight : " << switchToPointLight << std::endl;
        if (switchToPointLight)
        {
            currentLight = PointLightPos;
        }
        else
        {
            currentLight = spotlightPos;

        }
    }


    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
     

    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        isAnimationKeyPressed = !isAnimationKeyPressed;
    }

}



const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;


Camera camera(glm::vec3(0, 2, 20));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::vector<Model*> loadedModels;
std::vector<Model*> lightDebugModels;
std::vector<Model*> animatingDoorModels;
std::vector<Model*> starsModels;
Model* testingModel;
bool isTestingModel = false;
void CheckingValues(Model* testModel, float x, float y, float z);
bool isfirstDoorOpen = false;


int randomNumberGen(int min, int max);
void LoadStarsModel();
int main()
{



    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "KAIZOKU", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    glEnable(GL_DEPTH_TEST);


    Shader defaultShader("Shaders/Light_VertexShader.vert", "Shaders/Light_FragmentShader.frag");


    Shader lightSource("Shaders/lighting.vert", "Shaders/lighting.frag");


#pragma region Model Loading
    Model* sphere = new Model((char*)"Models/Ball/pokeball.obj", false, false);
    sphere->transform.SetTranslation(glm::vec3(0, 13, 0));
    sphere->transform.SetScale(glm::vec3(0.5f));
    loadedModels.push_back(sphere);

    Model* sphere2 = new Model((char*)"Models/Ball/pokeball.obj", false, false);
  /*  sphere2->transform.SetTranslation(glm::vec3(1, 0.0f, 0));
    sphere2->transform.SetScale(glm::vec3(0.5f));
    loadedModels.push_back(sphere2);*/




    Model* Floor = new Model((char*)"Models/Floor/Floorfbx.fbx", false, false);
    Floor->transform.SetTranslation(glm::vec3(0, -5.0f, 0));
    Floor->transform.SetRotation(90,glm::vec3(1, 0, 0));
    Floor->transform.SetScale(glm::vec3(5));
    Floor->isWireFrame = true;
    loadedModels.push_back(Floor);

    Model* Floor2 = new Model((char*)"Models/Floor/Floorfbx.fbx", false, false);
    Floor2->transform.SetTranslation(glm::vec3(0, -2.0f, -8.0f));
    Floor2->transform.SetRotation(135, glm::vec3(1, 0, 0));
  //  Floor2->transform.SetRotation(45, glm::vec3(1, 0, 0));
    Floor2->transform.SetScale(glm::vec3(5));
    Floor2->isWireFrame = true;
    loadedModels.push_back(Floor2);

    Model* Floor3 = new Model((char*)"Models/Floor/Floorfbx.fbx", false, false);
    Floor3->transform.SetTranslation(glm::vec3(0, -2.0f, 8));
    Floor3->transform.SetRotation(-135, glm::vec3(1, 0, 0));
    //  Floor2->transform.SetRotation(45, glm::vec3(1, 0, 0));
    Floor3->transform.SetScale(glm::vec3(5));
    Floor3->isWireFrame = true;
    loadedModels.push_back(Floor3);

    Model* Floor4 = new Model((char*)"Models/Floor/Floorfbx.fbx", false, false);
    Floor4->transform.SetTranslation(glm::vec3(5, 0, 0));
    Floor4->transform.SetRotation(-90, glm::vec3(0, 1, 0));
    Floor4->transform.SetScale(glm::vec3(5));
    Floor4->isWireFrame = true;
    loadedModels.push_back(Floor4);

    Model* Floor5 = new Model((char*)"Models/Floor/Floorfbx.fbx", false, false);
    Floor5->transform.SetTranslation(glm::vec3(-5, 0, 0));
    Floor5->transform.SetRotation(-90, glm::vec3(0, 1, 0));
    Floor5->transform.SetScale(glm::vec3(5));
    Floor5->isWireFrame = true;
    loadedModels.push_back(Floor5);
#pragma endregion


    PhysicsObject* spherephys= new PhysicsObject(sphere);
    spherephys->physicsType = SPHERE;
    spherephys->Initialize(false, true, ObjectMode::DYNAMIC);
    
    

    PhysicsObject* spherephys2 = new PhysicsObject(sphere2);
    spherephys2->physicsType = SPHERE;
    spherephys2->Initialize(false, true, ObjectMode::DYNAMIC);
    spherephys2->SetMass(0.5f);
    PhysicsObject* floorPhys = new PhysicsObject(Floor);
    floorPhys->physicsType = TRIANGLE;
    floorPhys->Initialize(false, true, ObjectMode::STATIC);

    PhysicsObject* floorPhys2 = new PhysicsObject(Floor2);
    floorPhys2->physicsType = TRIANGLE;
    floorPhys2->Initialize(false, true, ObjectMode::STATIC);
    PhysicsObject* floorPhys3 = new PhysicsObject(Floor3);
    floorPhys3->physicsType = TRIANGLE;
    floorPhys3->Initialize(false, true, ObjectMode::STATIC);
    PhysicsObject* floorPhys4 = new PhysicsObject(Floor4);
    floorPhys4->physicsType = TRIANGLE;
    floorPhys4->Initialize(false, true, ObjectMode::STATIC);
    PhysicsObject* floorPhys5 = new PhysicsObject(Floor5);
    floorPhys5->physicsType = TRIANGLE;
    floorPhys5->Initialize(false, true, ObjectMode::STATIC);

    Material material( 128.0f);


    
    Renderer render;

    PhysicsEngine engine;

    for (size_t i = 0; i < 3; i++)
    {
        Model* sphere3 = new Model(sphere2);
        sphere3->transform.SetTranslation(glm::vec3(1.5f * i, 0.0f, 0));
        sphere3->transform.SetScale(glm::vec3(0.5f));
        loadedModels.push_back(sphere3);
        PhysicsObject* spherephysclone = new PhysicsObject(sphere3);
        spherephysclone->physicsType = SPHERE;
        spherephysclone->Initialize(false, true, ObjectMode::DYNAMIC);
        engine.AddPhysicsObjects(spherephysclone);
    }
    for (size_t i = 1; i < 4; i++)
    {
        Model* sphere3 = new Model(sphere2);
        sphere3->transform.SetTranslation(glm::vec3(1.5f * i, 2, 0.5f));
        sphere3->transform.SetScale(glm::vec3(0.5f));
        loadedModels.push_back(sphere3);
        PhysicsObject* spherephysclone = new PhysicsObject(sphere3);
        spherephysclone->physicsType = SPHERE;
        spherephysclone->Initialize(false, true, ObjectMode::DYNAMIC);
        engine.AddPhysicsObjects(spherephysclone);
    }

    for (size_t i = 1; i < 5; i++)
    {
        Model* sphere3 = new Model(sphere2);
        sphere3->transform.SetTranslation(glm::vec3(0, 0.0f, -1.5f * i));
        sphere3->transform.SetScale(glm::vec3(0.5f));
        loadedModels.push_back(sphere3);
        PhysicsObject* spherephysclone = new PhysicsObject(sphere3);
        spherephysclone->physicsType = SPHERE;
        spherephysclone->Initialize(false, true, ObjectMode::DYNAMIC);
        engine.AddPhysicsObjects(spherephysclone);
    }

    //for (size_t i = 1; i < 5; i++)
    //{
    //    Model* sphere3 = new Model(sphere2);
    //    sphere3->transform.SetTranslation(glm::vec3(0, 0.0f, 1.5f * i));
    //    sphere3->transform.SetScale(glm::vec3(0.5f));
    //    loadedModels.push_back(sphere3);
    //    PhysicsObject* spherephysclone = new PhysicsObject(sphere3);
    //    spherephysclone->physicsType = SPHERE;
    //    spherephysclone->Initialize(false, true, ObjectMode::DYNAMIC);
    //    spherephysclone->SetMass(0.5f);
    //    engine.AddPhysicsObjects(spherephysclone);
    //}


    engine.AddPhysicsObjects(spherephys);
   // engine.AddPhysicsObjects(spherephys2);
    engine.AddPhysicsObjects(floorPhys);
    engine.AddPhysicsObjects(floorPhys2);
    engine.AddPhysicsObjects(floorPhys3);
    engine.AddPhysicsObjects(floorPhys4);
    engine.AddPhysicsObjects(floorPhys5);

#pragma region Mulitple LightHandler




    LightManager lightManager;

    Model* directionLightModel = new Model((char*)"Models/DefaultSphere/Sphere_1_unit_Radius.ply", false, true);
    lightDebugModels.push_back(directionLightModel);
  



    
    Light directionLight;
    directionLight.lightType = LightType:: DIRECTION_LIGHT;
    directionLight.lightModel = directionLightModel;
    directionLight.ambient =  glm::vec3(0.71f);
    directionLight.specular = glm::vec3(0.71f);
    directionLight.diffuse =  glm::vec3(0.71f);



    lightManager.AddNewLight(directionLight);


    //lightManager.AddNewLight(spotLight);
     lightManager.SetUniforms(defaultShader.ID);
#pragma endregion
    


    bool isWireFrame = false;

    double lastTime = glfwGetTime();

    float updatedXPos;
    float updatedYPos;
    float updatedZPos;
    char inputBufferX[256]; 
    char inputBufferY[256]; 
    char inputBufferZ[256]; 


    float currentFrame = static_cast<float>(glfwGetTime());
    lastFrame = currentFrame;
    while (!glfwWindowShouldClose(window))
    {


        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

       render.Clear();
       // glClearColor(0.7f, 0.5f, 0.5f, 0.5f);
        glm::mat4 _projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 _view = camera.GetViewMatrix();
        glm::mat4 _lightModel = glm::mat4(1.0f);


        lightSource.use();
        lightSource.setMat4("projection", _projection);
        lightSource.setMat4("view", _view);

        for (size_t i = 0; i < lightDebugModels.size(); i++)
        {
            //lightDebugModels[i]->transform.scale =glm::vec3(0.5f); // setting scale of the light object sphere 
           // lightDebugModels[i]->Draw(lightSource);
        }



        defaultShader.use();
        lightManager.UpdateUniformValues(defaultShader.ID);
        material.SetMaterialProperties(defaultShader);
         // view/projection transformations
         glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
         glm::mat4 view2 = camera.GetViewMatrix();
         defaultShader.setMat4("projection", projection2);
         defaultShader.setMat4("view", view2);
         //drawing all the models
         for (size_t i = 0; i < loadedModels.size(); i++)
         {
            
             loadedModels[i]->Draw(defaultShader);
         }
        


         engine.Update(deltaTime);

         if (isWireFrame)
         {
             glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
         }
         else
         {
             glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

         }
        glfwSwapBuffers(window);
        glfwPollEvents();

    }


    glfwTerminate();
    return 0;


}




int randomNumberGen(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max - 1);
    int random_number = dis(gen);
    return random_number;
}


void CheckingValues(Model* testModel, float x, float y, float z)
{
    if (isTestingModel)
    {
        testModel->transform.position = glm::vec3(x, y, z);

    }
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraMoveSpeed = 5.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {

        if (switchCamera)
        {
            float velocity = camera.MovementSpeed * deltaTime;
            currentLight += camera.Front * velocity;
        }
        else
        {
            camera.ProcessKeyboard(FORWARD, deltaTime* cameraMoveSpeed);
        }


    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {

        if (switchCamera)
        {
            float velocity = camera.MovementSpeed * deltaTime;
            currentLight -= camera.Front * velocity;
        }
        else
        {
            camera.ProcessKeyboard(BACKWARD, deltaTime* cameraMoveSpeed);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if (switchCamera)
        {
            float velocity = camera.MovementSpeed * deltaTime;
            currentLight -= camera.Right * velocity;
        }
        else
        {
            camera.ProcessKeyboard(LEFT, deltaTime* cameraMoveSpeed);

        }

    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if (switchCamera)
        {
            float velocity = camera.MovementSpeed * deltaTime;
            currentLight += camera.Right * velocity;

        }
        else
        {
            camera.ProcessKeyboard(RIGHT, deltaTime* cameraMoveSpeed);

        }

    }

    if (switchToPointLight)
    {
        PointLightPos = currentLight;
    }
    else
    {
        spotlightPos = currentLight;
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}








