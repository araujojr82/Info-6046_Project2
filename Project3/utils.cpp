#include "utils.h"

extern const std::string MALE_VOICE = "Male";
extern const std::string FEMALE_VOICE = "Female";

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