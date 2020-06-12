/**
 * @file Main.cpp
 * Group: Ross Hoyt, Nazneen Tamboli, Sonali D'Souza, Ruoyang Qiu 
 * Class: CPSC 5910 Graphics/Game Project
 * Term:  SeattleU SQ 2020
 * 
 * This file provides the main program entry point and manages application/gameplay state.
 */
#include <iostream>
#include <stdlib.h> // rand()
#include <memory>   // shared_ptr
#include <thread>   // std::thread
#include <chrono>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Game-Engine/GameObject.h"
#include "Game-Engine/Shader.h"
#include "Game-Engine/Model.h"
#include "Game-Engine/CharacterCamera.h"
#include "Audio-Engine/AudioEngine.h"
#include "Audio-Engine/FootstepSoundController.h"
#include "Audio-Engine/CoinChallengeSoundController.h"
#include "GameData.h"
// custom game objects
#include "Game-Engine/Bird.h"
#include "Game-Engine/Harp.h"
#include "Game-Engine/AsteroidRing.h"
#include "Game-Engine/Coin.h"
#include "Game-Engine/Grass.h"
#include "Game-Engine/NPC.h"

// GLFW callbacks
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

// timing globals
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

// Character/camera data
CharacterCamera camera(STARTING_PLAYER_LOCATION);
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// Lists for all game objects
std::vector<GameObject*> gameObjects;
std::vector<Animation*> animationObjects;
std::vector<InstancedObject*> instancedObjects;
std::vector<Coin*> coins;

// Audio Engine
std::shared_ptr<AudioEngine> audioEngine;

// Sound Controllers
FootstepSoundController* footstepController;
CoinChallengeSoundController* coinSoundController;

/**
 * Method used to make all coins appear on the game map
 */
void resetCoins() {
	for (Coin* coin : coins) coin->setDestroyed(false);
}

/**
 * Method used by helper thread to display coins after waiting for dialog sound to finish
 */
void waitForDialogueCompletion(unsigned int lengthMS) {
	//std::cout << "In thread, waiting for  "<< lengthMS << " ms\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(lengthMS));
	resetCoins();
	coinSoundController->startScore();

}

/**
 * Gets the current projection matrix based on screen dimensions and zoom amount
 */
static glm::mat4 getProjection() {
	return glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
}

/**
 *  Performs the OpenGL calls to render a GameObject with a provided shader.
 */
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

