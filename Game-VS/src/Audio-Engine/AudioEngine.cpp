#include "AudioEngine.h"
#include <iostream>

AudioEngine::AudioEngine() : soundCache() {
    FMOD::Studio::System::create(&system);
    system->getCoreSystem(&coreSystem);
    coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);
    coreSystem->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
    system->initialize(MAX_AUDIO_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
}


//AudioEngine::Sound* AudioEngine::createSound(const char* filepath, bool dim3D, bool loop) {
//    FMOD::Sound* sound;
//    coreSystem->createSound(filepath, dim3D ? FMOD_3D : FMOD_2D, 0, &sound);
//    
//    if (dim3D) sound->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
//    sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
//    FMOD::Channel* channel = 0;
//    Sound s(sound, channel); // TODO test
//    
//    return &s;
//}

//void AudioEngine::playSound(Sound* sound) {
//    FMOD::Channel* channel;
//    coreSystem->playSound(sound->sound, 0, false, &channel);
//    //sound->setPaused(false);
//}

void AudioEngine::set3DListenerPosition(float posX, float posY, float posZ, float forwardX, float forwardY, float forwardZ, float upX, float upY, float upZ) {
    listenerpos = { posX,     posY,     posZ };
    forward =     { forwardX, forwardY, forwardZ };
    up =          { upX,      upY,      upZ      };
    coreSystem->set3DListenerAttributes(0, &listenerpos, 0, &forward, &up);
    //FMOD_VECTOR vel;


        //if (listenerflag)
        //{
        //    listenerpos.x = (float)sin(t * 0.05f) * 24.0f * DISTANCEFACTOR; // left right pingpong
        //}

        // ********* NOTE ******* READ NEXT COMMENT!!!!!
        // vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
        /*vel.x = (listenerpos.x - lastpos.x) * (1000 / INTERFACE_UPDATETIME);
        vel.y = (listenerpos.y - lastpos.y) * (1000 / INTERFACE_UPDATETIME);
        vel.z = (listenerpos.z - lastpos.z) * (1000 / INTERFACE_UPDATETIME);*/

        // store pos for next time
        //lastpos = listenerpos;



        //t += (30 * (1.0f / (float)INTERFACE_UPDATETIME));    // t is just a time value .. it increments in 30m/s steps in this example

        //FMOD_VECTOR  listenervel = { velocity.x, 0.0f, velocity.y };
        ////update position & velocity of listener
        ////position of listener needed for spatial & reverb effects
        ////velocity of listener needed for dopper effects
        //FMOD_VECTOR  listenerpos = { listenerSprite.getPosition().x, 0.0f, listenerSprite.getPosition().y };
        ////final pair of parameters are forward direction and up direction of listener (not needed in 2D)
        //FMODsys->set3DListenerAttributes(0, &listenerpos, &listenervel, 0, 0);

        ////update position of sound
        //if (channel) {
        //    FMOD_VECTOR  sourcePos = { sourceSprite.getPosition().x, 0.0f, sourceSprite.getPosition().y };
        //    //source is fixed so velocity is zero
        //    channel->set3DAttributes(&sourcePos, 0);
        //}
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

//FMOD::Sound* AudioEngine::createSound(const char* filepath) {
//    FMOD::Sound* sound;
//    coreSystem->createSound(filepath, FMOD_LOOP_OFF, 0, &sound);
//    return sound;
//}


// private method definitions

//FMOD::Sound* AudioEngine::createOrGetSound(unsigned int id) {
//    FMOD::Sound* sound;
//    // get sound from cache, or create if not added
//    if (!soundIsCached(1)) {
//        // sound has not been created yet
//        std::cout << "sound has not been created yet. Creating!\n";
//        coreSystem->createSound(filepath, FMOD_LOOP_OFF, 0, &sound);
//    }
//    else {
//        sound = getSound(filepath);
//    }
//    return sound;
//}


bool AudioEngine::soundIsCached(const char* filepath) {
    return soundCache.count(filepath) > 0;
}

FMOD::Sound* AudioEngine::getSound(const char* filepath) {
    return soundCache[filepath];
}


