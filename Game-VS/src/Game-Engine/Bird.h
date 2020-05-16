#pragma once
#include "../Audio-Engine/AudioEngine.h"
#include "../Game-Engine/GameObject.h"
#include "../Game-Engine/Animation.h"
#include <iostream>

class Bird : public GameObject, public Animation {
private:
	//SoundEmitter soundEmitter;
protected:
	float lastFrameTime = 0.0f;
	float speed = 1.0f;


public:

	Bird(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot)
		: GameObject(objFile, defTrans, defScale, defRot) {
	}

	void update(float time) override {
		float timePassed = time - lastFrameTime;


		setTranslation({ trans.x, trans.y, trans.z - speed * timePassed });
		
		lastFrameTime = time;

	
	}
	

};