// Simple-FMOD-Audio-Player.cpp 
// @author Ross Hoyt
// Seattle University SQ2020
// CPSC 5910 Graphics/Game Project 

#include <iostream>
#include <fmod.hpp>
#include <Windows.h>

FMOD::System* sys;                           // FMOD's low-level audio system
const char* AUDIO_FILE = "res/sound/mx_section_1.ogg"; // 00:31:30 loopable music file

/**
* A simple function which plays a sound file using FMOD's low level audio system.
* NOTE - For audio playback, the main thread sleeps for a calculated amound of time to allow
*        for FMOD's to playback sound - This 'calculating' the playback time is not really
*        the 'default' way to use FMOD to play audio from within your OpenGL game
*
* @var filename - relative path to file from project directory. (Can be .OGG, .WAV, .MP3,
*                 or any other FMOD-supported audio format)
*/
void playSoundFMOD(const char* filename, bool loop = false, int nLoops = 1) {
    // sound    
    FMOD::Sound* sound;
    // the sound's channel
    FMOD::Channel* channel;
    // create sound
    sys->createSound(filename, loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, 0, &sound);
    // assign channel and play sound in 'paused' mode
    sys->playSound(sound, 0, true /*start paused*/, &channel);
    // actually start the sound
    channel->setPaused(false);

    // calculate playback time needed (not the'default' way to achieve looping audio playback with FMOD...)
    unsigned int msLen;
    sound->getLength(&msLen, FMOD_TIMEUNIT_MS);
    // sleep main thread for total amount of audio playback time
    Sleep(msLen * nLoops);
}

int main() {
    FMOD::System_Create(&sys);
    sys->init(32 /*max number of audio channels*/, FMOD_INIT_NORMAL, 0);
    playSoundFMOD(AUDIO_FILE, true, 1);
    return 0;
}