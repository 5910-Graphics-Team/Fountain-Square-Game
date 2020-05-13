#pragma once
#include "../Audio-Engine/SoundEmitter.h"
#include "../Audio-Engine/AudioEngine.h"
#include "../Game-Engine/GameObject.h"
#include "../Game-Engine/Animation.h"
#include <iostream>

class Bird : public GameObject, public Animation, public SoundEmitter {
private:
	//SoundEmitter soundEmitter;
protected:
	float lastFrameTime = 0.0f;
	float speed = 1.0f;


public:

	Bird(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot, AudioEngine* audioEngine, const char* soundFile)
		: GameObject(objFile, defTrans, defScale, defRot), SoundEmitter(audioEngine, soundFile, defTrans.x, defTrans.y, defTrans.z) {
		
	}

	void updateLocation(float time) override {
		float timePassed = time - lastFrameTime;


		setTranslation({ trans.x, trans.y, trans.z - speed * timePassed });
		
		lastFrameTime = time;

		updateSoundPosition(trans.x, trans.y, trans.z);
	
	}
	

};