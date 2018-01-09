#include "sound.h"
#include "utils.h"

extern bool g_bIsInstructionsSaved;

// FMOD variables & constants
FMOD_RESULT mresult;
FMOD::System *msystem = NULL;

const int NUMBER_OF_SOUNDS = 5;
const int STREAM_BUFFER_SIZE = 65536;
const int DEVICE_INDEX = 0;

FMOD::Sound *msounds[NUMBER_OF_SOUNDS];
FMOD::Channel *mchannels[NUMBER_OF_SOUNDS];

//Master channel group
//FMOD::ChannelGroup *mastergroup = 0;
FMOD::ChannelGroup *group1 = 0;
FMOD::ChannelGroup *group2 = 0;

//DSP variables
FMOD::DSP          *dspHighpass = 0;
FMOD::DSP          *dspEcho = 0;
FMOD::DSP          *dspFlange = 0;

FMOD::DSP          *dspDistortion = 0;
FMOD::DSP          *dspPitchshift = 0;
FMOD::DSP          *dspTremolo = 0;

// Create openstate
FMOD_OPENSTATE mopenstate = FMOD_OPENSTATE_READY;

// Radio stations URLs
char *murl_classic_rock = "http://78.129.202.200:8040";
char *murl_classic_jazz = "http://37.187.57.33:8028";
char *murl_classic_video_game = "http://74.207.229.209:8010/classic/";
char *mpath_recorded_intro = "c:/araujo_euclides/project3.wav";

// Voice objects
ISpVoice *pVoice = NULL;
HRESULT hr = NULL;
HRESULT hr2 = NULL;

CComPtr<ISpVoice> cpVoice;
CComPtr<ISpStream> cpStream;
CSpStreamFormat	cAudioFormat;

bool g_bIsVoiceActive = false;
bool g_bIsPaused = false;
//bool g_bIsReverbEnable = false;
bool g_bIsRecording = false;

float pitchValue = 0.8f;

bool isVoiceActive()
{
	return g_bIsVoiceActive;
}

void checkErrorFMOD( FMOD_RESULT result )
{   // Check for errors in the FMOD_RESULT

	if( result != FMOD_OK )
	{
		fprintf( stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString( result ) );
		exit( -1 );
	}

	return;
}

void initFMOD()
{
	// Create the main system object.
	mresult = FMOD::System_Create( &msystem );
	checkErrorFMOD( mresult );

	//Initializes the system object, and the msound device. This has to be called at the start of the user's program
	mresult = msystem->init( 512, FMOD_INIT_NORMAL, 0 );    // Initialize FMOD.
	checkErrorFMOD( mresult );

	//Increase internal buffersize for streams opened to account for Internet lag, default is 16384 
	mresult = msystem->setStreamBufferSize( STREAM_BUFFER_SIZE, FMOD_TIMEUNIT_RAWBYTES );
	checkErrorFMOD( mresult );

	return;
}

void initStreamingChannels()
{
	// Create the 3 Sounds
	mresult = msystem->createSound( murl_classic_video_game, FMOD_CREATESTREAM | FMOD_NONBLOCKING, 0, &msounds[0] );
	checkErrorFMOD( mresult );
	mresult = msystem->createSound( murl_classic_rock, FMOD_CREATESTREAM | FMOD_NONBLOCKING, 0, &msounds[1] );
	checkErrorFMOD( mresult );
	mresult = msystem->createSound( murl_classic_jazz, FMOD_CREATESTREAM | FMOD_NONBLOCKING, 0, &msounds[2] );
	checkErrorFMOD( mresult );
	return;
}

void startChannels()
{
	bool bPaused = true;

	//Important to update msystem
	mresult = msystem->update();
	checkErrorFMOD( mresult );

	// If channel is not started, start the streaming channel
	// Channel 1 (0) is started in play, the others in paused
	for( int i = 0; i != NUMBER_OF_SOUNDS; i++ )
	{
		//if( i == 0 ) bPaused = false;
		//else bPaused = true;

		if( !mchannels[i] )
		{
			msystem->playSound( msounds[i], 0, bPaused, &mchannels[i] );
		}
	}

	return;
}

void playPauseChannel( int channel_number )
{
	mresult = mchannels[channel_number]->getPaused( &g_bIsPaused );
	checkErrorFMOD( mresult );

	mresult = mchannels[channel_number]->setPaused( !g_bIsPaused );
	checkErrorFMOD( mresult );
	return;
}

