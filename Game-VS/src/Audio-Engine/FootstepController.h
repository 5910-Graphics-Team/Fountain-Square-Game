#pragma once
#include "AudioEngine.h"
#include "../GameData.h"


struct FootstepController {
	
	FootstepController(AudioEngine* audioEngine) : audioEngine(audioEngine) {}

	void processFootstepKey(float currFrame) {
		//std::cout << "WASD Footstep key.. " << currFrame << "\n";
		// check if we've already triggered this method this frame, and do nothing if so
		if (this->currFrame != currFrame) {
			this->currFrame = currFrame;
			if (lastFootstep + footstepWaitingTIME < currFrame) {
				audioEngine->playEvent(FMOD_EVENT_CHARACTER_FOOTSTEPS);
				lastFootstep = currFrame;
				setFootstepTimeRandomely();
			}
		}
	}

private:
	AudioEngine* audioEngine;
	const float DEF_MIN_FOOTSTEP_TIME = 0.5f;
	float footstepWaitingTIME = DEF_MIN_FOOTSTEP_TIME;
	float lastFootstep = 0.0f;
	float currFrame = 0.0f;

	void setFootstepTimeRandomely() {
		float seed((rand() % 100) / 600.f);
		footstepWaitingTIME = DEF_MIN_FOOTSTEP_TIME + seed;
		//std::cout << "Seed = " << seed << " waitingTime = " << footstepWaitingTIME << "\n";
	}

};