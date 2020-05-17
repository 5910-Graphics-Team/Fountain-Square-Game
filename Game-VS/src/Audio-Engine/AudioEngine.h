#pragma once
///
/// @file AudioEngine.h
/// 
/// This audio engine is an FMOD wrapper with provides 2D/3D audio 
/// playback and sound loading
///
/// @author Ross Hoyt
/// @dependencies FMOD Studio & Core .dll's, lib's
/// 
/// @licese Creative Commons
/// 
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <FMOD/fmod_studio.hpp>
#include <FMOD/fmod.hpp>

//class AECoreSystemCache {
//    // TODO implement
//};
class AEStudioSystemCache {
public:

    AEStudioSystemCache() : eventDescriptions(), eventInstances(), eventParameters() {}

    void addEventDescription(const char* eventName, FMOD::Studio::EventDescription* eventDescription) {
        eventDescriptions.insert({ eventName, eventDescription });
    }

    void addEventInstance(const char* eventName, FMOD::Studio::EventInstance* eventInstance) {
        eventInstances.insert({ eventName, { eventInstance } });
        
    }

    void addEventParameter(const char* eventName, const char* parameterName) {
        if(eventParameters.count(eventName) > 0) {
            eventParameters[eventName].insert(parameterName);
        } 
        else
            eventParameters.insert({ eventName, { parameterName } });
    }

   /* void addParameter(const char* eventName, const char* paramName, float initValue) {
        parameterDescriptions.insert({ createUniqueParamKey(eventName, paramName), nullptr });
    }*/
    void setEventInstanceParam(const char* eventName, const char* paramName, float value, int instanceIndex = 0) {
        if (hasEventParameter(eventName, paramName)) {
            eventInstances.find(eventName)->second[instanceIndex]->setParameterByName(paramName, value);
            std::cout << "AEStudioSystemCache - Setting event " << eventName << " param " << paramName << " to value " << value << '\n';;
        }
        else {
            std::cout << "AEStudioSystemCache - Did not have event " << eventName << " parameter " << paramName << '\n';
        }
        
            //std::cout << "Event "<< eventName << " was not found while setting param " << paramName << '\n';
    }
    bool hasEvent(const char* eventName) {
        return eventInstances.count(eventName) > 0;
    }
    bool hasEventInstance(const char* eventName, unsigned int instanceIndex = 0) {
        if (hasEvent(eventName))
            return eventInstances[eventName].size() > instanceIndex;
        else return false;
    }
    bool hasEventParameter(const char* eventName, const char* parameterName) {
        if (hasEvent(eventName))
            return eventParameters.count(parameterName) > 0;
        else return false;
    }
    /**
    * Note - Programmer must call hasEventInstance() before calling this method 
    */
    FMOD::Studio::EventInstance* getEventInstance(const char* eventName, unsigned int instanceIndex = 0) {
        return eventInstances[eventName].at(instanceIndex);
    }
    

private:
    std::string createUniqueParamKey(const char* eventName, const char* paramName) {
        std::string buffer(eventName);
        buffer.append("_");
        buffer.append(paramName);
        return buffer;
    }
    std::map<const char*, FMOD::Studio::EventDescription*>    eventDescriptions;

    std::map<const char*, std::set<const char*>> eventParameters;

    std::map<const char*, std::vector<FMOD::Studio::EventInstance*>> eventInstances;


    //std::map <std::string, FMOD_STUDIO_PARAMETER_DESCRIPTION> parameterDescriptions;

};
class AudioEngine {

public:
    
    /**
    * Initializes Audio Engine Studio and Core systems
    */
    AudioEngine();

    /**
    * Loads a 2D sound which will playback in tradidional stereo mode (IE, not positional audio)
    * The file is read into the cache to prepare for later playback.
    * Only reads file and creates the sound if it has not already been added to the cache.
    * To play the sound later, use method play2DSoundFile()
    */
    void load2DSoundFile(const char* filepath,bool loop);
    
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
    void play2DSoundFile(const char* filepath);

    void load3DSoundFile(const char* filepath, bool loop);

    void play3DSound(const char* filename, float x, float y, float z);
   