void playSoundOnLoop( int index )
{
	mresult = msystem->createChannelGroup( "group1", &group1 );
	checkErrorFMOD( mresult );

	//Create DSP effects
	mresult = msystem->createDSPByType( FMOD_DSP_TYPE_HIGHPASS, &dspHighpass );
	checkErrorFMOD( mresult );
	mresult = msystem->createDSPByType( FMOD_DSP_TYPE_ECHO, &dspEcho );
	checkErrorFMOD( mresult );
	mresult = msystem->createDSPByType( FMOD_DSP_TYPE_FLANGE, &dspFlange );
	checkErrorFMOD( mresult );

	//Add effects to the channel group.
	mresult = group1->addDSP( 0, dspHighpass );
	checkErrorFMOD( mresult );
	mresult = group1->addDSP( 0, dspEcho );
	checkErrorFMOD( mresult );
	mresult = group1->addDSP( 0, dspFlange );
	checkErrorFMOD( mresult );

	//Bypass all effects, this plays the sound with no effects.
	mresult = dspHighpass->setBypass( true );;
	checkErrorFMOD( mresult );
	mresult = dspEcho->setBypass( true );
	checkErrorFMOD( mresult );
	mresult = dspFlange->setBypass( true );
	checkErrorFMOD( mresult );

	mresult = msystem->createSound( mpath_recorded_intro, FMOD_LOOP_NORMAL, 0, &msounds[index] );
	checkErrorFMOD( mresult );
		
	mresult = msystem->playSound( msounds[index], 0, false, &mchannels[index] );
	checkErrorFMOD( mresult );

	// Set the Sound to Channel Group1
	mresult = mchannels[index]->setChannelGroup( group1 );
	checkErrorFMOD( mresult );
}

void startRecording( int index )
{
	int numOfDrivers = 0;
	int numOfTries = 20;
	int nativeRate = 0;
	int nativeChannels = 0;

	// Identify recording drivers in your pc
	while( numOfTries > 0 && numOfDrivers <= 0 )
	{
		numOfTries--;	
		Sleep( 100 );	// Give some time before next try
		mresult = msystem->getRecordNumDrivers( 0, &numOfDrivers );
		checkErrorFMOD( mresult );
	}

	if( numOfDrivers == 0 )	// If no recording drivers are found
		return;				// Skip this function

	// Get recording driver info
	mresult = msystem->getRecordDriverInfo( DEVICE_INDEX, NULL, 0, NULL, &nativeRate, NULL, &nativeChannels, NULL );
	checkErrorFMOD( mresult );

	// Create user sound
	FMOD_CREATESOUNDEXINFO exinfo = { 0 };
	exinfo.cbsize = sizeof( FMOD_CREATESOUNDEXINFO );
	exinfo.numchannels = nativeChannels;
	exinfo.format = FMOD_SOUND_FORMAT_PCM16;
	exinfo.defaultfrequency = nativeRate;
	exinfo.length = nativeRate * sizeof( short ) * nativeChannels;

	// Create the Channel Group
	mresult = msystem->createChannelGroup( "group2", &group2 );
	checkErrorFMOD( mresult );
	
	// Create DSP effects
	mresult = msystem->createDSPByType( FMOD_DSP_TYPE_DISTORTION, &dspDistortion );
	checkErrorFMOD( mresult );
	mresult = msystem->createDSPByType( FMOD_DSP_TYPE_PITCHSHIFT, &dspPitchshift );
	checkErrorFMOD( mresult );
	mresult = msystem->createDSPByType( FMOD_DSP_TYPE_TREMOLO, &dspTremolo );
	checkErrorFMOD( mresult );

	mresult = dspPitchshift->setParameterFloat( 0, pitchValue );
	checkErrorFMOD( mresult );

	// Add effects to the channel group.
	mresult = group1->addDSP( 0, dspDistortion );
	checkErrorFMOD( mresult );
	mresult = group1->addDSP( 0, dspPitchshift );
	checkErrorFMOD( mresult );
	mresult = group1->addDSP( 0, dspTremolo );
	checkErrorFMOD( mresult );

	// Bypass all effects, this plays the sound with no effects.
	mresult = dspDistortion->setBypass( true );;
	checkErrorFMOD( mresult );
	mresult = dspPitchshift->setBypass( true );
	checkErrorFMOD( mresult );
	mresult = dspTremolo->setBypass( true );
	checkErrorFMOD( mresult );

	mresult = msystem->createSound( 0, FMOD_LOOP_NORMAL | FMOD_OPENUSER, &exinfo, &msounds[index] );
	checkErrorFMOD( mresult );

	// Start the recording
	mresult = msystem->recordStart( DEVICE_INDEX, msounds[index], true );
	checkErrorFMOD( mresult );

	// Play the recording sound over
	mresult = msystem->playSound( msounds[index], 0, false, &mchannels[index] );
	checkErrorFMOD( mresult );

	// Set the recording sound to Channel Group2
	mresult = mchannels[index]->setChannelGroup( group2 );
	checkErrorFMOD( mresult );

	g_bIsRecording = true;

	return;

}

void stopRecording()
{
	mresult = msystem->recordStop( DEVICE_INDEX );
	g_bIsRecording = false;
	return;
}


