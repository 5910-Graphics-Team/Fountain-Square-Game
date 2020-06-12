#pragma once
#include "../Audio-Engine/AudioEngine.h"
#include "../Game-Engine/GameObject.h"
#include "../Game-Engine/Animation.h"
/**
 * Custom animated game object for a Bird model.
 */
class Bird : public GameObject, public Animation {
protected:
	float lastFrameTime = 0.0f;
	float speed = 2.0f;


public:
	/**
	 * Constructs a Bird with provided filepath, translation, scale and rotation
	 */
	Bird(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot)
		: GameObject(objFile, defTrans, defScale, defRot) {
	}
	/**
	 * Updates the bird location.
	 * TODO improve Bird pathfinding algorithm
	 */
	void update(float time) override {
		
		float timePassed = time - lastFrameTime;
		setTranslation({ trans.x, trans.y, trans.z - speed * timePassed });
		lastFrameTime = time;

	
	}
	

};