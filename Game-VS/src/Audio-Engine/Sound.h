#pragma once
#include "AudioEngine.h"
#include <FMOD/fmod_common.h>

/**
* Base abstraction for a sound event in the game
* Encapsulates the FMOD Channel and FMOD::Sound, as well as audio file path
*/
class Sound {
protected:

	AudioEngine* audioEngine = nullptr;
	const char*  filePath    = nullptr;

	FMOD::Sound*  theSound = nullptr;
	FMOD::Channel* channel = nullptr;

	/**
	* Custom initializing method to be implemented by inheriting class
	*/
	//virtual void init() {}

public:
	
	Sound(AudioEngine* audioEngine, const char* filePath) : filePath(filePath) {
		this->audioEngine = audioEngine;
		//init();
	}

	/**
	* Custom play method to be implemented by inheriting class
	*/
	virtual void play() {
		
	}
};