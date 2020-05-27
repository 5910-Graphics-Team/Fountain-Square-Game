#include <iostream>
#include <stdlib.h> // rand()
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Game-Engine/GameObject.h"
#include "Game-Engine/Shader.h"
#include "Game-Engine/Model.h"
#include "Game-Engine/LOGL_Camera.h"
#include "Audio-Engine/AudioEngine.h"
#include "Audio-Engine/FootstepController.h"
#include "GameData.h"
// custom game objects
#include "Game-Engine/Bird.h"
#include "Game-Engine/Harp.h"


void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

// camera
CharacterCamera camera(STARTING_PLAYER_LOCATION);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// audio engine
AudioEngine* audioEngine;
FootstepController* footstepController;

SoundInfo soundOneShot     (STINGER_1_GUITAR);
SoundInfo soundOneShot3D   (STINGER_3_HARP,    false, true, tranHarp.x,    tranHarp.y,     tranHarp.z);
SoundInfo soundLoop2D      (MUSIC_2,           true);
SoundInfo soundLoop3D      (SFX_LOOP_FOUNTAIN, true, true, tranFountain.x, tranFountain.y, tranFountain.z);

SoundInfo soundJapaneseTree(SFX_LOOP_TREE_BIRDS, true, true, tranTreeFir.x, tranTreeFir.y, tranTreeFir.z);
SoundInfo soundTree        (SFX_LOOP_TREE_BIRDS, true, true, tranWillowtree.x, tranWillowtree.y, tranWillowtree.z);

SoundInfo soundLoop3DMoving(SFX_LOOP_BIRD,     true, true, tranBirds.x,    tranBirds.y,    tranBirds.z);


static glm::mat4 getProjection() {
    return glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
}

