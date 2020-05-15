#pragma once
#include "Sound.h"
#include "AudioEngine.h"
#include <FMOD/fmod_common.h>
#include <iostream>
//class Sound3D : public Sound {
//
//};

class LoopingSound3D : public Sound {
public:
	LoopingSound3D(AudioEngine* audioEngine, const char* filePath, float x, float y, float z) : Sound(audioEngine, filePath) {
		init();
		setSoundPosition(x, y, z);
		std::cout << "in LoopingSound3D Constructor for " << filePath <<"\n";
	}
	
	void init() {
		audioEngine->initFMOD3DSound(theSound, filePath);
	}


	void play() override {
		std::cout << "in LoopingSound3D::play()\n";
		audioEngine->playFMOD3DSound(theSound, channel, x, y, z);
	}

	void stop() {
		channel->stop();
		channel = nullptr;
	}
	
	void setSoundPosition(float x, float y, float z) {
		this->x = x, this->y = y, this->z = z;
		audioEngine->updateChannel3DAttributes(channel, x, y, z);
	}
private:
	float x = 0, y = 0, z = 0;
};