#pragma once
// AudioEngine.h
// @author Ross Hoyt
// Seattle University SQ2020
// CPSC 5910 Graphics/Game Project 
#pragma once
#include <fmod.hpp>

class AudioEngine {
private:
    // Max FMOD audio channels for this audio engine 
    static const unsigned int AE_MAX_AUDIO_CHANNELS = 32;

    FMOD::System* sys; // FMOD's low-level audio system
public:
    AudioEngine();

    /**
    * A simple function which plays a sound file using FMOD's low level audio system.
    * NOTE - To hear the audio playback, the main thread sleeps for a calculated amount. This
    *        is just for the purposes of the test program
    *
    * @var filename - relative path to file from project directory. (Can be .OGG, .WAV, .MP3,
    *                 or any other FMOD-supported audio format)
    */
    void playSoundFMOD(const char* filename, bool loop = false, int nLoops = 1);

};