void enableDSP( int dspNumber )
{
	bool bypass;
	switch( dspNumber )
	{
	case 1:
		mresult = dspHighpass->getBypass( &bypass );
		checkErrorFMOD( mresult );
		mresult = dspHighpass->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	case 2:
		mresult = dspEcho->getBypass( &bypass );
		checkErrorFMOD( mresult );
		mresult = dspEcho->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	case 3:
		mresult = dspFlange->getBypass( &bypass );
		checkErrorFMOD( mresult );
		mresult = dspFlange->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	case 5:			
		mresult = dspDistortion->getBypass( &bypass );
		checkErrorFMOD( mresult );
		mresult = dspDistortion->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	case 6:
		mresult = dspPitchshift->getBypass( &bypass );
		checkErrorFMOD( mresult );

		mresult = dspPitchshift->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	case 7:
		mresult = dspTremolo->getBypass( &bypass );
		checkErrorFMOD( mresult );
		mresult = dspTremolo->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	default:
		break;
	}

	return;
}

void changePitch( float amount )
{
	pitchValue += amount;

	mresult = dspPitchshift->setParameterFloat( 0, pitchValue );
	checkErrorFMOD( mresult );
	return;
}

void performSoundAction( int action )
{
	switch( action )
	{
	case 1:
		playPauseChannel( 0 );
		break;
	case 2:
		playPauseChannel( 1 );
		break;
	case 3:
		playPauseChannel( 2 );
		break;
	case 4:
		if( !g_bIsRecording ) startRecording( 4 );
		else stopRecording();
		
		break;
	case 5:
		enableDSP( 5 );
		break;
	case 6:
		enableDSP( 6 );
		break;
	case 7:
		enableDSP( 7 );
		break;
	case 8:
		enableDSP( 1 );
		break;
	case 9:
		enableDSP( 2 );
		break;
	case 0:
		enableDSP( 3 );
		break;
	default:
		break;
	}

	return;
}

void textToSpeech( std::string textToBeRead, std::string gender )
{
	std::string textWithParameters;

	textWithParameters = "";

	// Assemble the Text to be read with the function parameters
	if( gender != "" )
	{
		textWithParameters += "<voice required='Gender = ";
		textWithParameters += gender;
		textWithParameters += "'>";
	}
	textWithParameters += textToBeRead;
	textWithParameters += "</voice>";

	// Convert the Strint to WString so we can use the c_str() function
	std::wstring textToWString( textWithParameters.begin(), textWithParameters.end() );

	// Convert the WString to LPCWSTR
	LPCWSTR theText = textToWString.c_str();

	// Call the voice module with the text to be read
	hr = pVoice->Speak( theText, 0, NULL );

	return;
}

void initVoiceModule()
{
	//Initialize the COM library on the current thread
	if( FAILED( ::CoInitialize( NULL ) ) ) {
		g_bIsVoiceActive = false;
		return;
	}

	//Initialize voice.
	hr = CoCreateInstance( CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, ( void ** )&pVoice );

	// If successfully initialized, return true, otherwise return false;
	if( SUCCEEDED( hr ) ) g_bIsVoiceActive = true;
	else g_bIsVoiceActive = false;

	return;
}

void textToSpeechToFile( std::string textToBeRead, std::string gender )
{
	// Convert the Strint to WString so we can use the c_str() function
	std::wstring textToWString( textToBeRead.begin(), textToBeRead.end() );

	// Convert the WString to LPCWSTR
	LPCWSTR theText = textToWString.c_str();

	// Call the voice module with the text to be read and saved to file
	hr2 = cpVoice->Speak( theText, SPF_DEFAULT, NULL );

	return;
}

void initInstructions()
{
	// Create voice
	hr2 = cpVoice.CoCreateInstance( CLSID_SpVoice );

	// Set audio format	
	if( SUCCEEDED( hr2 ) ) {
		hr2 = cAudioFormat.AssignFormat( SPSF_22kHz16BitStereo );
	}

	// Bind stream to file
	if( SUCCEEDED( hr2 ) ) {
		hr2 = SPBindToFile( L"c:\\araujo_euclides\\project3.wav", SPFM_CREATE_ALWAYS, &cpStream, &cAudioFormat.FormatId(), cAudioFormat.WaveFormatExPtr() );
	}

	// Set output to cpstream
	if( SUCCEEDED( hr2 ) ) {
		hr2 = cpVoice->SetOutput( cpStream, true );
	}

	// Record the text to file as speak
	if( SUCCEEDED( hr2 ) ) {
		recordInstructions();
		//hr2 = cpVoice->Speak( L"Hello media fundamentals this is second example, saving into a wav file!", SPF_DEFAULT, NULL );
	}

	//TODO: close the stream
	if( SUCCEEDED( hr ) ) {
		hr2 = cpStream->Close();

		g_bIsInstructionsSaved = true;
	}
	else
	{
		g_bIsInstructionsSaved = false;
	}

	return;
}

void releaseSounds()
{
	// Release the voice objects
	pVoice->Release();
	pVoice = NULL;
	::CoUninitialize();
}