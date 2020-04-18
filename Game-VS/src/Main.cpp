// Main.cpp
// Seattle University SQ2020
// CPSC 5910 Graphics/Game Project 
#include <iostream>
#include "Audio-Engine/AudioEngine.h"

/**
* Main entry point which starts the game
*/
int main() {
	// For now, just setup the audio engine and play a sound
	AudioEngine audioEngine;
	audioEngine.playSoundFMOD("res/sound/mx_section_1.ogg");
	std::cin.get(); // wait for user input to exit
	return 0;
}