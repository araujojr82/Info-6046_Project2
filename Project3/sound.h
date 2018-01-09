#ifndef _sound_HG_
#define _sound_HG_

#include <string>

#include <sapi.h>	//Text to Speech Library
#include <sphelper.h>
#include <fmod.hpp>
#include <fmod_errors.h>

bool isVoiceActive();
void checkErrorFMOD( FMOD_RESULT result );
void initFMOD();
void initStreamingChannels();
void startChannels();
void playPauseChannel( int channel_number );
void playSoundOnLoop( int index );
void startRecording( int index );
void enableDSP( int dspNumber );
void changePitch( float amount );
void performSoundAction( int action );
void textToSpeech( std::string textToBeRead, std::string gender );
void initVoiceModule();
void textToSpeechToFile( std::string textToBeRead, std::string gender );
void initInstructions();
void releaseSounds();

#endif