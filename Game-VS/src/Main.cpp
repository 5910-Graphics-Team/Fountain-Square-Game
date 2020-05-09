#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Model.h"
#include "LOGL_Camera.h"
#include "Audio-Engine/AudioEngine.h"
#include "Misc.h"
#include "GLXtras.h"
#include <iostream>

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

// screen settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// audio timing
float stinger1LastTime = 0.0f, stinger2LastTime = 0.0f, stinger3LastTime = 0.0f;
float MIN_STINGER_RETRIGGER_TIME = 0.5f;

// audio engine
AudioEngine audioEngine;

// audio assets
const char* MUSIC        = "res/sound/Medieval Village Full Theme Demo_2.5.1.3.wav";
const char* STINGER_1    = "res/sound/Medieval Village_Stinger1 Guitar_2.5.1.3.wav";
const char* STINGER_2    = "res/sound/Medieval Village_Stinger2 Guitar_2.5.1.3.wav";
const char* STINGER_3    = "res/sound/Medieval Village_Stinger3 Harp_2.5.1.3.wav";
const char* FOUNTAIN_SFX = "res/sound/Fountain_Loop.wav";


// Container for a Model and its default translation,scale, and rotation values. 
class GameObject {
private:
    Model model; // TODO could allocate memory for this (make pointer)
    glm::vec3 trans, scale, rotAngs;
public:
    GameObject(const char* filepath) : GameObject(filepath, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f)) {}
    GameObject(const char* filepath, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot) : model(filepath), trans(defTrans), scale(defScale), rotAngs(defRot) {   }

    void draw(Shader* shader) {
        model.Draw(*shader);
    }

    glm::vec3 getTranslation() {
        return trans;
    }
    glm::vec3 getScale() {
        return scale;
    }
    glm::vec3 getRotationAngles() {
        return rotAngs;
    }
    glm::mat4 getModel() {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, trans);
        m = glm::rotate(m, glm::radians(rotAngs.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotation x
        m = glm::rotate(m, glm::radians(rotAngs.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotation y
        m = glm::rotate(m, glm::radians(rotAngs.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotation z 
        m = glm::scale(m, scale);	  // scale object
        return m;
    }
};

static glm::mat4 getProjection() {
    return glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
}

// Performs the OpenGL calls to render a GameObject with a provided shader.
static void renderGameObject(GameObject* gameObject, Shader* shader) {
    // enable shader before setting uniforms
    shader->use();

    // view/projection transformations
    glm::mat4 projection = getProjection();
    glm::mat4 view = camera.GetViewMatrix();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    // render the loaded model 
    // TODO refactor GameObject to do this itself in draw()
    shader->setMat4("model", gameObject->getModel());
    gameObject->draw(shader);
}

float currentFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fountain Game", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // tell GLFW what to do with mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader ourShader("res/shaders/1.model_loading.vs", "res/shaders/1.model_loading.fs");
    
    std::vector<GameObject> gameObjects;
    
    // create and load models with default trans/scale/rotation  (TODO move object 'default' params to config file?)
    glm::vec3 backpackTran(0.5f,   -1.2f,  0.0f),   backpackScale(0.5f),   backpackRot(0.0f);
    glm::vec3 houseTran   (-15.0f, -8.0f, -20.0f),  houseScale   (0.42f),  houseRot(0.0f, 70.0f, 0.0f);
    glm::vec3 groundTran  (50.0f,  -8.0f, -200.0f), groundScale  (20.0f),  groundRot(90.0f, 0.0f, 0.0f);
    glm::vec3 treeFirTran (0.0f,  -8.0f,  -10.0f),  treeFirScale (0.008f), treeFirRot(0.0f);
    glm::vec3 rockTran    (-8.0f, -8.0f, -15.0f),   rockScale(0.34f),      rockRot(0.0f);
    glm::vec3 fountainTran(-10.0f, -7.9f, -5.0f),   fountainScale(0.07f),  fountainRot(0.0f);
    // Harp location based on fountain location
    glm::vec3 harpTran(fountainTran.x, fountainTran.y + 4.0f, fountainTran.z), harpScale(0.003f), harpRot(0.0f, 120.0f, 0.0f); //(-2.0f, 0.0f, -10.0f)

    GameObject fountain("res/objects/fountains/fountainOBJ/fountain.obj", fountainTran, fountainScale, fountainRot);
    GameObject backpack("res/LearnOpenGL/objects/backpack/backpack.obj",  backpackTran, backpackScale, backpackRot);
    GameObject house("res/objects/Monster House/Monster House.obj",       houseTran,    houseScale,    houseRot);
    GameObject rock("res/objects/ground/rock/rock.obj",                   rockTran,     rockScale,     rockRot);
    GameObject ground("res/objects/ground/groundModel/ground.obj",        groundTran,   groundScale,   groundRot);
    GameObject treeFir("res/objects/flora/trees/fir/fir.obj",             treeFirTran,  treeFirScale,  treeFirRot);
    GameObject harp("res/objects/instruments/harp/3d-model.obj",          harpTran,     harpScale,     harpRot);
    //GameObject stoneFloor("res/objects/ground/stone-ground/test/StoneFloor_Sample.obj", houseTran, glm::vec3(0.05f), houseRot);

    gameObjects.push_back(fountain);
    gameObjects.push_back(backpack);
    gameObjects.push_back(house);   
    //gameObjects.push_back(stoneFloor);
    gameObjects.push_back(ground);
    gameObjects.push_back(treeFir);
    gameObjects.push_back(harp);
    gameObjects.push_back(rock);
    
    // load non-looping sound effects
    audioEngine.loadSoundFile(STINGER_1,    false, false);
    audioEngine.loadSoundFile(STINGER_2,    false, false);
    // load looping sfx and main music
    audioEngine.loadSoundFile(STINGER_3, true, true);
    audioEngine.loadSoundFile(MUSIC,        false, true); 
    audioEngine.loadSoundFile(FOUNTAIN_SFX, true,  true);
    

    // play inital soundscape
    //audioEngine.play3DSound(FOUNTAIN_SFX, fountainTran.x, fountainTran.y, fountainTran.z); 
    //audioEngine.play3DSound(STINGER_3,    harpTran.x,     harpTran.y,     harpTran.z);
    //audioEngine.playSoundFile(MUSIC);
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //std::cout << "current frame: " << currentFrame << "\n";
        
        // input
        // -----
        ProcessInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Update listener position
        audioEngine.set3DListenerPosition(camera.Position.x, camera.Position.y, camera.Position.z,
                                          camera.Front.x, camera.Front.y, camera.Front.z, 
                                          camera.Up.x,    camera.Up.y,    camera.Up.z
                                          
        );

        // render Game Objects
        for (int i = 0; i < gameObjects.size(); i++) 
            renderGameObject(&gameObjects[i], &ourShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow* window)
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
   
    // Audio Processing
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && currentFrame - stinger1LastTime >= MIN_STINGER_RETRIGGER_TIME) {
        audioEngine.playSoundFile(STINGER_1);
        stinger1LastTime = currentFrame;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS  && currentFrame - stinger2LastTime >= MIN_STINGER_RETRIGGER_TIME) {
        audioEngine.playSoundFile(STINGER_2);
        stinger2LastTime = currentFrame;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS  && currentFrame - stinger3LastTime >= MIN_STINGER_RETRIGGER_TIME) {
        audioEngine.playSoundFile(STINGER_3);
        stinger3LastTime = currentFrame;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
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

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
