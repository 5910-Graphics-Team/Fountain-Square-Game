#pragma once
#include "AudioEngine.h"
#include "SoundInfo.h"
#include "../GameData.h"


class CoinChallengeSoundController {
	
public:


	CoinChallengeSoundController(std::shared_ptr<AudioEngine> audioEngine, int nTotalCoins)
		: audioEngine(audioEngine), nTotalCoins(nTotalCoins) {
		//this->coinPickup = coinPickup, this->coinSuccess = coinSuccess;
		init();
		std::cout << "Beat sample length: " << beatSampleLength << "\n";
	}

	// starts the interactive music container from ambienct music level 1 
	void startScore() {
		std::cout << "Starting Coin Challenge Score\n";
		audioEngine->playSound(musicLayer_BeforeChallenge);
		audioEngine->playSound(musicLayer_StartedChallenge);
		audioEngine->playSound(musicLayer_CompletedChallenge);
	}


	void characterPickedUpCoin() {
		
		nCharacterCoins++;
		
		if (nCharacterCoins == 1) {
			audioEngine->updateSoundLoopVolume(musicLayer_StartedChallenge, defVolume, timeSignatureNumerator * beatSampleLength);
			audioEngine->playSound(stinger_CoinPickup);
		}
		else if (nCharacterCoins == nTotalCoins) {
			audioEngine->updateSoundLoopVolume(musicLayer_CompletedChallenge, defVolume, beatSampleLength);
			audioEngine->playSound(stinger_Success);

		}
		else {
			audioEngine->playSound(stinger_CoinPickup);
		}

		std::cout << "Character picked up a coin\n";

	}
private:
	// Player stats
	int nTotalCoins;
	// Total coins needed
	int nCharacterCoins = 0;

	// Audio engine
	std::shared_ptr<AudioEngine> audioEngine;

	// Tempo speed 
	const float bpm = 158.0f;
	int timeSignatureNumerator = 4, timeSignatureDenominator = 4;
	int beatSampleLength = AudioEngine::AUDIO_SAMPLE_RATE * 60 / bpm;

	// Music assets/info 
	const char* MUSIC_LAYER1 =              "res/sound/music/coin-challenge/CoinChallenge_MXLayer1.wav";
	const char* MUSIC_LAYER2 =              "res/sound/music/coin-challenge/CoinChallenge_MXLayer2.wav";
	const char* MUSIC_LAYER3 =              "res/sound/music/coin-challenge/CoinChallenge_MXLayer3.wav";
	const char* STINGER_COIN_PICKUP =       "res/sound/music/coin-challenge/CoinChallenge_Stinger_PickupCoin.wav";
	const char* STINGER_CHALLENGE_SUCCESS = "res/sound/music/coin-challenge/CoinChallenge_Stinger_CompleteChallenge.wav"; 

	SoundInfo musicLayer_BeforeChallenge    { MUSIC_LAYER1,        defVolume, 0.0f, SOUND_LOOP };
	SoundInfo musicLayer_StartedChallenge   { MUSIC_LAYER2,        0.0f,      0.0f, SOUND_LOOP };
	SoundInfo musicLayer_CompletedChallenge { MUSIC_LAYER3,        0.0f,      0.0f, SOUND_LOOP };
	SoundInfo stinger_CoinPickup            { STINGER_COIN_PICKUP, defVolume, 0.0f };
	SoundInfo stinger_Success               { STINGER_CHALLENGE_SUCCESS, defVolume, 0.0f };


	void init() {
		audioEngine->loadSound(stinger_CoinPickup);
		audioEngine->loadSound(musicLayer_BeforeChallenge);
		audioEngine->loadSound(musicLayer_StartedChallenge);
		audioEngine->loadSound(musicLayer_CompletedChallenge);
		audioEngine->loadSound(stinger_Success);
	}


		//typedef enum {
	//	STATE_BEFORE_START,
	//	STATE_STARTED,
	//	STATE_COMPLETED
	//} ChallengeState;


};