/*
Magician
Developed by Euclides Araujo 
and Jorge Amengol
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>	// "String stream"
#include <windows.h>

#include "global_stuff.h"
#include "utils.h"
#include "sound.h"

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
		
		if( isVoiceActive() )
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

		if( isVoiceActive() )
		{
			textToSpeech( theAnswerText, MALE_VOICE );
		}
		else
		{
			std::cout << theAnswerText;
		}
	}	
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
	playIntro();
	
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

	if( isVoiceActive() )
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
	
	releaseSound();

	return 0;
}