/**
 * Main program entry point which contains the OpenGL Loop.
 */
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fountain Game", NULL, NULL);
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
	Shader gameObjectShader("res/shaders/1.model_loading.vs", "res/shaders/1.model_loading.fs");
	Shader* instancedObjectShader = new Shader("res/shaders/instanced_model_loading.vs", "res/shaders/instanced_model_loading.fs");

	/*
		Initialize game objects and add to list
	*/
	GameObject* fountain = new GameObject(OBJ_FOUNTAIN, tranFountain, scaleFountain, rotFountain);
	GameObject* house = new GameObject(OBJ_HOUSE, tranHouse, scaleHouse, rotHouse);
	GameObject* rock = new GameObject(OBJ_ROCK, tranRock, scaleRock, rotRock);
	GameObject* ground = new GameObject(OBJ_GROUND, tranGround, scaleGround, rotGround);
	//GameObject* grass = new GameObject(OBJ_GRASS, tranGrass, scaleGrass, rotGrass);
	GameObject* cooltree = new GameObject(OBJ_COOLTREE, tranCooltree, scaleCooltree, rotCooltree);
	GameObject* oak = new GameObject(OBJ_OAK, tranPine, scalePine, rotPine);
	GameObject* house2 = new GameObject(OBJ_HOUSE2, tranHouse2, scaleHouse2, rotHouse2);
	//GameObject* japaneseTree = new GameObject(OBJ_JAPANESE_TREE, tranJapaneseTree, scaleJapaneseTree, rotJapaneseTree);
	GameObject* cottage = new GameObject(OBJ_COTTAGE, tranCottage, scaleCottage, rotCottage);
	GameObject* cottage1 = new GameObject(OBJ_COTTAGE, tranCottage1, scaleCottage1, rotCottage1);
	GameObject* cottage2 = new GameObject(OBJ_COTTAGE, tranCottage2, scaleCottage2, rotCottage2);
	GameObject* willowtree = new GameObject(OBJ_WILLOWTREE, tranWillowtree, scaleWillowtree, rotWillowtree);
	GameObject* well = new GameObject(OBJ_WELL, tranWell, scaleWell, rotWell);
	GameObject* townhouse = new GameObject(OBJ_TOWNHOUSE, tranGreenPine, scaleGreenPine, rotGreenPine);
	GameObject* townhouse1 = new GameObject(OBJ_TOWNHOUSE, tranGreenPine1, scaleGreenPine1, rotGreenPine1);
	GameObject* fir1 = new GameObject(OBJ_OAK, tranfir1, scalefir1, rotfir1);
	GameObject* fir2 = new GameObject(OBJ_OAK, tranfir2, scalefir2, rotfir2);
	GameObject* fir3 = new GameObject(OBJ_OAK, tranfir3, scalefir3, rotfir3);
	GameObject* house3 = new GameObject(OBJ_HOUSE3, tranHouse, scaleHouse, rotHouse);
	GameObject* backhouse2 = new GameObject(OBJ_HOUSE3, tranHouseback2, scaleHouseback2, rotHouseback2);
	GameObject* japaneseTree2 = new GameObject(OBJ_JAPANESE_TREE2, tranJapaneseTree2, scaleJapaneseTree2, rotJapaneseTree2);
	GameObject* japaneseTree3 = new GameObject(OBJ_JAPANESE_TREE2, tranJapaneseTree3, scaleJapaneseTree2, rotJapaneseTree2);
	GameObject* fir4 = new GameObject(OBJ_TREE_BUSH, tranfir4, scalefir4, rotfir4);
	GameObject* fir5 = new GameObject(OBJ_OAK, tranfir5, scalefir5, rotfir5);
	GameObject* fir6 = new GameObject(OBJ_OAK, tranfir6, scalefir6, rotfir6);
	GameObject* fir7 = new GameObject(OBJ_OAK, tranfir7, scalefir7, rotfir7);
	GameObject* fir8 = new GameObject(OBJ_OAK, tranfir8, scalefir8, rotfir8);
	GameObject* fir9 = new GameObject(OBJ_OAK, tranfir9, scalefir9, rotfir9);
	GameObject* fir10 = new GameObject(OBJ_OAK, tranfir10, scalefir9, rotfir9);
	GameObject* fir11 = new GameObject(OBJ_OAK, tranfir11, scalefir9, rotfir9);
	GameObject* fir12 = new GameObject(OBJ_OAK, tranfir12, scalefir9, rotfir9);
	GameObject* house4 = new GameObject(OBJ_HOUSE4, tranHouse4, scaleHouse4, rotHouse4);
	GameObject* house5 = new GameObject(OBJ_HOUSE4, tranHouse5, scaleHouse4, rotHouse5);
	GameObject* tree_bush = new GameObject(OBJ_TREE_BUSH, tranbush, scalebush, rotbush);
	GameObject* tree_bush_1 = new GameObject(OBJ_TREE_BUSH, tranbush1, scalebush1, rotbush1);
	GameObject* tree_bush_2 = new GameObject(OBJ_TREE_BUSH, tranbush2, scalebush2, rotbush2);
	GameObject* backfir1 = new GameObject(OBJ_OAK, tranfirback1, scalefirback1, rotfirback1);
	GameObject* backfir2 = new GameObject(OBJ_OAK, tranfirback2, scalefirback2, rotfirback2);
	GameObject* backfir3 = new GameObject(OBJ_OAK, tranfirback3, scalefirback3, rotfirback3);
	GameObject* backfir5 = new GameObject(OBJ_OAK, tranfirback5, scalefirback5, rotfirback5);
	GameObject* backfir6 = new GameObject(OBJ_OAK, tranfirback6, scalefirback6, rotfirback6);
	GameObject* backfir7 = new GameObject(OBJ_OAK, tranfirback7, scalefirback7, rotfirback7);
	GameObject* backfir8 = new GameObject(OBJ_OAK, tranfirback8, scalefirback8, rotfirback8);
	GameObject* backfir9 = new GameObject(OBJ_OAK, tranfirback9, scalefirback9, rotfirback9);
	GameObject* treeline = new GameObject(OBJ_TREE_LINE, trantreeline, scaletreeline, rottreeline);
	GameObject* treeline1 = new GameObject(OBJ_TREE_LINE, trantreeline1, scaletreeline1, rottreeline1);
	GameObject* treeline2 = new GameObject(OBJ_TREE_LINE, trantreeline2, scaletreeline2, rottreeline2);
	GameObject* treeline3 = new GameObject(OBJ_TREE_LINE, trantreeline3, scaletreeline3, rottreeline3);
	GameObject* treeline4 = new GameObject(OBJ_TREE_LINE, trantreeline4, scaletreeline4, rottreeline4);
	GameObject* treeline5 = new GameObject(OBJ_TREE_LINE, trantreeline5, scaletreeline4, rottreeline4);
	GameObject* tree_bush_4 = new GameObject(OBJ_TREE_BUSH, tranbushback4, scalebushback4, rotbushback4);
	GameObject* tree_bush_5 = new GameObject(OBJ_TREE_BUSH, tranbushback5, scalebushback5, rotbushback5);
	GameObject* tree_bush_6 = new GameObject(OBJ_OAK, tranbush6, scalebush6, rotbush6);
	
	// List for all game objects
	gameObjects.push_back(fountain);
	gameObjects.push_back(house3);
	gameObjects.push_back(ground);
	gameObjects.push_back(rock);
	gameObjects.push_back(cooltree);
	gameObjects.push_back(oak);
	gameObjects.push_back(house2);
	gameObjects.push_back(cottage);
	gameObjects.push_back(cottage1);
	gameObjects.push_back(cottage2);
	gameObjects.push_back(willowtree);
	gameObjects.push_back(well);
	gameObjects.push_back(townhouse);
	gameObjects.push_back(townhouse1);
	gameObjects.push_back(japaneseTree2);
	gameObjects.push_back(japaneseTree3);
	gameObjects.push_back(fir1);
	gameObjects.push_back(fir2);
	gameObjects.push_back(fir3);
	gameObjects.push_back(fir4);
	gameObjects.push_back(fir5);
	gameObjects.push_back(fir6);
	gameObjects.push_back(fir7);
	gameObjects.push_back(fir8);
	gameObjects.push_back(fir9);
	gameObjects.push_back(fir10);
	gameObjects.push_back(fir11);
	gameObjects.push_back(fir12);
	gameObjects.push_back(house4);
	gameObjects.push_back(house5);
	gameObjects.push_back(backhouse2);
    gameObjects.push_back(tree_bush);
	gameObjects.push_back(tree_bush_1);
	gameObjects.push_back(tree_bush_2);
	gameObjects.push_back(backfir1);
	gameObjects.push_back(backfir2);
	gameObjects.push_back(backfir3);
	gameObjects.push_back(backfir5);
	gameObjects.push_back(backfir6);
	gameObjects.push_back(backfir7);
	gameObjects.push_back(backfir8);
	gameObjects.push_back(backfir9);
	gameObjects.push_back(treeline);
	gameObjects.push_back(treeline1);
	gameObjects.push_back(treeline2);
	gameObjects.push_back(treeline3);
	gameObjects.push_back(treeline4);
	gameObjects.push_back(treeline5);
	gameObjects.push_back(tree_bush_4);
	gameObjects.push_back(tree_bush_5);
	gameObjects.push_back(tree_bush_6);

    /*
        Initialize and store animatable game objects 
    */

    Bird* birds = new Bird(OBJ_BIRDS, tranBirds, scaleBirds, rotBirds);
    Harp* harp  = new Harp(OBJ_HARP, tranHarp, scaleHarp, rotHarp);
	NPC* npc    = new NPC (OBJ_YUN, tranNPC, scaleNPC, rotNPC, 5.0f);
    gameObjects.push_back(birds);
    gameObjects.push_back(harp);
	gameObjects.push_back(npc);

    // add to list of animation objects which need to be updated each frame
    animationObjects.push_back(birds);
    animationObjects.push_back(harp);
	
    // Load all animated coins and add to game objects, animation objects, and coin list
	for (auto trans : coinTranslations) {
		Coin* coin = new Coin(OBJ_COIN, trans, scaleCoins, rotCoins);
		coin->setDestroyed(true);
        gameObjects.push_back(coin);
        animationObjects.push_back(coin);
        coins.push_back(coin);
	}

	// Scale all objects Size and Translation
	for (auto gameObject : gameObjects) {
		gameObject->setScale(gameObject->getScale() * GLOBAL_SCALE);
		gameObject->setTranslation(gameObject->getTranslation()* GLOBAL_POSITION_SCALE);
	}


	/*
		Initialize instanced game objects
	*/

	Grass* grass = new Grass(OBJ_GRASS, instancedObjectShader);
	instancedObjects.push_back(grass);
	
	/*
		AUDIO ENGINE and SOUND LOADING
	*/
	// Initialize Audio Engine
	audioEngine = std::make_shared<AudioEngine>();
	audioEngine->init();
	
	// load sounds
	audioEngine->loadSound(fountainSoundLoop);
	audioEngine->loadSound(soundTree);
	audioEngine->loadSound(soundJapaneseTree);
	audioEngine->loadSound(dialogue);
	
	// setup sound controllers
	footstepController = new FootstepSoundController(audioEngine);
	coinSoundController = new CoinChallengeSoundController(audioEngine, coins.size());

	// Start inital soundscape
	audioEngine->playSound(soundTree);
	audioEngine->playSound(soundJapaneseTree);
	audioEngine->playSound(fountainSoundLoop);
	
    
    /* render loop */ 
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        ProcessInput(window);
		
		glClearColor(COLOR_SKY.x, COLOR_SKY.y, COLOR_SKY.z, COLOR_SKY.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // enable blended overwrite of color buffer
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Collision detection for coins
        for (auto coin : coins) {
            if (!coin->isDestroyed()) {
                if (coin->collidesWithSphere(camera)) {
                    coin->setDestroyed(true);
					coinSoundController->characterPickedUpCoin();
					
                }
            }
        }

		// Collision detection for dialogue triggering
		if (!npc->hasSaidDialogueLine()) {
			if (npc->collidesWithSphere(camera)) {
				audioEngine->playSound(dialogue);
				npc->setHasSaidDialogueLine(true);
				unsigned int lengthMS = audioEngine->getSoundLengthInMS(dialogue);
				std::cout << "Before thread, length MS = " << lengthMS << '\n';
				std::thread thd(waitForDialogueCompletion, lengthMS);
				thd.detach();
			}
		}

        // update animation objects with current frame
        for (int i = 0; i < animationObjects.size(); i++)
            animationObjects[i]->update(currentFrame);

        // render Game Objects
        for (int i = 0; i < gameObjects.size(); i++) 
            renderGameObject(*gameObjects[i], &gameObjectShader);
        
        // render instanced objects
        for(InstancedObject *instancedObject : instancedObjects)
            instancedObject->drawInstances(getProjection(), camera.GetViewMatrix());
       
		/*
            Audio Engine per-frame updates
        */
		// per-frame FMOD update
		audioEngine->update(); 
        // set current player position in audio engine (X and Y in Fron and Up vectors need to be swapped for FMOD compatability)
        audioEngine->set3DListenerPosition(camera.Position.x, camera.Position.y, camera.Position.z,
                                          camera.Front.y,    camera.Front.x,    camera.Front.z,
                                          camera.Up.y,       camera.Up.x,       camera.Up.z );
        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

/**
 * Checks if a key can be triggered depending on how recently it was last triggered.
 * If true sets the key's most recent trigger time to current frame
 */

bool keyCanRetrigger(float &currFrame, float &lastTriggerFrame) {
	bool canRetrigger = currFrame - lastTriggerFrame >= KEY_MIN_RETRIGGER_TIME;
	if (canRetrigger) lastTriggerFrame = currFrame;
	return canRetrigger;
}

/**
 * process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
 */
void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// WASD Handling (Character Movement)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.processKeyboard(FORWARD, deltaTime);
		footstepController->processFootstepKey(currentFrame);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.processKeyboard(BACKWARD, deltaTime);
		footstepController->processFootstepKey(currentFrame);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.processKeyboard(LEFT, deltaTime);
		footstepController->processFootstepKey(currentFrame);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.processKeyboard(RIGHT, deltaTime);
		footstepController->processFootstepKey(currentFrame);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.processKeyboard(RUNNING_START, deltaTime);
		footstepController->setRunning(true);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		camera.processKeyboard(RUNNING_STOP, deltaTime);
		footstepController->setRunning(false);
	}

	// 'Reset Coin Challenge' Key (k)
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && keyCanRetrigger(currentFrame, keyKLastTime)) {
		resetCoins();
		coinSoundController->reset();
	}
	// Audio Engine Mute Key (m)
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && keyCanRetrigger(currentFrame, keyMLastTime))
		audioEngine->isMuted() ? audioEngine->unmuteAllSound() : audioEngine->muteAllSounds();


	// Number Keys: Coin Controls TODO fix collision detection so that these controls aren't needed
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && keyCanRetrigger(currentFrame, key1LastTime)) {
		if (!coins[0]->isDestroyed()) {
			coins[0]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && keyCanRetrigger(currentFrame, key2LastTime)) {
		if (!coins[1]->isDestroyed()) {
			coins[1]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}	
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && keyCanRetrigger(currentFrame, key3LastTime)) {
		if (!coins[2]->isDestroyed()) {
			coins[2]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && keyCanRetrigger(currentFrame, key4LastTime)) {
		if (!coins[3]->isDestroyed()) {
			coins[3]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && keyCanRetrigger(currentFrame, key5LastTime)) {
		if (!coins[4]->isDestroyed()) {
			coins[4]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && keyCanRetrigger(currentFrame, key6LastTime)) {
		if (!coins[5]->isDestroyed()) {
			coins[5]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && keyCanRetrigger(currentFrame, key7LastTime)) {
		if (!coins[6]->isDestroyed()) {
			coins[6]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && keyCanRetrigger(currentFrame, key8LastTime)) {
		if (!coins[7]->isDestroyed()) {
			coins[7]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && keyCanRetrigger(currentFrame, key9LastTime)) {
		if (!coins[8]->isDestroyed()) {
			coins[8]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && keyCanRetrigger(currentFrame, key0LastTime)) {
		if (!coins[9]->isDestroyed()) {
			coins[9]->setDestroyed(true);
			coinSoundController->characterPickedUpCoin();
		}
	}


}

/**
 * GLFW resize window function
 */
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
/**
 * GLFW mouse function
 */
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

	camera.processMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.processMouseScroll(yoffset);
}