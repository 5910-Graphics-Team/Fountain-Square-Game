#pragma once

typedef enum {
    SOUND_LOOP,
    SOUND_ONE_SHOT,
} SOUND_PLAYBACK_TYPE;

typedef enum {
    SOUND_2D,
    SOUND_3D
    // TODO SOUND_2D_MONO, SOUND_2D_STEREO, SOUND_3D_MONO, SOUND_3D_STEREO
} SOUND_POSITION_TYPE;

typedef enum {
    SOUND_NOT_LOADED,
    SOUND_LOADED
} SOUND_LOAD_INFO;

/**
 * Holds the information about a raw audio file needed by the Audio Engine
 * to load, configure and play a sound with user-controlled settings.
 */
class SoundInfo {
public:

    /**
     * SoundInfo main constructor
     * TODO Update Doc
     * @var filepath (default = "")    Relative location of audio file
     * @var reverbAmount
     * @var soundPlaybackType
     * @var soundPositionType 
     * @var x        (default = 0.0f)  X coordinate - only used when @var is3D is true
     * @var y        (default = 0.0f)  Y coordinate - only used when @var is3D is true
     * @var z        (default = 0.0f)  Z coordinate - only used when @var is3D is true
     */
    SoundInfo(const char* filePath, float volume = 1.0f, float reverbAmount = 0.0f, SOUND_PLAYBACK_TYPE soundPlaybackType = SOUND_ONE_SHOT, SOUND_POSITION_TYPE soundPositionType = SOUND_2D,
        float x = 0.0f, float y = 0.0f, float z = 0.0f) : filePath(filePath), volume(volume), reverbAmount(reverbAmount), x(x), y(y), z(z) {
        uniqueID = filePath; // for now, filepath is unique id TODO generate uid based on instance number of sound
        this->soundPlaybackType = soundPlaybackType;
        this->soundPositionType = soundPositionType;
        this->soundLoadInfo = SOUND_NOT_LOADED;
    }
    

    bool isLoop() {
        return soundPlaybackType == SOUND_LOOP;
    }

    bool is3D() {
        return soundPositionType == SOUND_3D;
    }

    bool isLoaded() {
        return soundLoadInfo == SOUND_LOADED;
    }

    float getX() {
        return x;
    }
    float getY() {
        return y;
    }
    float getZ() {
        return z;
    }

    /**
     * Sets the 3D coordinates of the sound
     */
    void set3DCoords(float x, float y, float z) {
        this->x = x, this->y = y, this->z = z;
    }

    const char* getFilePath() {
        return filePath;
    }

    std::string getUniqueID() {
        return uniqueID;
    }

    float getReverbAmount() {
        return reverbAmount;
    }
    
	float getVolume() {
		return volume;
	}

    void setLoaded(SOUND_LOAD_INFO loadInfo) {
        this->soundLoadInfo = loadInfo;
    }
    
    void setVolume(float vol) {
        this->volume = vol;
    }
    // TODO FIX
    void setMSLength(unsigned int msLength) {
        this->msLength = msLength;
        //this->msLength = new unsigned int(msLength);
    }
    // TODO FIX
    const unsigned int getMSLength() {
        return msLength;
    }

private:
    std::string uniqueID;
    const char* filePath;


    SOUND_POSITION_TYPE soundPositionType;
    SOUND_PLAYBACK_TYPE soundPlaybackType;
    SOUND_LOAD_INFO     soundLoadInfo;

    
    float reverbAmount;
    float volume; // 0 to 1
    float x, y, z;

    // Calculated and set during loading by the audio engine
	unsigned int msLength;


    // TODO implement sound instancing
    // int instanceID = -1;
};