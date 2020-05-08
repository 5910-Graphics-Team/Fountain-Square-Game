#include "AudioEngine.h"
#include <iostream>

AudioEngine::AudioEngine() : soundCache() {
    FMOD::Studio::System::create(&system);
    system->getCoreSystem(&coreSystem);
    coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);
    coreSystem->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
    system->initialize(MAX_AUDIO_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
}

void AudioEngine::set3DListenerPosition(float posX, float posY, float posZ, float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ) {
    listenerpos = { posX,     posY,     posZ };
    forward =     { forwardY, forwardX, forwardZ };
    up =          { upY,      upX,      upZ      };
    coreSystem->set3DListenerAttributes(0, &listenerpos, 0, &forward, &up);
}

void AudioEngine::loadSoundFile(const char* filepath, bool dim3D, bool loop) {
    if (!soundIsCached(filepath)) {
        std::cout << "Creating and caching FMOD::Sound for " << filepath << "\n";

        FMOD::Sound* sound;
        coreSystem->createSound(filepath, dim3D ? FMOD_3D : FMOD_2D, 0, &sound);

        if (dim3D) sound->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
        sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

        soundCache.insert({ filepath,  sound });

    }
    else 
        std::cout << "Sound was already cached!\n";
}

void AudioEngine::play3DSound(const char* filepath, float x, float y, float z) {
    //position = { x, y, z };
    if (soundIsCached(filepath)) {

        FMOD_VECTOR position = { x * DISTANCEFACTOR, y, z };
        FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
        FMOD::Channel* channel;
        coreSystem->playSound(getSound(filepath), 0, true, &channel);
        channel->set3DAttributes(&position, &velocity);
        channel->setPaused(false);
    }
    else
        std::cout << "AudioEngine: Trying to play a 3DSound that wasn't loaded!\n";
}


void AudioEngine::playSoundFile(const char* filepath) {  
    if (soundIsCached(filepath)) {
        // channel for sound to play on
        FMOD::Channel* channel; 
        coreSystem->playSound(getSound(filepath), 0, false, &channel);
    }
    else
        std::cout << "AudioEngine: Trying to play a sound that wasn't loaded!\n";
}



bool AudioEngine::soundIsCached(const char* filepath) {
    return soundCache.count(filepath) > 0;
}

FMOD::Sound* AudioEngine::getSound(const char* filepath) {
    return soundCache[filepath];
}


