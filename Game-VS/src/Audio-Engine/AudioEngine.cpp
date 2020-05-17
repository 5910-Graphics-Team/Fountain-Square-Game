#include "AudioEngine.h"
#include <iostream>

// Public definitions 

AudioEngine::AudioEngine() : soundCache(), channelMap() {
    FMOD::Studio::System::create(&studioSystem);
    studioSystem->getCoreSystem(&lowLevelSystem);
    lowLevelSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);
    lowLevelSystem->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
    studioSystem->initialize(MAX_AUDIO_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
}

void AudioEngine::load2DSoundFile(const char* filepath, bool loop) {
    if (!soundIsCached(filepath)) {
        std::cout << "Loading Sound File " << filepath << "\n";

        FMOD::Sound* sound;
        lowLevelSystem->createSound(filepath, FMOD_2D, 0, &sound);
        
        sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

        soundCache.insert({ filepath,  sound });

    }
    else 
        std::cout << "Sound File was already loaded!\n";
}

void AudioEngine::play2DSoundFile(const char* filepath) {
    if (soundIsCached(filepath)) {
        // channel for sound to play on
        FMOD::Channel* channel;
        lowLevelSystem->playSound(getSound(filepath), 0, false, &channel);
    }
    else
        std::cout << "AudioEngine: Trying to play a sound that wasn't loaded!\n";
}

void AudioEngine::load3DSoundFile(const char* filepath, bool loop) {
    if (!soundIsCached(filepath)) {
        std::cout << "Loading 3D Sound File " << filepath << "\n";
        FMOD::Sound* sound;
        lowLevelSystem->createSound(filepath, FMOD_3D, 0, &sound);
        
        // TODO allow user to set custom 3D Min Max Distance
        sound->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
        
        sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

        // TODO create separate cache for 3D sounds
        soundCache.insert({ filepath,  sound });
    
    }
    else
        std::cout << "AudioEngine: 3D Sound File was already loaded!\n";
}

void AudioEngine::play3DSound(const char* filepath, float x, float y, float z) {
    if (soundIsCached(filepath)) {

        FMOD_VECTOR position = { x * DISTANCEFACTOR, y, z };
        FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
        FMOD::Channel* channel;
        lowLevelSystem->playSound(getSound(filepath), 0, true, &channel);
        channel->set3DAttributes(&position, &velocity);
        channel->setPaused(false);
    }
    else
        std::cout << "AudioEngine: Trying to play a 3DSound that wasn't loaded!\n";
}

void AudioEngine::update3DSoundPosition(const char* filename, float x, float y, float z) {
    if (channelMap.count(filename) > 0) {
        FMOD::Channel* channel = channelMap[filename];
        FMOD_VECTOR position = { x * DISTANCEFACTOR, y * DISTANCEFACTOR, z * DISTANCEFACTOR };
        FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
        channel->set3DAttributes(&position, &velocity);
    }
}

void AudioEngine::set3DListenerPosition(float posX, float posY, float posZ, float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ) {
    listenerpos = { posX,     posY,     posZ };
    forward = { forwardY, forwardX, forwardZ };
    up = { upY,      upX,      upZ };
    lowLevelSystem->set3DListenerAttributes(0, &listenerpos, 0, &forward, &up);
}

// Private definitions 

bool AudioEngine::soundIsCached(const char* filepath) {
    return soundCache.count(filepath) > 0;
}

FMOD::Sound* AudioEngine::getSound(const char* filepath) {
    return soundCache[filepath];
}


