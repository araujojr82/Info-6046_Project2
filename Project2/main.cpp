#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

bool g_exit_game = false;
bool g_bStartProgram = false;
bool g_bIsHideModeOn = false;
bool g_bNewInput = false;
bool g_bIsShiftPressed = false;
bool g_bIsCapsPressed = false;

unsigned char theKeyState[256];

std::vector<char> numberMap =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

std::vector<char> keyMap =
{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

std::vector<char> standardLine =
{ 'O', 'h', ' ', 'M', 'i', 's', 't', 'e', 'r', ' ', 'M', 'a', 'g', 'i', 'c', 'i', 'a', 'n', ',' };

std::vector<std::string> thePreviousLines;

std::vector<char> theCurrentLine;

bool checkAnyKeyWasPressed( unsigned char keyState[256] )
{
	//Sleep( 100 );

	//unsigned char keyState[256];

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

	if( keyState[VK_ESCAPE] )
		//if( GetAsyncKeyState( VK_ESCAPE ) )
	{   // Escape Key
		g_exit_game = true;
		g_bNewInput = true;
		return;
	}

	if( keyState[VK_CAPITAL] )
	{   // Caps Lock
		g_bIsCapsPressed = !g_bIsCapsPressed;
	}

	//if( keyState[VK_LSHIFT] || keyState[VK_RSHIFT] )
	if( keyState[VK_SHIFT] )
	{   // Left Lock

		//if( keyLS < 0 )
		//	std::cout << "Shift Key is Down" << std::endl;
		//	//g_bIsShiftPressed = true;
		//else
		//	std::cout << "Shift Key is Up" << std::endl;
		//	//g_bIsShiftPressed = false;
	}

	if( keyState[VK_LCONTROL] )
	{   // Left Control (Activate or Deactivate the hiden Mode)
		g_bIsHideModeOn = !g_bIsHideModeOn;
		if( g_bIsHideModeOn )
			std::cout << "Hidden Mode is ON" << std::endl;
		else
			std::cout << "Hidden Mode is OFF" << std::endl;
	}

	if( keyState[VK_RETURN] )
	{   // Enter Key
		//system( "cls" );
		std::string theLine;

		for( int pos = 0; pos != theCurrentLine.size(); pos++ )
		{
			theLine += theCurrentLine[pos];
		}		

		thePreviousLines.push_back( theLine );
		theCurrentLine.clear();

		//thePreviousLines.push_back( " " );

		system( "cls" );
		for( int line = 0; line != thePreviousLines.size(); line++ )
		{
			std::cout << thePreviousLines[line] << std::endl;
		}

	}

	if( keyState[VK_SPACE] )
	{   // Space Key
		theCurrentLine.push_back( ' ' );
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

			// print the a blank line until the last po
			for( int pos = 0; pos != howManyChars; pos++ )
			{
				std::cout << " ";
			}
		}
	}

	for( int key = 0; key != 257; key++ )
	{
		if( keyState[key] )
		{
			// Check for number input
			if( key >= 48 && key <= 57 )
			{
				theCurrentLine.push_back( numberMap[key - 48] );
			}

			// Check for letter input
			else if( key >= 65 && key <= 90 )
			{
				char toBePrinted;

				toBePrinted = keyMap[key - 65];
				if( g_bIsCapsPressed || g_bIsShiftPressed )
					toBePrinted = toupper( toBePrinted );

				theCurrentLine.push_back( toBePrinted );
			}
		}
	}

}

void printIntro()
{
	//std::cout << "Starting" << std::endl;
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

int main()
{
	printIntro();

	while( !g_bStartProgram )
	{
		g_bStartProgram = checkAnyKeyWasPressed( theKeyState );
	}

	//system( "cls" );	
	thePreviousLines.push_back( "Welcome to Magician!" );
	thePreviousLines.push_back( "I can answer all your questions by using only my great magical skills..." );
	thePreviousLines.push_back( " " );
	thePreviousLines.push_back( "What is that you wish to know?" );

	system( "cls" );
	for( int line = 0; line != thePreviousLines.size(); line++ )
	{
		std::cout << thePreviousLines[line] << std::endl;
	}

	while( !g_exit_game )
	{
		if( checkAnyKeyWasPressed( theKeyState ) )
		{
			//std::cout << "Some key was pressed!" << std::endl;
			checkInput( theKeyState );

			// return the cursor to the beggining of the line
			std::cout << "\r";

			// print the current line over again
			for( int pos = 0; pos != theCurrentLine.size(); pos++ )
			{
				std::cout << theCurrentLine[pos];
			}

			//system( "cls" );
			//for( int line = 0; line != thePreviousLines.size(); line++ )
			//{
			//	std::cout << thePreviousLines[line] << std::endl;
			//}

		}
		//Sleep( 100 );	

		//theCurrentLine

		//if( g_bNewInput )
		//{
		//	if( g_bIsHideModeOn )
		//		std::cout << "Hide Mode On" << std::endl;
		//	else
		//		std::cout << "Hide Mode Off!" << std::endl;
		//}
		//g_bNewInput = false;
	}

	//std::string input;

	//std::cin >> input;

	return 0;
}