#pragma once
#include "AudioEngine.h"
#include <memory>
//#include <tgmath.h>
#include "../GameData.h"

struct IndexRandomizer {
	// total entries must be > 0
	IndexRandomizer(int totalEntries, int roundRobinMin = 3) : totalEntries(totalEntries), roundRobinMin(roundRobinMin), queue() {
		initQueue();
	}

	int getNextIndex() {
		int index = queue.at(0);
		queue.erase(queue.begin());
		queue.push_back(getNewIndex());
		//std::cout << "Playing footstep #" << index << '\n';
		return index;
	}
private:

	std::vector<int> queue;
	int totalEntries;
	int roundRobinMin;// 3 new sounds must be played before a sound is retriggered

	void initQueue() {
		for (int i = 0; i < roundRobinMin; ++i)
			queue.push_back(getNewIndex());
	}
	// generates a new index not in the soundqueue
	int getNewIndex() {
		bool ready = false;
		int index = rand() % totalEntries;
		while (queueContains(index)) index = rand() % totalEntries;
		return index;
	}

	bool queueContains(int val) {
		for (int i : queue)
			if (i == val) return true;
		return false;
	}
};

class FootstepSoundController {
public:
	std::vector<SoundInfo> soundsFootsteps {
		SoundInfo(SFX_FOOTSTEP1, defVolume, defReverb),
		SoundInfo(SFX_FOOTSTEP2, defVolume, defReverb),
		SoundInfo(SFX_FOOTSTEP3, defVolume, defReverb),
		SoundInfo(SFX_FOOTSTEP4, defVolume, defReverb),
		SoundInfo(SFX_FOOTSTEP5, defVolume, defReverb),
		SoundInfo(SFX_FOOTSTEP6, defVolume, defReverb),
		SoundInfo(SFX_FOOTSTEP7, defVolume, defReverb),
		SoundInfo(SFX_FOOTSTEP8, defVolume, defReverb)
	};


	FootstepSoundController(std::shared_ptr<AudioEngine> audioEngine) : audioEngine(audioEngine), indexRandomizer(soundsFootsteps.size(), 3) {
		init();
	}

	void processFootstepKey(float currFrame) {
		// check that we haven't already triggered this method this frame
		if (this->currFrame != currFrame) {
			this->currFrame = currFrame;
			if (lastFootstepTime + footstepWaitingTIME < currFrame) {
				//audioEngine->playEvent(FMOD_EVENT_CHARACTER_FOOTSTEPS);
				audioEngine->playSound(soundsFootsteps[indexRandomizer.getNextIndex()]);
				lastFootstepTime = currFrame;
				setFootstepTimeRandomely();
			}
		}
	}

	void setRunning(bool isRunning) {
		this->isRunning = isRunning;
		setFootstepTimeRandomely();
	}
	
private:

	std::shared_ptr<AudioEngine> audioEngine;
	
	IndexRandomizer indexRandomizer;
	
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