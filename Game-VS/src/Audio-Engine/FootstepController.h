#pragma once
#include "AudioEngine.h"
#include "../GameData.h"


struct FootstepController {
	
	FootstepController(AudioEngine* audioEngine) : audioEngine(audioEngine) {}

	void processFootstepKey(float currFrame) {
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

	void setRunning(bool isRunning) {
		this->isRunning = isRunning;
		setFootstepTimeRandomely();
	}
	
private:
	AudioEngine* audioEngine;

	const float MIN_FOOTSTEP_TIME_WALKING = 0.5f, MIN_FOOTSTEP_TIME_RUNNING = 0.25f;
	float footstepWaitingTIME = MIN_FOOTSTEP_TIME_WALKING;
	float lastFootstep = 0.0f;
	float currFrame = 0.0f;
	bool isRunning = false;

	void setFootstepTimeRandomely() {
		float randomness((rand() % 100) / 600.f);
		footstepWaitingTIME = isRunning ? MIN_FOOTSTEP_TIME_RUNNING : MIN_FOOTSTEP_TIME_WALKING + randomness;
	}

};