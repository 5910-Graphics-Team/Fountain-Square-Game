#pragma once
#include "AudioEngine.h"
#include "SoundInfo.h"
#include "../GameData.h"

/**
 * Class which manages all sounds associated with the Game's coin challenge,
 * including SFX and music.
 */
class CoinChallengeSoundController {
	
public:

	/**
	 * Creates the coin challenge sound controller.
	 * Upon construction, loads all footstep sounds using the provided audio engine
	 * @param audioEngine Shared audio engine used to load and play sounds.
	 *                    Must be initialized before passing into this constructor
	 * @param nTotalCoins The total number of coins in the coin challenge
	 */
	CoinChallengeSoundController(std::shared_ptr<AudioEngine> audioEngine, int nTotalCoins)
		: audioEngine(audioEngine), nTotalCoins(nTotalCoins) {
		init();
	}

	// starts the interactive music container from ambienct music level 1 
	void startScore() {
		std::cout << "Starting Coin Challenge Score\n";
		
		audioEngine->playSound(musicLayer_BeforeChallenge);
		audioEngine->playSound(musicLayer_StartedChallenge);
		audioEngine->playSound(musicLayer_ChallengeIntensity2);
		// fade in music	
		audioEngine->updateSoundLoopVolume(musicLayer_BeforeChallenge, defVolume, 2 * beatSampleLength * timeSignatureNumerator);
	}


	void characterPickedUpCoin() {
		nCharacterCoins++;
		std::cout << "Character picked up a coin\n";
		audioEngine->playSound(stinger_CoinPickup);
		// start search with intensity level 1
		if (nCharacterCoins <= nTotalCoins / 2) 
			audioEngine->updateSoundLoopVolume(musicLayer_StartedChallenge, defVolume / (nTotalCoins / 2) * nCharacterCoins, timeSignatureNumerator * beatSampleLength / 2);
		// intensity level 2
		else if ( nCharacterCoins < nTotalCoins)
			audioEngine->updateSoundLoopVolume(musicLayer_ChallengeIntensity2, defVolume / (nTotalCoins / 2) * nCharacterCoins, timeSignatureNumerator * beatSampleLength / 2);
		// success
		else if (nCharacterCoins == nTotalCoins) {
			std::cout << "Completed challenge\n";
			// stop song 1 layers
			unsigned long fadeLength = beat2SampleLength / 2;
			audioEngine->updateSoundLoopVolume(musicLayer_BeforeChallenge, 0.0f, fadeLength);
			audioEngine->updateSoundLoopVolume(musicLayer_StartedChallenge, 0.0f, fadeLength);
			audioEngine->updateSoundLoopVolume(musicLayer_ChallengeIntensity2, 0.0f, fadeLength);
			// play success sound TODO update sound
			audioEngine->playSound(stinger_Success);
			// play new music
			audioEngine->playSound(musicSection2_FullMix);
			// set 'Winning' music to fade out
			int remainingLoops = 30;
			audioEngine->updateSoundLoopVolume(musicSection2_FullMix, 0.0f, beat2SampleLength * timeSignatureNumerator * remainingLoops);
			// TODO fade back into 'regular' ambient music (non-coin challenge)?
		}

		// regular TODO delete below else statement
		else {
			audioEngine->playSound(stinger_CoinPickup);
		}


	}
	void reset() {
		nCharacterCoins = 0;
		audioEngine->stopSound(musicLayer_BeforeChallenge);
		audioEngine->stopSound(musicLayer_ChallengeIntensity2);
		audioEngine->stopSound(musicLayer_StartedChallenge);
		audioEngine->stopSound(musicSection2_FullMix);
		initLoopingSoundInfoVolumes();
		startScore();
	}

private:
	
	// Total coins needed
	int nTotalCoins;	
	// Player stats
	int nCharacterCoins = 0;

	// Audio engine
	std::shared_ptr<AudioEngine> audioEngine;

	// BPM (beats per minute) information about the 2 songs in this container
	const float bpm = 158.f, bpm2 = 109.f;
	// Rythmn information about the songs
	unsigned int timeSignatureNumerator = 4, timeSignatureDenominator = 4;
	// calculated time length, in audio samples, of a musical beat (ie 'quarter note') in the songs
	unsigned int beatSampleLength = AudioEngine::AUDIO_SAMPLE_RATE * 60 / bpm;
	unsigned int beat2SampleLength = AudioEngine::AUDIO_SAMPLE_RATE * 60 / bpm2;


	// Music assets/info 
	const char* MUSIC_LAYER1 =              "res/sound/music/coin-challenge/CoinChallenge_MXLayer1.wav";
	const char* MUSIC_LAYER2 =              "res/sound/music/coin-challenge/CoinChallenge_MXLayer2.wav";
	const char* MUSIC_LAYER3 =              "res/sound/music/coin-challenge/CoinChallenge_MXLayer3.wav";
	const char* STINGER_COIN_PICKUP =       "res/sound/music/coin-challenge/CoinChallenge_Stinger_PickupCoin.wav";
	const char* STINGER_CHALLENGE_SUCCESS = "res/sound/music/coin-challenge/CoinChallenge_Stinger_CompleteChallenge.wav"; 
	const char* MUSIC2_FULLMIX =            "res/sound/music/coin-challenge/CoinChallenge_MX2_FullMix.wav";

	SoundInfo musicLayer_BeforeChallenge     { MUSIC_LAYER1,              0.0f,      0.0f, SOUND_LOOP };
	SoundInfo musicLayer_StartedChallenge    { MUSIC_LAYER2,              0.0f,      0.0f, SOUND_LOOP };
	SoundInfo musicLayer_ChallengeIntensity2 { MUSIC_LAYER3,              0.0f,      0.0f, SOUND_LOOP };
	SoundInfo musicSection2_FullMix          { MUSIC2_FULLMIX,            0.8f,      0.0f, SOUND_LOOP };
	SoundInfo stinger_CoinPickup             { STINGER_COIN_PICKUP,       0.75f,     0.0f };
	SoundInfo stinger_Success                { STINGER_CHALLENGE_SUCCESS, 0.70f,     0.0f };
	
	// Prepares the volumes of the looping sounds before starting the challenge
	void initLoopingSoundInfoVolumes() {
		musicLayer_BeforeChallenge.setVolume(0.0f);
		musicLayer_StartedChallenge.setVolume(0.0f);
		musicLayer_ChallengeIntensity2.setVolume(0.0f);
		musicSection2_FullMix.setVolume(1.0f);
	}

	// 
	void init() {
		initLoopingSoundInfoVolumes();
		audioEngine->loadSound(stinger_CoinPickup);
		audioEngine->loadSound(musicLayer_BeforeChallenge);
		audioEngine->loadSound(musicLayer_StartedChallenge);
		audioEngine->loadSound(musicLayer_ChallengeIntensity2);
		audioEngine->loadSound(stinger_Success);
		audioEngine->loadSound(musicSection2_FullMix);
	}
	
};