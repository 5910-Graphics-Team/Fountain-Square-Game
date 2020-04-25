//#include "AudioEngine.h"
//
//
//
//AudioEngine::AudioEngine() {
//    FMOD::System_Create(&sys);
//    sys->init(AE_MAX_AUDIO_CHANNELS, FMOD_INIT_NORMAL, 0);
//}
//
//
//
//void AudioEngine::playSoundFMOD(const char* filename, bool loop = false, int nLoops = 1) {
//    // sound    
//    FMOD::Sound* sound;
//    // the sound's channel
//    FMOD::Channel* channel;
//    // create sound
//    sys->createSound(filename, loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, 0, &sound);
//    // assign channel and play sound in 'paused' mode
//    sys->playSound(sound, 0, true /*start paused*/, &channel);
//    // actually start the sound
//    channel->setPaused(false);
//
//    // calculate playback time needed (not the'default' way to achieve looping audio playback with FMOD...)
//    unsigned int msLen;
//    sound->getLength(&msLen, FMOD_TIMEUNIT_MS);
//    // sleep main thread for total amount of audio playback time
//
//}