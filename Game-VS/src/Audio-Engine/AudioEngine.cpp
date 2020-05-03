#include "AudioEngine.h"
#include <iostream>

AudioEngine::AudioEngine() : soundCache() {
    FMOD::Studio::System::create(&system);
    system->getCoreSystem(&coreSystem);
    coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);
    coreSystem->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
    system->initialize(MAX_AUDIO_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    
}

void AudioEngine::cacheSoundFile(const char* filepath) {   
    if (!soundIsCached(filepath)) {
        std::cout << "Creating and caching sound at " << filepath << "\n";
        soundCache.insert({ filepath,  createSound(filepath) });
    }
    else 
        std::cout << "Sound was already cached!\n";
}


void AudioEngine::playSoundFile(const char* filepath, bool cache = true) {  
    FMOD::Sound* sound = createOrGetSound(filepath);
    // channel for sound to play on
    FMOD::Channel* channel;
    coreSystem->playSound(sound, 0, false, &channel);
}




// private method definitions

FMOD::Sound* AudioEngine::createOrGetSound(const char* filepath) {
    FMOD::Sound* sound;
    // get sound from cache, or create if not added
    if (!soundIsCached(filepath)) {
        // sound has not been created yet
        std::cout << "sound has not been created yet. Creating!\n";
        coreSystem->createSound(filepath, FMOD_LOOP_OFF, 0, &sound);
    }
    else {
        sound = getSound(filepath);
    }
    return sound;
}


bool AudioEngine::soundIsCached(const char* filepath) {
    return soundCache.count(filepath) > 0;
}

FMOD::Sound* AudioEngine::getSound(const char* filepath) {
    return soundCache[filepath];
}


FMOD::Sound* AudioEngine::createSound(const char* filepath) {
    FMOD::Sound* sound;
    coreSystem->createSound(filepath, FMOD_LOOP_OFF, 0, &sound);
    return sound;
}