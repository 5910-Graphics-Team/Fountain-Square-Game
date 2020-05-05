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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// audio engine
AudioEngine audioEngine;


static glm::mat4 getProjection() {
    return glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
}

// Container for a Model and its default translation,scale, and rotation values. 
class GameObject {
private:
    Model model; // TODO could allocate memory for this (make pointer)
    glm::vec3 trans, scale, rotAngs;
public:
    GameObject(const char* filepath) : GameObject(filepath, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f)) {}
    GameObject(const char* filepath, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot) : model(filepath), trans(defTrans), scale(defScale), rotAngs(defRot) {
        std::cout << "Created model for " << filepath << "\n";
    }
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
        m = glm::translate(m, trans); // translate it down so it's at the center of the scene
        m = glm::rotate(m, glm::radians(rotAngs.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotation x
        m = glm::rotate(m, glm::radians(rotAngs.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotation y
        m = glm::rotate(m, glm::radians(rotAngs.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotation z 
        m = glm::scale(m, scale);	  // scale object
        return m;
    }
};


// Performs the OpenGL calls to render a GameObject with a provided shader.
static void renderGameObject(GameObject* gameObject, Shader* shader) {
    // enable shader before setting uniforms
    shader->use();

    // view/projection transformations
    glm::mat4 projection = getProjection();//glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    // render the loaded model 
    // TODO refactor GameObject to do this itself in draw()
    shader->setMat4("model", gameObject->getModel());
    gameObject->draw(shader);
}



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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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
    Shader ourShader("res/LearnOpenGL/shaders/1.model_loading.vs", "res/LearnOpenGL/shaders/1.model_loading.fs");
    
    std::vector<GameObject> gameObjects;
    
    // create and load models with default trans/scale/rotation  (TODO move object 'default' params to config file?)
    glm::vec3 fountainTran(-30.0f, -12.5f, -70.0f), fountainScale(0.5f), fountainRot(0.0f);
    glm::vec3 backpackTran(0.5f, -1.2f, 0.0f),      backpackScale(0.5f), backpackRot(0.0f);
    glm::vec3 groundTran(50.0f, -17.0f, -200.0f),   groundScale(20.0f),  groundRot(90.0f, 0.0f, 0.0f);
    
    GameObject fountain("res/objects/fountains/fountainOBJ/fountain.obj", fountainTran, fountainScale, fountainRot);
    GameObject backpack("res/LearnOpenGL/objects/backpack/backpack.obj",  backpackTran, backpackScale, backpackRot);
    GameObject groundObj("res/objects/ground/ground.obj",                 groundTran,   groundScale,   groundRot);
    
    gameObjects.push_back(fountain);
    gameObjects.push_back(backpack);
    gameObjects.push_back(groundObj);

    // create sound effects/music
    const char* MUSIC        = "res/sound/Medieval Village2.5_Loop1_ImplementationDemo.wav";
    const char* FOUNTAIN_SFX = "res/sound/Fountain_Loop.wav";

    audioEngine.loadSoundFile(MUSIC, false, true);
    audioEngine.loadSoundFile(FOUNTAIN_SFX, true, true);

    audioEngine.play3DSound(FOUNTAIN_SFX, fountainTran.x, fountainTran.y, fountainTran.z); //, -10.0f, 0.0f, 0.0f);
    //audioEngine.playSoundFile(MUSIC);
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
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
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        audioEngine.set3DListenerPosition(lastX, lastY, 0.0f);
        // render objects
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
