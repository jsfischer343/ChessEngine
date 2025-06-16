#ifndef __UTILITIES_HH__
#define __UTILITIES_HH__
#include <cctype>
#include <cstdint>

char engineRank_TO_notationRank(int8_t engineRank);
char engineFile_TO_notationFile(int8_t engineFile);
int8_t notationRank_TO_engineRank(char notationRank);
int8_t notationFile_TO_engineFile(char notationFile);
char enginePieceType_TO_notationPieceType(char enginePieceType, char enginePieceColor);
char* deleteCharacter(char* charString, int index, int length);
bool isFileOrRank(char charInQuestion);
bool isRank(char charInQuestion);
bool isFile(char charInQuestion);

#endif