    void update3DSoundPosition(const char* filename, float x, float y, float z);

    /**
    * Sets the position of the listener in the 3D scene.
    * @var posX, posY, posZ - 3D translation of listener
    * @var forwardX, forwardY, forwardZ - forward angle character is looking in
    * @var upX, upY, upZ - up which must be perpendicular to forward vector
    */
    void set3DListenerPosition(float posX,     float posY,     float posZ,
                               float forwardX, float forwardY, float forwardZ,
                               float upX,      float upY,      float upZ);



    void loadFMODStudioBank(const char* filepath) {
        std::cout << "AudioEngine: Loading FMOD Studio Sound Bank " << filepath << '\n';
        FMOD::Studio::Bank* bank = NULL;
        studioSystem->loadBankFile(filepath, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
    }

    
    void loadFMODStudioEvent(const char* eventName, std::vector<std::pair<const char*, float>> paramsValues){ // std::vector<std::map<const char*, float>> perInstanceParameterValues) {
        std::cout << "AudioEngine: Loading FMOD Studio Event " << eventName << '\n';
        FMOD::Studio::EventDescription* eventDescription = NULL;
        studioSystem->getEvent(eventName, &eventDescription);
        // Cache the EventDescription* to create additional event instances later (TODO implement)
        ssCache.addEventDescription(eventName, eventDescription); 
        // For logging, also cache the event parameter names
        //for (const auto& val : paramsValues)
        //    ssCache.addEventParameter(eventName, val.first);

        // Create an instance of the event
        FMOD::Studio::EventInstance* eventInstance = NULL;
        eventDescription->createInstance(&eventInstance);
        for (const auto& parVal : paramsValues) {
            std::cout << "Event Parameter " << parVal.first << " value: " << parVal.second << '\n';
            // Set the parameter values of the event instance
            eventInstance->setParameterByName(parVal.first, parVal.second);
        }
        ssCache.addEventInstance(eventName, eventInstance);

    }



    void setFMODEventParamValue(const char* eventName, const char* parameterName, float value) {
        // get instance
       if(ssCache.hasEventParameter(eventName, parameterName))
          ssCache.setEventInstanceParam(eventName, parameterName, value); // TODO setting instance's param
       
        
    }

    void playEvent(const char* eventName, int instanceIndex = 0) {
        std::cout << "Trying to play event " << eventName << '\n';
        if (ssCache.hasEventInstance(eventName, instanceIndex)) {
            std::cout << "...Found and playing Playing event" << eventName << '\n';
            ssCache.getEventInstance(eventName, instanceIndex)->start();
        }
    }

    void stopEvent(const char* eventName, int instanceIndex = 0) {
        if (ssCache.hasEventInstance(eventName, instanceIndex)) {
            ssCache.getEventInstance(eventName, instanceIndex)->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
        }
    }


private:    

    // FMOD Studio API system, which can play FMOD sound banks (*.bank)
    FMOD::Studio::System* studioSystem = nullptr;       
    // FMOD's low-level audio system which plays audio files and is obtained from Studio System)
    FMOD::System*         lowLevelSystem = nullptr;          
    
    // Max FMOD::Channels for the audio engine 
    static const unsigned int MAX_AUDIO_CHANNELS = 1024; 
    // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.
    const float DISTANCEFACTOR = 1.0f;  
    // Listener position, initialized to default value
    FMOD_VECTOR listenerpos = { 0.0f, 0.0f, -1.0f * DISTANCEFACTOR };
    // Listener forward vector, initialized to default value
    FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
    // Listener upwards vector, initialized to default value
    FMOD_VECTOR up      = { 0.0f, 1.0f, 0.0f };

    /*
    * Map which stores the low-level sound cache. 
    * Key is the relative file path of each unique sound asset. 
    * Value is the FMOD::Sound* to be played back.
    * TODO Refactor to use numeric UID as key
    */
    std::map<std::string, FMOD::Sound*> soundCache;      

    std::map<std::string, FMOD::Channel*> channelMap;

    AEStudioSystemCache ssCache;
    
    
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
 
};
