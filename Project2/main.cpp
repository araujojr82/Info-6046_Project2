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

bool getConsoleChar( KEY_EVENT_RECORD& krec )
{
	DWORD cc;
	INPUT_RECORD irec;
	HANDLE h = GetStdHandle( STD_INPUT_HANDLE );

	if( h == NULL )
	{
		return false; // console not found
	}

	for( ; ; )
	{
		ReadConsoleInput( h, &irec, 1, &cc );
		if( irec.EventType == KEY_EVENT && ( ( KEY_EVENT_RECORD& )irec.Event ).bKeyDown )
		{
			krec = ( KEY_EVENT_RECORD& )irec.Event;
			return true;
		}
	}
	return false;
}


bool checkInput( KEY_EVENT_RECORD &key )
{
	getConsoleChar( key );

	if( key.wVirtualKeyCode != NULL )
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

bool handleInputForInstructions( KEY_EVENT_RECORD &key )
{
	char theChar = key.uChar.AsciiChar;

	if( key.wVirtualKeyCode == VK_MENU )
	{
		return false;
	}

	// Check for number Input 
	if( theChar >= 48 && theChar <= 57 )
	{
		// Check if Alt was pressed, so there's no input in this case
		if( key.dwControlKeyState == LEFT_ALT_PRESSED ||
			key.dwControlKeyState == RIGHT_ALT_PRESSED )
		{
			int theNumber = theChar - '0';
			performSoundAction( theNumber );
			return false;
		}
		else
			return true;
	}

	return true;
}

void handleInput( KEY_EVENT_RECORD &key )
{
	switch( key.wVirtualKeyCode )
	{
	case VK_SHIFT:
		return;
		break;
	case VK_CAPITAL:
		return;
		break;
	case VK_MENU:	//The ALT key
		return;
		break;
	case VK_TAB:
		return;
		break;
	case VK_ESCAPE:	// Exit the application
		g_bExit_game = true;
		break;
	case VK_RETURN:	// Start the answer mode
		{
			std::string theLine;

			for( int pos = 0; pos != theCurrentLine.size(); pos++ )		
				theLine += theCurrentLine[pos];

			thePreviousLines.push_back( theLine ); //Store this question
			theCurrentLine.clear();

			system( "cls" );
			for( int line = 0; line != thePreviousLines.size(); line++ )
				std::cout << thePreviousLines[line] << std::endl;

			g_bIsTimeToAnser = true;
		}				
		return;
		break;
	case VK_CONTROL: // ON / OFF the hide mode
		g_bIsHideModeOn = !g_bIsHideModeOn;
		//if( g_bIsHideModeOn )
		//	std::cout << "Hidden Mode is ON" << std::endl;
		//else
		//	std::cout << "Hidden Mode is OFF" << std::endl;
		return;
		break;
	case VK_BACK:
		{
			int howManyChars = theCurrentLine.size();

			if( howManyChars > 0 )	// Cannot delete if the line is empty!
			{	// Remove the last character on the current line
				theCurrentLine.pop_back();

				// return the cursor to the beggining of the line
				std::cout << "\r";

				// print a blank line until the last position, 
				// this will clear the last typed char of the screen
				for( int pos = 0; pos != howManyChars; pos++ )
					std::cout << " ";
			}
		}
		return;
		break;
	default:	// Every other key

		break;
	}

	char theChar = key.uChar.AsciiChar;

	// Check for number Input 
	if( theChar >= 48 && theChar <= 57 )
	{
		// Check if Alt was pressed, so there's no input in this case
		if( key.dwControlKeyState == LEFT_ALT_PRESSED ||
			key.dwControlKeyState == RIGHT_ALT_PRESSED )
		{
			int theNumber = theChar - '0';
			performSoundAction( theNumber );
			return;
		}
	}

	// Still here? Let's treat the input
	theChar = key.uChar.AsciiChar;
	if( theChar != 0 )
	{	// It's a valid input, store this		
		if( g_bIsHideModeOn )	// This is where the magic happens!!
		{	// If Hide Mode is On, store it in the answer variable...
			// and copy the sample phrase to the stream
			if( theCurrentLine.size() < standardLine.size() )
				theCurrentLine.push_back( standardLine[theCurrentLine.size()] );
			else
				theCurrentLine.push_back( ' ' );
		
			theAnswer.push_back( theChar );
		}
		else
		{ // If Hide Mode is Off, copy the input to the stream
			theCurrentLine.push_back( theChar );
		}

	}

	return;
}

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
			textToSpeech( textToBeRead, MALE_VOICE );
		else
			std::cout << "The answer to your question is: " << theAnswerText;
	}
	else
	{ // There's no answer, give some feedback to the user
		theAnswerText = "I didn't understand, could you rephrase the question, please?";

		if( isVoiceActive() )
			textToSpeech( theAnswerText, MALE_VOICE );
		else
			std::cout << theAnswerText;		
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
	KEY_EVENT_RECORD theKey;

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
		g_bStartProgram = checkInput( theKey );
	}	

	//theCurrentLine.clear();	// clear the typing line
	
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

	// g_bIsInstructionsSaved = false;
	if( g_bIsInstructionsSaved )
	{
		playInstructions();

		bool bStopInstructions = false;
		std::cout << "Press any key to stop the instructions...";
		while( !bStopInstructions )
		{			
			if( checkInput( theKey ) )
			{
				bStopInstructions = handleInputForInstructions( theKey );
			}
		}	
		playPauseChannel( 3 );
		std::cout << "\r";		
		std::cout << "                                         ";
		std::cout << "\r";
		std::cout << "What is that you wish to know?" << std::endl;
		thePreviousLines.push_back( "What is that you wish to know?" );
	}
	else
	{
		std::cout << "What is that you wish to know?" << std::endl;
		thePreviousLines.push_back( "What is that you wish to know?" );
	}

	while( !g_bExit_game )
	{
		startChannels();

		if( checkInput( theKey ) )
		{					
			handleInput( theKey );
			
			std::cout << "\r";				// return the cursor to the beggining of the line

			// check to see if the user pressed Enter, and display the answer
			if( g_bIsTimeToAnser ) showAnswer(); 

			// in every loop step we print the current line over again
			for( int pos = 0; pos != theCurrentLine.size(); pos++ )
				std::cout << theCurrentLine[pos];
		}
	}
	
	releaseSound();

	return 0;
}