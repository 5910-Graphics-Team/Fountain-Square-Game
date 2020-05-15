#pragma once
///
/// @file AudioEngine.h
/// 
/// This audio engine is an FMOD wrapper with provides 2D/3D audio 
/// playback and sound loading
///
/// @author Ross Hoyt
/// @dependencies FMOD Studio/Core .dll's, lib's
///
#include <FMOD/fmod_studio.hpp>
#include <FMOD/fmod.hpp>
#include <iostream>
#include <string>
#include <map>

#include "Sound.h"


struct SoundID {

    unsigned int UID;

};
class AudioEngine {

public:
    
    /**
    * Constructor that initializes Audio Engine Studio and Core systems
    * DISTANCE_FACTOR is set to 1 Unit per 1 Meter by default
    */
    AudioEngine(float distanceFactor);



    /** 
    * Method used to set the 'head' position of the player in the game world,
    * so that 3D audio effects
    */
    void set3DListenerPosition(float posX,     float posY,     float posZ,
                               float forwardX, float forwardY, float forwardZ, 
                               float upX,      float upY,      float upZ);

    /**
    * Loads a sound file into the cache to prepare for later playback.
    * Only reads file and creates the sound if it has not already been added to the cache.
    * To play the sound later, use method playSoundFile()
    */
    void loadSoundFile(const char* filepath, bool dim3D, bool loop);
    

    void play3DSound(const char* filename, float x, float y, float z);


    void update3DSoundPosition(const char* filename, float x, float y, float z);
    
    
    /**
    * Plays a sound file using FMOD's low level audio system. If the sound file has not been 
    * previously created and cached prior to this method call, it is done before playback starts.
    * 
    * @var filename - relative path to file from project directory. (Can be .OGG, .WAV, .MP3,
    *                 or any other FMOD-supported audio format)
    *                 TODO List all supported file formats 
    * @var cache - if the sound should be stored for later use. Default = true
    *              if sound file has already been cached, value is ignored
    */
    void playSoundFile(const char* filepath);
  
    
    void initFMOD3DSound(FMOD::Sound* sound, const char* filePath);

    void playFMOD3DSound(FMOD::Sound* sound, FMOD::Channel* channel, float x, float y, float z);

    void updateChannel3DAttributes(FMOD::Channel* channel, float x, float y, float z);

    //void playSound(Sound& sound);
    
private:    


    FMOD::Studio::System* system = nullptr;              // FMOD Studio API
    FMOD::System* coreSystem = nullptr;          // FMOD's low-level audio system, obtained from Studio System    
    static const unsigned int MAX_AUDIO_CHANNELS = 1024; // Max FMOD audio channels for this audio engine 
    
    float t = 0;
    float DISTANCEFACTOR; // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.
    FMOD_VECTOR listenerpos = { 0.0f, 0.0f, -1.0f * DISTANCEFACTOR };
    FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
    FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };

    /*
    * Map which stores the low-level sound cache. 
    * Key is the relative file path of each unique sound asset. TODO Refactor to use numeric UID as key
    * Value is the FMOD::Sound* to be played back.
    */
    std::map<std::string, FMOD::Sound*> soundCache;      

    std::map<std::string, FMOD::Channel*> channelMap;

    /**
    * Gets a sound from the sound cache, or creates it if it hasn't been cached
    */
    //FMOD::Sound* createOrGetSound(const char* filepath);
    
    /**
    * Checks if a sound is in the cache
    */
    bool soundIsCached(const char* filepath);

    /**
    * Gets a sound from the cache without checking if it has been added. 
    * Only use if certain sound is in cache, as one will be created at provided filepath 
    * key value (but it won't be initialized or work)
    */
    FMOD::Sound* getSound(const char* filepath);

    /**
    * Creates a sound without checking if it already has been created and cached.
    */
    //FMOD::Sound* createSound(const char* filepath);
 
};
