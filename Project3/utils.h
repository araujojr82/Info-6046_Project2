#ifndef _utils_HG_
#define _utils_HG_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>	// "String stream"
#include <iostream>

#include "sound.h"

void loadFileToVector( std::string fileName, std::vector<std::string> &fileIntoLines );
void recordInstructions();
void playIntro();

#endif