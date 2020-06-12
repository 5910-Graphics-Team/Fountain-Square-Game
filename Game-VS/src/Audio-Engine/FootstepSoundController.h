#pragma once
#include "AudioEngine.h"
#include "IndexRandomizer.h"
#include "../GameData.h"

/**
 * Class that manages the sounds associated with producing footsteps based on GLFW-managed keyboard input
 */
class FootstepSoundController {
public:
	/**
	 * Creates a footstep sound controller.
	 * Upon construction, loads all footstep sounds using the provided audio engine
	 * @param audioEngine Shared audio engine used to load and play sounds. 
	 *                    Must be initialized before passing into this constructor
	 */
	FootstepSoundController(std::shared_ptr<AudioEngine> audioEngine) : audioEngine(audioEngine), indexRandomizer(soundsFootsteps.size()) {
		init();
	}

	/**
	 * Method called when player has pressed, or is pressing, a keyboard key associated with walking/running.
	 * @param currFrame the GLFW time stamp of the current frame
	 */
	void processFootstepKey(float currFrame) {
		// make check that we haven't already triggered this method this frame
		if (this->currFrame != currFrame) {
			this->currFrame = currFrame;
			if (lastFootstepTime + footstepWaitingTIME < currFrame) {
				// play the footstep sound effect
				audioEngine->playSound(soundsFootsteps[indexRandomizer.getNextIndex()]);
				lastFootstepTime = currFrame; // track the last frame
				// set the amount of time to wait before the next footstep
				setFootstepTimeRandomely();
			}
		}
	}

	/**
	 * Method called when the player's running state changes.
	 * @param isRunning True if player is running, false if they are not running
	 */
	void setRunning(bool isRunning) {
		this->isRunning = isRunning;
		// reset amount of time to wait before playing next footstep
		setFootstepTimeRandomely();
	}

private:
	std::shared_ptr<AudioEngine> audioEngine;

	// Locations of Footstep sound effects
	const char* SFX_FOOTSTEP1 = "res/sound/footsteps/SFX_FOOTSTEP1.wav";
	const char* SFX_FOOTSTEP2 = "res/sound/footsteps/SFX_FOOTSTEP2.wav";
	const char* SFX_FOOTSTEP3 = "res/sound/footsteps/SFX_FOOTSTEP3.wav";
	const char* SFX_FOOTSTEP4 = "res/sound/footsteps/SFX_FOOTSTEP4.wav";
	const char* SFX_FOOTSTEP5 = "res/sound/footsteps/SFX_FOOTSTEP5.wav";
	const char* SFX_FOOTSTEP6 = "res/sound/footsteps/SFX_FOOTSTEP6.wav";
	const char* SFX_FOOTSTEP7 = "res/sound/footsteps/SFX_FOOTSTEP7.wav";
	const char* SFX_FOOTSTEP8 = "res/sound/footsteps/SFX_FOOTSTEP8.wav";

	// footstep default sound values
	float footstepVolume = 0.3f, footstepReverb = 0.4f;

	// the list of footstep sounds
	std::vector<SoundInfo> soundsFootsteps{
		SoundInfo(SFX_FOOTSTEP1, footstepVolume, footstepReverb),
		SoundInfo(SFX_FOOTSTEP2, footstepVolume, footstepReverb),
		SoundInfo(SFX_FOOTSTEP3, footstepVolume, footstepReverb),
		SoundInfo(SFX_FOOTSTEP4, footstepVolume, footstepReverb),
		SoundInfo(SFX_FOOTSTEP5, footstepVolume, footstepReverb),
		SoundInfo(SFX_FOOTSTEP6, footstepVolume, footstepReverb),
		SoundInfo(SFX_FOOTSTEP7, footstepVolume, footstepReverb),
		SoundInfo(SFX_FOOTSTEP8, footstepVolume, footstepReverb)
	};

	/**
	 * Loads the sound effects associated with this container and sets the first footstep audio file index.
	 */
	void init() {
		for (SoundInfo sound : soundsFootsteps) 
			audioEngine->loadSound(sound);		
	}

	// Utility which generates a random index without repeating a specific index 
	IndexRandomizer indexRandomizer;
	
	// Footstep Timing information
	const float MIN_FOOTSTEP_TIME_WALKING = 0.5f, MIN_FOOTSTEP_TIME_RUNNING = 0.25f;
	float footstepWaitingTIME = MIN_FOOTSTEP_TIME_WALKING;
	float lastFootstepTime = 0.0f;
	float currFrame = 0.0f;
	bool isRunning = false;

	/**
	 * Sets the time to wait before playing the next footstep if the player continues moving.
	 * Changes amound of time based on running vs walking state
	 */
	void setFootstepTimeRandomely() {
		float randomness((rand() % 100) / 600.f);
		footstepWaitingTIME = isRunning ? MIN_FOOTSTEP_TIME_RUNNING : MIN_FOOTSTEP_TIME_WALKING + randomness;
	}
};
