#pragma once
#include "AudioEngine.h"
#include <memory>
//#include <tgmath.h>
#include "../GameData.h"


struct FootstepController {
	
	FootstepController(std::shared_ptr<AudioEngine> audioEngine) : audioEngine(audioEngine) {
		init();
	}



	void processFootstepKey(float currFrame) {
		// check if we've already triggered this method this frame, and do nothing if so
		if (this->currFrame != currFrame) {
			this->currFrame = currFrame;
			if (lastFootstepTime + footstepWaitingTIME < currFrame) {
				//audioEngine->playEvent(FMOD_EVENT_CHARACTER_FOOTSTEPS);
				audioEngine->playSound(soundsFootsteps[nextFootstepIndex]);
				lastFootstepTime = currFrame;
				lastFootstepIndex = nextFootstepIndex;
				setFootstepTimeRandomely();
				setFootstepIndexRandomely();

			}
		}
	}

	void setRunning(bool isRunning) {
		this->isRunning = isRunning;
		setFootstepTimeRandomely();
	}
	
private:
	std::shared_ptr<AudioEngine> audioEngine;


	void init() {
		for (SoundInfo sound : soundsFootsteps) {
			audioEngine->loadSound(sound);
			++nSounds;
		}
		setFootstepIndexRandomely();
	}
	// Timing information
	const float MIN_FOOTSTEP_TIME_WALKING = 0.5f, MIN_FOOTSTEP_TIME_RUNNING = 0.25f;
	float footstepWaitingTIME = MIN_FOOTSTEP_TIME_WALKING;
	float lastFootstepTime = 0.0f;
	float currFrame = 0.0f;
	bool isRunning = false;

	// Sample variation information
	int nSounds = 0;
	int nextFootstepIndex = 0, lastFootstepIndex = 0;
	


	void setFootstepTimeRandomely() {
		float randomness((rand() % 100) / 600.f);
		footstepWaitingTIME = isRunning ? MIN_FOOTSTEP_TIME_RUNNING : MIN_FOOTSTEP_TIME_WALKING + randomness;
	}
	void setFootstepIndexRandomely() {
		nextFootstepIndex = rand() % nSounds;
		if (nextFootstepIndex == lastFootstepIndex)
			setFootstepTimeRandomely();
	}


};