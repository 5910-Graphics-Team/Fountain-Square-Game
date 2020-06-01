#pragma once
#include "AudioEngine.h"
#include "../GameData.h"


class CoinSoundController {
	
public:
	CoinSoundController(std::shared_ptr<AudioEngine> audioEngine, int nTotalCoins) : nTotalCoins(nTotalCoins), nCharacterCoins(0) {
		//this->coinPickup = coinPickup, this->coinSuccess = coinSuccess;
	}
	void characterPickedUpCoin() {
		std::cout << "Character picked up coin\n";
		nCharacterCoins++;
		/*if (nCharacterCoins == nTotalCoins)
			audioEngine->playSound(coinSuccess);
		else
			audioEngine->playSound(coinPickup);*/

	}

	int nTotalCoins;
	int nCharacterCoins;
private:
	std::shared_ptr<AudioEngine> audioEngine;
	//SoundInfo coinPickup, coinSuccess;




};