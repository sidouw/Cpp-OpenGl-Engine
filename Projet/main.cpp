//Chaouadi Sidahmed / Zegadi Mehdi Abdenour
#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "shader.h"
#include "camera.h"
#include "model.h"
#include "Shapes.h"
#include "light.h"
#include "actor.h"
#include "skyBox.h"

#include <iostream>
#include <vector>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
int lightTrace();
int trace();
unsigned int loadTexture(char const* path);
// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(3.0f, 1.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// world space positions of our cubes

std::vector<Actor> Cubes;
std::vector<PointLight> pointLights;

int main()
{

#pragma region initialization
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Projet Synthese", NULL, NULL);
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

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // build and compile our shader program
    // ------------------------------------
    Shader basicShader("basic.vs", "basic.fs");
    Shader lightShader("lighting.vs", "lighting.fs");
    Shader skyBoxShader("skyBox.vs", "skyBox.fs");

    Model wellModel("well.obj");
    Model grassModel("grass.obj");
    Model tree01Model("tree01.obj");
    Model tree02Model("tree02.obj");
    cube basicCube;


#pragma endregion


#pragma region load and create a texture for premitive box 
    // -------------------------
    unsigned int diffuseMap = loadTexture("tex.jpg");
    unsigned int specularMap = loadTexture("tex_spec.jpg");
    unsigned int normalMap = loadTexture("norm.jpg");
#pragma endregion

    // set texture units
    // ----------------------------------------------------------------
    lightShader.use();
    lightShader.setInt("material.diffuse", 0);
    lightShader.setInt("material.specular", 1);
    lightShader.setInt("material.normal", 2);

    lightShader.setFloat("material.shininess", 64.0f);

    skyBoxShader.use();
    skyBoxShader.setInt("skybox", 0);
    
    
    //create Cubes
    float of = .5;
    for (int i = 0; i < 5; i++) {

        std::srand(i * 25631);
        Actor c(&basicCube, true);
        c.position = glm::vec3(-4, 1.1, i + of - 2);
        c.rotationAxis = glm::vec3((float)(std::rand() % 10) / 10, (float)(std::rand() % 10) / 10, (float)(std::rand() % 10) / 10);
        Cubes.push_back(c);
        of += .5;
    }
    
    //create Point Lights
    of = .5;
    for (int i = 0; i < 3; i++) {

        PointLight l(lightShader, &basicCube, glm::vec3(-1, 2, i + of-2));;
        pointLights.push_back(l);
        of += 2;
    }

    DirectionalLight dirLight(lightShader, glm::vec3(-0.2f, -1.0f, -0.3f));
    dirLight.setDiffuse(glm::vec3(.3,.2,.2));
    
    Actor wellActor(&wellModel);
    Actor grassActor(&grassModel);
    vector<Actor> trees;


    //create Trees
    std::random_device rd;
    std::mt19937 gen(rd());
    int range = 40;
    int rangeMask = 5;
    uniform_int_distribution<> dist(-range, range);
    
    for (int i = 0; i < 400; i++) {
        int randomX  = dist(gen) % range;
        int randomZ  = dist(gen) % range;
        randomX = randomX < rangeMask&& randomX>-rangeMask && randomZ < rangeMask&& randomZ>-rangeMask ? randomX + rangeMask+3 : randomX;
        randomZ = randomX < rangeMask&& randomX>-rangeMask && randomZ < rangeMask&& randomZ>-rangeMask ? randomZ + rangeMask + 3 : randomZ;

        Model *m = std::rand()%2 ? &tree01Model :  &tree02Model;

        Actor tree(m, false, glm::vec3(randomX, 0,randomZ));;
        tree.rotationAxis = glm::vec3(0, 1, 0);

        tree.rotationAngle = glm::radians( (float) dist(gen));

        trees.push_back(tree);
        of += .5;
    }
    
    
    vector<std::string> faces
    {
        "SkyBox/right.jpg",
        "SkyBox/left.jpg",
        "SkyBox/top.jpg",
        "SkyBox/bottom.jpg",
        "SkyBox/front.jpg",
        "SkyBox/back.jpg",
    };
    
    SkyBox skyBox;
    skyBox.loadCubemap(faces);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightShader.use();
        lightShader.setVec3("viewPos", camera.Position);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightShader.setMat4("projection", projection);
        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        lightShader.setMat4("view", view);
 
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        // bind normal map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalMap);

       
        // render boxes
        for (unsigned int i = 0; i < Cubes.size(); i++)
        {
            //Random Cube Movement
            if (Cubes[i].shouldMove)
                Cubes[i].Offset.y = glm::sin((float)glfwGetTime() + i - .5);
            //Random Cube Rotation
            if (Cubes[i].shouldRotate)
            {
                std::srand(i * 25631);
                float angle = std::rand()%180 ;     
                Cubes[i].rotationAngle = glm::radians((float)glfwGetTime() * angle*10);
            }    
            Cubes[i].draw(lightShader);
        }


        // also draw the lamp object(s)
        basicShader.use();
        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);

        for (unsigned int i = 0; i < pointLights.size(); i++) {
            pointLights[i].draw(basicShader);
            if (pointLights[i].shouldMove)
                pointLights[i].updateLight();
            
            
        }
        // -------------------------------------------------------------------------------
        skyBoxShader.use();
        skyBoxShader.setMat4("projection", projection);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyBoxShader.setMat4("view", view);
        skyBox.draw(skyBoxShader);


        //Render scene
        grassActor.draw(lightShader);
        wellActor.draw(lightShader);
        
        for (size_t i = 0; i < trees.size(); i++)
        {
            trees[i].draw(lightShader);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.MovementSpeed = 4.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.MovementSpeed = 2.5f;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        int target = trace();
        if (target > -1) {
            Cubes[target].position = camera.Position + (camera.Front * glm::vec3(2.5) );
        }
            
    }
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        int target = lightTrace();
        if (target > -1) {
            pointLights[target].shouldMove = true;
            pointLights[target].position = camera.Position + (camera.Front * glm::vec3(2.5) );
        }
            
    }

    
          
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        int target = trace();
        if (target >= 0)
            Cubes[target].shouldRotate = !Cubes[target].shouldRotate;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        int target = trace();
        if (target >= 0)
            Cubes[target].shouldMove = !Cubes[target].shouldMove;
    }
}

int trace() {
    float minDot = -99;
    float minDistance = -99;
    for(int i = 0 ; i<Cubes.size();i++)
    {
        glm::vec3 sub = glm::normalize(Cubes[i].position - camera.Position);
        minDot = glm::dot(sub,camera.Front);
        minDistance = glm::distance(Cubes[i].position, camera.Position);
        
        if (minDistance < 3) {
            if (minDot > .9) {
                return i;
            }
        }
    }
  
    return -1;
}

int lightTrace() {
    float minDot = -99;
    float minDistance = -99;
    for(int i = 0 ; i<pointLights.size();i++)
    {
        glm::vec3 sub = glm::normalize(pointLights[i].position - camera.Position);
        minDot = glm::dot(sub,camera.Front);
        minDistance = glm::distance(pointLights[i].position, camera.Position);
        
        if (minDistance < 3) {
            if (minDot > .9) {
                return i;
            }
        }
    }
  
    return -1;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}