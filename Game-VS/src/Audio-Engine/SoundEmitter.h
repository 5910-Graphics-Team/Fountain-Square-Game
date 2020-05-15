#pragma once
#include "AudioEngine.h"
#include "LoopingSound.h"

/**
* Class which can be implemented to allow looping 3D sound playback from a moving in-game location.
*/
class SoundEmitter {
public:

	SoundEmitter(AudioEngine* audioEngine, const char* soundFile, float x, float y, float z) {
		soundLoop = new LoopingSound3D(audioEngine, soundFile, x, y, z);
		soundLoop->init();
	}

	void startSound()  {
		soundLoop->play();
	}


	void stopSound() {
		soundLoop->stop();
	}

	void updateSoundPosition(float x, float y, float z) {
		soundLoop->setSoundPosition(x, y, z);
	}

private:
	LoopingSound3D* soundLoop;
};