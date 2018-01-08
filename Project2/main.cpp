#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <sapi.h>

//TODO: Create voice object.
ISpVoice *pVoice = NULL;
HRESULT hr = NULL;

bool g_exit_game = false;
bool g_bStartProgram = false;
bool g_bIsHideModeOn = false;
bool g_bIsTimeToAnser = false;
bool g_bIsShiftPressed = false;
bool g_bIsCapsPressed = false;
bool g_bIsVoiceActive = false;

const std::string g_voiceGender = "Male";
//const std::string g_voiceGender = "Female";

unsigned char theKeyState[256];

std::vector<char> numberMap =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

std::vector<char> keyMap =
{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

std::vector<char> standardLine =
{ 'O', 'h', ' ', 'M', 'i', 's', 't', 'e', 'r', ' ', 'M', 'a', 'g', 'i', 'c', 'i', 'a', 'n', ',' };

std::vector<char> theAnswer;

std::vector<std::string> thePreviousLines;

std::vector<char> theCurrentLine;

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

void checkInput( unsigned char keyState[256] )
{   //Check for Key Inputs

	char newChar;

	if( keyState[VK_ESCAPE] )
	{   // Escape Key
		g_exit_game = true;
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
				theTypedChar = numberMap[key - 48];
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
	{
		if( g_bIsHideModeOn )
		{	// If Hide Mode is On, store the answer and copy the sample phrase to the stream
			theCurrentLine.push_back( standardLine[theCurrentLine.size()] );
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
	std::cout << std::endl;
	std::cout << "Press any key to start..." << std::endl;
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

void textToSpeech( std::string textToBeRead )
{
	std::string textWithParameters;

	// Assemble the Text to be read with the function parameters
	// such as voice gender
	textWithParameters = "<voice required='Gender = ";
	textWithParameters += g_voiceGender;
	textWithParameters += "'>";
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
			textToSpeech( theAnswerText );
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
			textToSpeech( theAnswerText );
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


int main()
{
	initVoiceModule();

	printIntro();

	while( !g_bStartProgram )
	{
		g_bStartProgram = checkAnyKeyWasPressed( theKeyState );
	}

	theCurrentLine.clear();	// clear the typing line
	
	// Store the default intro in the line list
	thePreviousLines.push_back( "Welcome to Magician!" );
	thePreviousLines.push_back( "I can answer all your questions by using only my great magical skills..." );
	thePreviousLines.push_back( " " );
	thePreviousLines.push_back( "What is that you wish to know?" );

	system( "cls" );
	for( int line = 0; line != thePreviousLines.size(); line++ )
	{
		std::cout << thePreviousLines[line] << std::endl;
	}

	if( g_bIsVoiceActive )
	{
		textToSpeech( "Welcome to Magician!" );
		textToSpeech( "I can answer all your questions by using only my great magical skills..." );
		textToSpeech( "What is that you wish to know?" );
	}

	char theTypedChar;

	while( !g_exit_game )
	{
		checkShiftState();

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