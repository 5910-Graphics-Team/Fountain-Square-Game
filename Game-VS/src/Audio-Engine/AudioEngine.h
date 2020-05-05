#pragma once
// AudioEngine.h
// @author Ross Hoyt
// Seattle University SQ2020
// CPSC 5910 Graphics/Game Project 
#include <map>
#include <string>
#include <iostream>
#include <FMOD/fmod_studio.hpp>
#include <FMOD/fmod.hpp>



class AudioEngine {

public:
    class Sound {
    private:

        //bool dim3D, loop;
        FMOD_VECTOR position;// { 0.0f, 0.0f, 0.0f};

    public:
        //std::shared_ptr<FMOD::Sound>* sound;
        FMOD::Sound* sound;
        FMOD::Channel* channel;

        Sound(FMOD::Sound* sound, FMOD::Channel* channel, float x = 0.0f, float y = 0.0f, float z = 0.0f) {
            sound = sound;
            channel = channel;
            std::cout << "Created sound at with channel = " << channel << "\n";
            setPosition(x, y, z);
        }
        FMOD::Sound* getSound() {
            return sound;
        }
        FMOD::Channel** getChannel() {
            return &channel;
        }
        void setPosition(float x, float y, float z) {
            //position = { x, y, z };
            position = { -10.0f /* * DISTANCEFACTOR*/, 0.0f, 0.0f };
            FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };
            channel->set3DAttributes(&position, &velocity);
        }
        FMOD_VECTOR getPosition() {
            return position;
        }
        void setPaused(bool paused) {
            channel->setPaused(paused);
        }
    };
    /**
    * Initializes Audio Engine Studio and Core systems
    */
    AudioEngine();

    Sound* createSound(const char* filepath, bool dim3D, bool loop);

    void playSound(Sound* sound);

    void update3DListenerPosition(float forwardX, float forwardY, float forwardZ);



    /**
    * Loads a sound file into the cache to prepare for later playback. 
    * Only reads file and creates the sound if it has not already been added to the cache.
    * To play the sound later, use method playSoundFile()
    */
    void cacheSoundFile(const char* filepath);

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
    void playSoundFile(const char* filepath, bool cache);


private:    
    FMOD::Studio::System* system = nullptr;              // FMOD Studio API
    FMOD::System* coreSystem = nullptr;          // FMOD's low-level audio system, obtained from Studio System    
    static const unsigned int MAX_AUDIO_CHANNELS = 1024; // Max FMOD audio channels for this audio engine 
    
    float t = 0;
    const float DISTANCEFACTOR = 1.0f;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.
    FMOD_VECTOR listenerpos = { 0.0f, 0.0f, -1.0f * DISTANCEFACTOR };
    FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
    FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };

    /*
    * Map which stores the low-level sound cache. 
    * Key is the relative file path of each unique sound asset. TODO Refactor to use numeric UID as key
    * Value is the FMOD::Sound* to be played back.
    */
    std::map<std::string, FMOD::Sound*> soundCache;      

    /**
    * Gets a sound from the sound cache, or creates it if it hasn't been cached
    */
    FMOD::Sound* createOrGetSound(const char* filepath);
    
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
    FMOD::Sound* createSound(const char* filepath);

};