// Performs the OpenGL calls to render a GameObject with a provided shader.
static void renderGameObject(GameObject &gameObject, Shader* shader) {
    // enable shader before setting uniforms
    shader->use();
    // view/projection transformations
    glm::mat4 projection = getProjection();
    glm::mat4 view = camera.GetViewMatrix();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    // render the loaded model
    shader->setMat4("model", gameObject.getModel());       
    gameObject.draw(shader);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // tell GLFW what to do with mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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
    

    /*
        Initialize game objects and add to list
    */
    GameObject* fountain     = new GameObject(OBJ_FOUNTAIN,      tranFountain,     scaleFountain,     rotFountain);
    //GameObject* backpack   = new GameObject(OBJ_BACKPACK,      tranBackpack,     scaleBackpack,     rotBackpack);
    GameObject* house        = new GameObject(OBJ_HOUSE,         tranHouse,        scaleHouse,        rotHouse);
    GameObject* rock         = new GameObject(OBJ_ROCK,          tranRock,         scaleRock,         rotRock);
    GameObject* ground       = new GameObject(OBJ_GROUND,        tranGround,       scaleGround,       rotGround);
    GameObject* treeFir      = new GameObject(OBJ_TREE,          tranTreeFir,      scaleTreeFir,      rotTreeFir);
	GameObject* grass        = new GameObject(OBJ_GRASS,         tranGrass,        scaleGrass,        rotGrass);
	GameObject* cooltree     = new GameObject(OBJ_COOLTREE,      tranCooltree,     scaleCooltree,     rotCooltree);
	GameObject* oak		     = new GameObject(OBJ_OAK,           tranPine,         scalePine,         rotPine);
	GameObject* house2       = new GameObject(OBJ_HOUSE2,        tranHouse2,       scaleHouse2,       rotHouse2);
	GameObject* japaneseTree = new GameObject(OBJ_JAPANESE_TREE, tranJapaneseTree, scaleJapaneseTree, rotJapaneseTree);
	GameObject* cottage      = new GameObject(OBJ_COTTAGE,       tranCottage,      scaleCottage,      rotCottage);
	GameObject* willowtree   = new GameObject(OBJ_WILLOWTREE,    tranWillowtree,   scaleWillowtree,   rotWillowtree);
	GameObject* well         = new GameObject(OBJ_WELL,          tranWell,         scaleWell,         rotWell);
    
    // List for all game objects
    std::vector<GameObject*> gameObjects;
    gameObjects.push_back(fountain);
    gameObjects.push_back(house);   
    gameObjects.push_back(ground);
    gameObjects.push_back(treeFir);
    gameObjects.push_back(rock);
	gameObjects.push_back(grass);
	gameObjects.push_back(cooltree);
	gameObjects.push_back(oak);
	gameObjects.push_back(house2);
	gameObjects.push_back(japaneseTree);
	gameObjects.push_back(cottage);
	gameObjects.push_back(willowtree);
	gameObjects.push_back(well);


    /*
        Initialize animatable game objects and add to list of game objects, and to another animation objects list
    */
    Bird* birds = new Bird(OBJ_BIRDS, tranBirds, scaleBirds, rotBirds);
    Harp* harp  = new Harp(OBJ_HARP, tranHarp, scaleHarp, rotHarp);
    
    gameObjects.push_back(birds);
    gameObjects.push_back(harp);

    // list for all animation objects which need to be updated each frame
    std::vector<Animation*> animationObjects;
    animationObjects.push_back(birds);
    animationObjects.push_back(harp);

    
    /*
        Initialize Audio Engine and Load sounds
    */
    audioEngine = new AudioEngine();
    audioEngine->init();

    audioEngine->loadSound(soundOneShot);
    audioEngine->loadSound(soundLoop2D);
    audioEngine->loadSound(soundOneShot3D);
    audioEngine->loadSound(soundLoop3D);
    audioEngine->loadSound(soundLoop3DMoving);
    audioEngine->loadSound(soundTree);
	audioEngine->loadSound(soundJapaneseTree);

    
    // load FMOD soundbanks
    audioEngine->loadFMODStudioBank(FMOD_SOUNDBANK_MASTER);
    audioEngine->loadFMODStudioBank(FMOD_SOUNDBANK_MASTER_STRINGS);
    audioEngine->loadFMODStudioBank(FMOD_SOUNDBANK_SFX);
    // load FMOD event and its parameters
    audioEngine->loadFMODStudioEvent(FMOD_EVENT_CHARACTER_FOOTSTEPS, PARAM_CHARACTER_FOOTSTEPS_SURFACE);
    audioEngine->loadFMODStudioEvent(FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE);
    audioEngine->loadFMODStudioEvent(FMOD_EVENT_2D_ONESHOT_EXPLOSION);

    // setup sound event controllers
    footstepController = new FootstepController(audioEngine);



    /*
        play inital soundscape
    */
    audioEngine->playSound(soundLoop3D);
    audioEngine->playSound(soundLoop2D);
    audioEngine->playSound(soundTree);
	audioEngine->playSound(soundJapaneseTree);
    
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    /* render loop */ 
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        // input
        // -----
        ProcessInput(window);

        // render
        // ------
        glClearColor(COLOR_SKY.x, COLOR_SKY.y, COLOR_SKY.z, COLOR_SKY.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // enable blended overwrite of color buffer
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // update animation objects with current frame
        for (int i = 0; i < animationObjects.size(); i++)
            animationObjects[i]->update(currentFrame);

        // render Game Objects
        for (int i = 0; i < gameObjects.size(); i++) 
            renderGameObject(*gameObjects[i], &ourShader);


        // set current player position
        audioEngine->set3DListenerPosition(camera.Position.x, camera.Position.y, camera.Position.z,
                                          camera.Front.y,    camera.Front.x,    camera.Front.z,
                                          camera.Up.y,       camera.Up.x,       camera.Up.z );
        

        //soundLoop3DMoving.set3DCoords(birds->getTranslation().x, birds->getTranslation().y, birds->getTranslation().z);
        //audioEngine->update3DSoundPosition(soundLoop3DMoving);

        audioEngine->update(); // per-frame FMOD update

        // Update location of 3D sounds
        //glm::vec3 newBirdTran = birds->getTranslation();
        //audioEngine->update3DSoundPosition(SFX_LOOP_BIRD, newBirdTran.x, newBirdTran.y, newBirdTran.z);


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

// audio timing
float key1LastTime = 0.0f, key2LastTime = 0.0f, key3LastTime = 0.0f, key4LastTime = 0.0f, key5LastTime = 0.0f;
float MIN_SOUND_KEY_RETRIGGER_TIME = 0.2f;

bool keyCanRetrigger(float currFrame, float lastTriggerFrame) {
    return currFrame - lastTriggerFrame >= MIN_SOUND_KEY_RETRIGGER_TIME;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // WASD Handling
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        footstepController->processFootstepKey(currentFrame);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
        footstepController->processFootstepKey(currentFrame);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
        footstepController->processFootstepKey(currentFrame);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        footstepController->processFootstepKey(currentFrame);
    }
   
    // Numbers 1-5 : Audio Processing Tests
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && keyCanRetrigger(currentFrame, key1LastTime)) {
        audioEngine->playSound(soundOneShot);
        key1LastTime = currentFrame;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS  && keyCanRetrigger(currentFrame, key2LastTime)) {
        audioEngine->soundIsPlaying(soundLoop2D) ?  audioEngine->stopSound(soundLoop2D) : audioEngine->playSound(soundLoop2D);
        key2LastTime = currentFrame;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS  && keyCanRetrigger(currentFrame, key3LastTime)) {
        audioEngine->playSound(soundOneShot3D);
        key3LastTime = currentFrame;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && keyCanRetrigger(currentFrame, key4LastTime)) {
        audioEngine->soundIsPlaying(soundLoop3D) ? audioEngine->stopSound(soundLoop3D) : audioEngine->playSound(soundLoop3D);
        key4LastTime = currentFrame;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && keyCanRetrigger(currentFrame, key5LastTime)) {
        audioEngine->eventIsPlaying(FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE) ?
			audioEngine->stopEvent(FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE) :
            audioEngine->playEvent(FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE);
        //audioEngine->playEvent(FMOD_EVENT_2D_ONESHOT_EXPLOSION);
        key5LastTime = currentFrame;
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
