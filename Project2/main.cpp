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

std::vector<char> theAnswer;

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

			// print the a blank line until the last po
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

		// print the current line over again
		for( int pos = 0; pos != theAnswer.size(); pos++ )
		{
			//theLine += theAnswer[pos];
			std::cout << theAnswer[pos];
		}
		//thePreviousLines.push_back( theLine );
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
		if( g_bIsHideModeOn )
			std::cout << "Hidden Mode is ON" << std::endl;
		else
			std::cout << "Hidden Mode is OFF" << std::endl;
		return;
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
				theCurrentLine.push_back( ' ' );
			}

			else if( key == VK_CAPITAL )
			{   // Caps Lock
				g_bIsCapsPressed = !g_bIsCapsPressed;
			}

			// Check for number input
			else if( key >= 48 && key <= 57 )
			{
				theTypedChar = numberMap[key - 48];
				//theCurrentLine.push_back( numberMap[key - 48] );
			}

			// Check for letter input
			else if( key >= 65 && key <= 90 )
			{

				theTypedChar = keyMap[key - 65];
				if( g_bIsCapsPressed || g_bIsShiftPressed )
					theTypedChar = toupper( theTypedChar );

				//theCurrentLine.push_back( theTypedChar );
			}

			// Check for all others 
			else
			{		
				theTypedChar = MapVirtualKey( key, 2 );
				if( theTypedChar != 0 )
				{
					if( g_bIsShiftPressed ){
						if( theTypedChar == '/' ) theTypedChar = '?';
					}
					//theCurrentLine.push_back( theTypedChar );
				}
			} // else
		}
	} // for( int key = 0; key != 257; key++ )

	if( theTypedChar != NULL )
	{
		if( g_bIsHideModeOn )
		{
			theCurrentLine.push_back( standardLine[theCurrentLine.size()] );
			theAnswer.push_back( theTypedChar );
		}
		else
		{
			theCurrentLine.push_back( theTypedChar );
		}
	}

} // void checkInput...

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

void checkShiftState()
{
	short sState2 = GetKeyState( VK_SHIFT );

	g_bIsShiftPressed = false;	

	if( sState2 < 0 )
		g_bIsShiftPressed = true;

	return;
}

int main()
{
	printIntro();

	while( !g_bStartProgram )
	{
		g_bStartProgram = checkAnyKeyWasPressed( theKeyState );
	}
	theCurrentLine.clear();
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

			// print the current line over again
			for( int pos = 0; pos != theCurrentLine.size(); pos++ )
			{
				std::cout << theCurrentLine[pos];
			}

		}

	}

	return 0;
}