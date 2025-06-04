#include "utilities.hh"

char engineRank_TO_notationRank(int8_t engineRank)
{
	switch(engineRank)
	{
		case 0:
			return '8';
		case 1:
			return '7';
		case 2:
			return '6';
		case 3:
			return '5';
		case 4:
			return '4';
		case 5:
			return '3';
		case 6:
			return '2';
		case 7:
			return '1';
	}
	return '\0';
}
char engineFile_TO_notationFile(int8_t engineFile)
{
	switch(engineFile)
	{
		case 0:
			return 'a';
		case 1:
			return 'b';
		case 2:
			return 'c';
		case 3:
			return 'd';
		case 4:
			return 'e';
		case 5:
			return 'f';
		case 6:
			return 'g';
		case 7:
			return 'h';
	}
	return '\0';
}
int8_t notationRank_TO_engineRank(char notationRank)
{
	switch(notationRank)
	{
		case '8':
			return 0;
		case '7':
			return 1;
		case '6':
			return 2;
		case '5':
			return 3;
		case '4':
			return 4;
		case '3':
			return 5;
		case '2':
			return 6;
		case '1':
			return 7;
	}
	return -1;
}
int8_t notationFile_TO_engineFile(char notationFile)
{
	switch(notationFile)
	{
		case 'a':
			return 0;
		case 'b':
			return 1;
		case 'c':
			return 2;
		case 'd':
			return 3;
		case 'e':
			return 4;
		case 'f':
			return 5;
		case 'g':
			return 6;
		case 'h':
			return 7;
	}
	return -1;
}
char enginePieceType_TO_notationPieceType(char enginePieceType, char enginePieceColor)
{
	if(enginePieceType=='\0' && enginePieceColor=='\0')
	{
		return '.';
	}
	if(enginePieceColor=='w')
	{
		return toupper(enginePieceType);
	}
	else
	{
		return enginePieceType;
	}
}