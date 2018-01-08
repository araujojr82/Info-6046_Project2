/*
Magician
Developed by Euclides Araujo 
and Jorge Amengol
*/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>	// "String stream"

#include <vector>
#include <windows.h>

#include <sapi.h>	//Text to Speech Library
#include <sphelper.h>

#include <fmod.hpp>
#include <fmod_errors.h>

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

const std::string MALE_VOICE = "Male";
const std::string FEMALE_VOICE = "Female";

// Global check variables
bool g_bExit_game = false;
bool g_bStartProgram = false;
bool g_bIsHideModeOn = false;
bool g_bIsTimeToAnser = false;
bool g_bIsShiftPressed = false;
bool g_bIsCapsPressed = false;
bool g_bIsVoiceActive = false;
bool g_bIsPaused = false;
bool g_bIsInstructionsSaved = false;

// All global text variables/vectors/strings
unsigned char theKeyState[256];

std::vector<char> numberMap =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

std::vector<char> keyMap =
{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

std::vector<char> standardLine =
{ 'O', 'h', ' ', 'M', 'i', 's', 't', 'e', 'r', ' ', 'M', 'a', 'g', 'i', 'c', 'i', 'a', 'n', ',',
'c', 'o', 'u', 'l', 'd', ' ', 'y', 'o', 'u', ' ', 'p', 'l', 'e', 'a', 's', 'e', ' ', 't', 'e', 'l', 'l', 
' ', 'm', 'e', ' ', 't', 'h', 'e', ' ', 'a', 'n', 's', 'w', 'e', 'r', ' ', 't', 'o', ' ', 't', 'h', 'i', 's',
' ', 'q', 'u', 'e', 's', 't', 'i', 'o', 'n', ' ', 't', 'h', 'a', 't', ' ', 'I',' ', 'a', 'l', 'w', 'a', 'y', 's',
' ', 'w', 'a', 'n', 't', 'e', 'd', ' ', 't', 'o', ' ', 'k', 'n', 'o', 'w', ',', ' ', 'w', 'h', 'a', 't', ' ', 
'i', 's', ' ', 't', 'h', 'e' };

std::vector<char> theAnswer;

std::vector<std::string> thePreviousLines;

std::vector<char> theCurrentLine;

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

bool checkAnyKeyWasPressed( unsigned char keyState[256] )
{
	// get the state of everykey
	for( int i = 0; i < 256; i++ )
	{
		keyState[i] = GetAsyncKeyState( i );		
	}

	// if any key is pressed, return true
	for( int j = 1; j < 256; j++ )
	{
		if( keyState[j] )
		{
			return true;
		}
	}

	// else, return false
	return false;
}

void clearAllKeys( unsigned char keyState[256] )
{
	unsigned char emptyKeyState[256];

	keyState = emptyKeyState;
}

void checkInput( unsigned char keyState[256] )
{   //Check for Key Inputs

	char newChar;

	if( keyState[VK_ESCAPE] )
	{   // Escape Key
		g_bExit_game = true;
		return;
	}

	if( keyState[VK_BACK] )
	{   // Backspace Key

		int howManyChars = theCurrentLine.size();

		if( howManyChars > 0 )	// Cannot delete if the line is empty!
		{
			// Remove the last character on the current line
			theCurrentLine.pop_back();

			// return the cursor to the beggining of the line
			std::cout << "\r";

			// print a blank line until the last position, 
			// this will clear the last typed char of the screen
			for( int pos = 0; pos != howManyChars; pos++ )
			{
				std::cout << " ";
			}
		}
		return;
	}

	if( keyState[VK_RETURN] )
	{   // Enter Key
		std::string theLine;

		for( int pos = 0; pos != theCurrentLine.size(); pos++ )
		{
			theLine += theCurrentLine[pos];
		}

		thePreviousLines.push_back( theLine );
		theCurrentLine.clear();

		system( "cls" );
		for( int line = 0; line != thePreviousLines.size(); line++ )
		{
			std::cout << thePreviousLines[line] << std::endl;
		}

		g_bIsTimeToAnser = true;
		
		return;
	}

	if( keyState[VK_SHIFT] ||
		keyState[VK_LSHIFT] ||
		keyState[VK_RSHIFT] )
	{
		return;
	}

	if( keyState[VK_LCONTROL] )
	{   // Left Control (Activate or Deactivate the hiden Mode)
		g_bIsHideModeOn = !g_bIsHideModeOn;
		//if( g_bIsHideModeOn )
		//	std::cout << "Hidden Mode is ON" << std::endl;
		//else
		//	std::cout << "Hidden Mode is OFF" << std::endl;
		//return;
	}

	char theTypedChar = NULL;

	for( int key = 0; key != 256; key++ )
	{
		if( keyState[key] )
		{
			if( key == VK_SHIFT ||
				key == VK_LSHIFT ||
				key == VK_RSHIFT )
			{
				continue;
			}

			else if( key == VK_SPACE )
			{   // Space Key
				//theCurrentLine.push_back( ' ' );
				theTypedChar = ' ';
			}

			else if( key == VK_CAPITAL )
			{   // Caps Lock
				g_bIsCapsPressed = !g_bIsCapsPressed;
			}

			// Check for number input
			else if( key >= 48 && key <= 57 )
			{
				//theTypedChar = numberMap[key - 48];
				char numberAsChar = numberMap[key - 48];
				int theNumber = numberAsChar - '0';
				performSoundAction( theNumber );
			}

			// Check for letter input
			else if( key >= 65 && key <= 90 )
			{

				theTypedChar = keyMap[key - 65];
				//If Caps is ON or Shift is pressed, convert to Uppercase
				if( g_bIsCapsPressed || g_bIsShiftPressed )
					theTypedChar = toupper( theTypedChar );
			}

			// Check for all others keys
			else
			{		
				theTypedChar = MapVirtualKey( key, 2 );
				if( theTypedChar != 0 )
				{
					if( g_bIsShiftPressed ){
						if( theTypedChar == '/' ) theTypedChar = '?';
					}
				}
			} // else
		}
	} // for( int key = 0; key != 257; key++ )
	
	// Check to see if we have a valid input char
	if( theTypedChar != NULL )
	{   // This is where the magic happens!!
		if( g_bIsHideModeOn )
		{	// If Hide Mode is On, store the answer and copy the sample phrase to the stream
			if( theCurrentLine.size() < standardLine.size() )
				theCurrentLine.push_back( standardLine[theCurrentLine.size()] );
			else
				theCurrentLine.push_back( ' ' );

			theAnswer.push_back( theTypedChar );
		}
		else
		{
			// If Hide Mode is Off, copy the input to the stream
			theCurrentLine.push_back( theTypedChar );
		}
	}

} // void checkInput...

void printIntro()
{
	std::cout << std::endl;
	std::cout << "                                                               + *      + * _" << std::endl;
	std::cout << "                                                        /\\ * * + * + * +" << std::endl;
	std::cout << "                                          88          <    >+    + *" << std::endl;
	std::cout << "                                          \"\"           \\/\\/" << std::endl;
	std::cout << "                                                        ||" << std::endl;
	std::cout << "BB,dPYba,,adPYba,  ,adPPYYba,  ,adPPYb,dB BB  ,adPPYba, || ,adPPYYba,  Bb,dPPYba, " << std::endl;
	std::cout << "BBP'   \"BB\"    \"Ba \"\"     `YB aB\"    `YBB BB aB\"     \"\" || \"\"     `YB  BBP'   `\"Ba" << std::endl;
	std::cout << "BB      BB      BB ,adPPPPPBB Bb       BB BB Bb         || ,adPPPPPBB  BB       BB" << std::endl;
	std::cout << "BB      BB      BB BB,    ,BB \"Ba,   ,dBB BB \"Ba,   ,aa || BB,    ,BB  BB       BB" << std::endl;
	std::cout << "BB      BB      BB `\"BbbdP\"YB  `\"YbbdP\"YB BB  `\"YbbdB\"' :: `\"BbbdP\"YB  BB       BB" << std::endl;
	std::cout << "                               aa,    ,BB " << std::endl;
	std::cout << "                                \"YBbbdP\" " << std::endl;

	return;
}

void checkShiftState()
{
	short sState2 = GetKeyState( VK_SHIFT );

	g_bIsShiftPressed = false;	

	if( sState2 < 0 )
		g_bIsShiftPressed = true;

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

void showAnswer()
{
	std::string theAnswerText;
	g_bIsTimeToAnser = false;

	// Check to see if the user gave an answer
	if( theAnswer.size() > 0 )
	{	// There's an answer, go on...
		for( int pos = 0; pos != theAnswer.size(); pos++ )
		{
			theAnswerText += theAnswer[pos];
		}
		theAnswer.clear();

		std::string textToBeRead;
		textToBeRead = "The answer to your question is, ";
		textToBeRead += theAnswerText;
		
		if( g_bIsVoiceActive )
		{
			textToSpeech( textToBeRead, MALE_VOICE );
		}
		else
		{
			std::cout << "The answer to your question is: " << theAnswerText;
		}
	}
	else
	{ // There's no answer, give some feedback to the user

		theAnswerText = "I didn't understand, could you rephrase the question, please?";

		if( g_bIsVoiceActive )
		{
			textToSpeech( theAnswerText, MALE_VOICE );
		}
		else
		{
			std::cout << theAnswerText;
		}
	}	
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

void loadFileToVector( std::string fileName, std::vector<std::string> &fileIntoLines )
{
	// TODO change this config formating
	std::ifstream textFile( fileName );
	if( !textFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find text file" << std::endl;
	}
	else
	{	// File DID open, so read it... 
		std::string temp, theLine;

		std::stringstream tempLine;		// Inside "sstream"

		while( !textFile.eof() && textFile.is_open() )
		{
			tempLine.str( "" );

			bool b_KeepReadingLine = true;
			do
			{
				textFile >> temp;		// check if it's the end of the line (;)
				if( temp.back() != ';' )
				{
					tempLine << temp << " ";
				}
				else
				{	// the end of the line 
					temp.pop_back();	// remove the ;
					tempLine << temp;	// add the rest to the line
					b_KeepReadingLine = false;
				}
			} while( b_KeepReadingLine );

			theLine = tempLine.str();;
			fileIntoLines.push_back( theLine );
		}
		textFile.close();  // Close the file
	}
}

void recordInstructions()
{
	std::vector<std::string> textToVoiceLines;

	loadFileToVector( "instructions_text.txt", textToVoiceLines );

	for( int i = 0; i != textToVoiceLines.size(); i++ )
	{
		textToSpeechToFile( textToVoiceLines[i], MALE_VOICE );
	}

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

void playIntro()
{
	std::vector<std::string> textToVoiceLines;
	
	loadFileToVector( "intro_text.txt", textToVoiceLines );
	
	for( int i = 0; i != textToVoiceLines.size(); i++ )
	{
		textToSpeech( textToVoiceLines[i], "" );
	}
	
	return;
}

void playInstructions()
{
	// Play the Instructions from the recorded file
	playSoundOnLoop( 3 ); // 3 is actually the 4th channel
	return;
}


int main()
{
	initVoiceModule();
	initFMOD();
	initStreamingChannels();
	initInstructions();

	printIntro();
	//playIntro();
	
	std::cout << std::endl;
	std::cout << "Press any key to start..." << std::endl;

	while( !g_bStartProgram )
	{
		g_bStartProgram = checkAnyKeyWasPressed( theKeyState );
	}
	clearAllKeys( theKeyState );

	theCurrentLine.clear();	// clear the typing line
	
	// Store the default intro in the line list
	thePreviousLines.push_back( "Hello and welcome to the great magical show, I'm Mr. Magician..." );
	thePreviousLines.push_back( " " );

	system( "cls" );
	for( int line = 0; line != thePreviousLines.size(); line++ )
	{
		std::cout << thePreviousLines[line] << std::endl;
	}

	if( g_bIsVoiceActive )
	{		
		textToSpeech( "Hello and welcome to the great magical show, I'm Mr. Magician", MALE_VOICE );		
	}

	g_bStartProgram = false;
	
	if( g_bIsInstructionsSaved )
	{
		std::cout << "Press any key to stop the instructions...";
		while( !g_bStartProgram )
		{
			playInstructions();
			g_bStartProgram = checkAnyKeyWasPressed( theKeyState );
		}
		
		std::cout << "\r";		
		std::cout << "                                         ";
		std::cout << "\r";
		std::cout << "What is that you wish to know?" << std::endl;
		thePreviousLines.push_back( "What is that you wish to know?" );
		clearAllKeys( theKeyState );
	}

	//char theTypedChar;

	while( !g_bExit_game )
	{
		checkShiftState();
		startChannels();

		//std::string status;
		//if( g_bIsShiftPressed ) status = "ON";
		//else status = "OFF";

		//std::cout << status << std::endl;

		//Sleep( 100 );

		if( checkAnyKeyWasPressed( theKeyState ) )
		{					
			//std::cout << "Some key was pressed!" << std::endl;
			checkInput( theKeyState );

			// return the cursor to the beggining of the line
			std::cout << "\r";

			// check to see if the user pressed Enter, and display the answer
			if( g_bIsTimeToAnser ) showAnswer();

			// in every loop step we print the current line over again
			for( int pos = 0; pos != theCurrentLine.size(); pos++ )
			{
				std::cout << theCurrentLine[pos];
			}
		}
	}
	
	// Release the voice objects
	pVoice->Release();
	pVoice = NULL;
	::CoUninitialize();

	return 0;
}