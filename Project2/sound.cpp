#include "sound.h"
#include "utils.h"

extern bool g_bIsInstructionsSaved;

// FMOD variables & constants
FMOD_RESULT mresult;
FMOD::System *msystem = NULL;

const int NUMBER_OF_SOUNDS = 4;
const int STREAM_BUFFER_SIZE = 65536;

FMOD::Sound *msounds[NUMBER_OF_SOUNDS];
FMOD::Channel *mchannels[NUMBER_OF_SOUNDS];

//Master channel group
FMOD::ChannelGroup *mastergroup = 0;

//DSP variables
FMOD::DSP          *dsphighpass = 0;
FMOD::DSP          *dspecho = 0;
FMOD::DSP          *dspflange = 0;

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
	unsigned int l_curPos = 0;
	unsigned int l_lenght = 0;

	// Check to see if the channel is already playing
	mresult = mchannels[index]->getPaused( &g_bIsPaused );
	if( mresult == FMOD_OK )
	{
		mresult = mchannels[index]->getPosition( &l_curPos, FMOD_TIMEUNIT_MS );
		checkErrorFMOD( mresult );

		mresult = msounds[index]->getLength( &l_lenght, FMOD_TIMEUNIT_MS );
		checkErrorFMOD( mresult );

		if( l_curPos == l_lenght )
		{	// Channel is playing, but reaches the end, PLAY AGAIN
			mresult = msystem->playSound( msounds[index], 0, false, &mchannels[index] );
			checkErrorFMOD( mresult );
		}
	}
	else
	{   // Isn't playing, start playing

		//Get master channel group
		mresult = msystem->getMasterChannelGroup( &mastergroup );
		checkErrorFMOD( mresult );

		//Create DSP effects
		mresult = msystem->createDSPByType( FMOD_DSP_TYPE_HIGHPASS, &dsphighpass );
		checkErrorFMOD( mresult );
		mresult = msystem->createDSPByType( FMOD_DSP_TYPE_ECHO, &dspecho );
		checkErrorFMOD( mresult );
		mresult = msystem->createDSPByType( FMOD_DSP_TYPE_FLANGE, &dspflange );
		checkErrorFMOD( mresult );

		//Add effects to master channel group.
		mresult = mastergroup->addDSP( 0, dsphighpass );
		checkErrorFMOD( mresult );
		mresult = mastergroup->addDSP( 0, dspecho );
		checkErrorFMOD( mresult );
		mresult = mastergroup->addDSP( 0, dspflange );
		checkErrorFMOD( mresult );

		//Bypass all effects, this plays the sound with no effects.
		mresult = dsphighpass->setBypass( true );;
		checkErrorFMOD( mresult );
		mresult = dspecho->setBypass( true );
		checkErrorFMOD( mresult );
		mresult = dspflange->setBypass( true );
		checkErrorFMOD( mresult );

		mresult = msystem->createSound( mpath_recorded_intro, FMOD_CREATESTREAM, 0, &msounds[index] );
		checkErrorFMOD( mresult );

		mresult = msystem->playSound( msounds[index], 0, false, &mchannels[index] );
		checkErrorFMOD( mresult );
	}
}

void enableDSP( int dspNumber )
{
	bool bypass;
	switch( dspNumber )
	{
	case 1:
		mresult = dsphighpass->getBypass( &bypass );
		checkErrorFMOD( mresult );
		mresult = dsphighpass->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	case 2:
		mresult = dspecho->getBypass( &bypass );
		checkErrorFMOD( mresult );
		mresult = dspecho->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	case 3:
		mresult = dspflange->getBypass( &bypass );
		checkErrorFMOD( mresult );
		mresult = dspflange->setBypass( !bypass );
		checkErrorFMOD( mresult );
		break;
	default:
		break;
	}

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
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
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

void releaseSound()
{
	// Release the voice objects
	pVoice->Release();
	pVoice = NULL;
	::CoUninitialize();
}