#pragma once


typedef enum {
    SOUND_LOOP,
    SOUND_ONE_SHOT,
} SOUND_PLAYBACK_TYPE;

typedef enum {
    SOUND_2D,
    SOUND_3D,
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
     * @var isLoop   (default = false) For sound to repeat when playback reaches end of file, pass in true.
     * @var is3D     (default = false) For sound be positioned in 3D space, pass in true.
     * @var x        (default = 0.0f)  X coordinate - only used when @var is3D is true
     * @var y        (default = 0.0f)  Y coordinate - only used when @var is3D is true
     * @var z        (default = 0.0f)  Z coordinate - only used when @var is3D is true
     */
    SoundInfo(const char* filePath, float reverbAmount = 0.0f, SOUND_PLAYBACK_TYPE soundPlaybackType = SOUND_ONE_SHOT, SOUND_POSITION_TYPE soundPositionType = SOUND_2D,
        float x = 0.0f, float y = 0.0f, float z = 0.0f) : filePath(filePath), reverbAmount(reverbAmount), x(x), y(y), z(z) {
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

    void setLoaded(bool loaded) {
        loaded = loaded;
    }



private:

    std::string uniqueID;
    const char* filePath;


    SOUND_POSITION_TYPE soundPositionType;
    SOUND_PLAYBACK_TYPE soundPlaybackType;
    SOUND_LOAD_INFO     soundLoadInfo;


    float reverbAmount;

    float x, y, z;


    // TODO implement sound instancing
    // int instanceID = -1;
};