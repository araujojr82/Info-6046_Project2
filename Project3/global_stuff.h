#pragma once
#ifndef _global_stuff_HG_
#define _global_stuff_HG_

#include <vector>

const std::string MALE_VOICE = "Male";
const std::string FEMALE_VOICE = "Female";

// Global check variables
bool g_bExit_game = false;
bool g_bStartProgram = false;
bool g_bIsHideModeOn = false;
bool g_bIsTimeToAnser = false;
bool g_bIsShiftPressed = false;
bool g_bIsCapsPressed = false;
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


#endif