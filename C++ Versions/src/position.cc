#include "position.hh"

Position::Position()
{
	//1. create space
	//create space for storing piece information for this position
	whitePieces = new piece[16];
	blackPieces = new piece[16];
	//create space for storing board information for this position
	theBoard = new square*[8];
	for(size_t i=0;i<8;i++)
	{
		theBoard[i] = new square[8];
		for(size_t j=0;j<8;j++)
		{
			theBoard[i][j].rank = i;
			theBoard[i][j].file = j;
		}
	}
	//setup default position
	placePiece(0,0,'r','b');
	placePiece(0,1,'n','b');
	placePiece(0,2,'b','b');
	placePiece(0,3,'q','b');
	placePiece(0,4,'k','b');
	placePiece(0,5,'b','b');
	placePiece(0,6,'n','b');
	placePiece(0,7,'r','b');
	//
	placePiece(1,7,'p','b');
	placePiece(1,0,'p','b');
	placePiece(1,1,'p','b');
	placePiece(1,2,'p','b');
	placePiece(1,3,'p','b');
	placePiece(1,4,'p','b');
	placePiece(1,5,'p','b');
	placePiece(1,6,'p','b');
	placePiece(1,7,'p','b');
	///
	placePiece(7,0,'r','w');
	placePiece(7,1,'n','w');
	placePiece(7,2,'b','w');
	placePiece(7,3,'q','w');
	placePiece(7,4,'k','w');
	placePiece(7,5,'b','w');
	placePiece(7,6,'n','w');
	placePiece(7,7,'r','w');
	//
	placePiece(6,7,'p','w');
	placePiece(6,0,'p','w');
	placePiece(6,1,'p','w');
	placePiece(6,2,'p','w');
	placePiece(6,3,'p','w');
	placePiece(6,4,'p','w');
	placePiece(6,5,'p','w');
	placePiece(6,6,'p','w');
	placePiece(6,7,'p','w');
	colorToMove = 'w';
	for(size_t i=0;i<4;i++)
	{
		castlingFlags[i] = true;
	}
	fiftyMoveRuleCounter = 0;
	moveCounter = 1;
}
Position::Position(const char* FENString)
{
	//1. create space
	//create space for storing piece information for this position
	whitePieces = new piece[16];
	blackPieces = new piece[16];
	//create space for storing board information for this position
	theBoard = new square*[8];
	for(size_t i=0;i<8;i++)
	{
		theBoard[i] = new square[8];
		for(size_t j=0;j<8;j++)
		{
			theBoard[i][j].rank = i;
			theBoard[i][j].file = j;
		}
	}
	//2. convert FENString and store in whitePieces, blackPieces, and theBoard
	//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
	int currentFile;
	char currentChar;
	int j=0;
	for(size_t i=0;i<8;i++)
	{
		currentFile = 0;
		currentChar = FENString[j];
		while(true)
		{
			switch(currentChar)
			{
				case 'k':
					placePiece(i,currentFile,'k','b');
					currentFile++;
					break;
				case 'q':
					placePiece(i,currentFile,'q','b');
					currentFile++;
					break;
				case 'r':
					placePiece(i,currentFile,'r','b');
					currentFile++;
					break;
				case 'b':
					placePiece(i,currentFile,'b','b');
					currentFile++;
					break;
				case 'n':
					placePiece(i,currentFile,'n','b');
					currentFile++;
					break;
				case 'p':
					placePiece(i,currentFile,'p','b');
					currentFile++;
					break;
				case 'K':
					placePiece(i,currentFile,'k','w');
					currentFile++;
					break;
				case 'Q':
					placePiece(i,currentFile,'q','w');
					currentFile++;
					break;
				case 'R':
					placePiece(i,currentFile,'r','w');
					currentFile++;
					break;
				case 'B':
					placePiece(i,currentFile,'b','w');
					currentFile++;
					break;
				case 'N':
					placePiece(i,currentFile,'n','w');
					currentFile++;
					break;
				case 'P':
					placePiece(i,currentFile,'p','w');
					currentFile++;
					break;
				case '1':
					currentFile++;
					break;
				case '2':
					currentFile=currentFile+2;
					break;
				case '3':
					currentFile=currentFile+3;
					break;
				case '4':
					currentFile=currentFile+4;
					break;
				case '5':
					currentFile=currentFile+5;
					break;
				case '6':
					currentFile=currentFile+6;
					break;
				case '7':
					currentFile=currentFile+7;
					break;
				case '8':
					currentFile=currentFile+8;
					break;
				case '/':
					break;
				case ' ':
					break;
				default:
					printf("Error 147");
					return;
			}
			if(currentChar=='/'||currentChar==' ')
			{
				j++;
				break;
			}
			j++;
			currentChar = FENString[j];
		}
	}
	colorToMove = FENString[j];
	j=j+2;
	currentChar = FENString[j];
	if(currentChar=='-')
	{
		j=j+2;
	}
	else
	{
		switch(currentChar)
		{
			case 'K':
				castlingFlags[kingside_white]=true;
				j++;
				currentChar = FENString[j];
			case 'Q':
				castlingFlags[queenside_white]=true;
				j++;
				currentChar = FENString[j];
			case 'k':
				castlingFlags[kingside_black]=true;
				j++;
				currentChar = FENString[j];
			case 'q':
				castlingFlags[queenside_black]=true;
				j++;
				currentChar = FENString[j];
		}
		j++;
		currentChar = FENString[j];
	}
	//TODO: en passant, fiftymove, movecounter
}
Position::Position(const Position& lastPosition)
{
	//1. create space & deepcopy
	lastMove = new move;
	*lastMove = *(lastPosition.lastMove);
	//create space for storing piece information for this position
	whitePieces = new piece[16];
	std::memcpy(whitePieces,lastPosition.whitePieces,16*sizeof(piece));
	blackPieces = new piece[16];
	std::memcpy(blackPieces,lastPosition.blackPieces,16*sizeof(piece));
	//create space for storing board information for this position
	theBoard = new square*[8];
	for(size_t i=0;i<8;i++)
	{
		theBoard[i] = new square[8];
		for(size_t j=0;j<8;j++)
		{
			theBoard[i][j] = lastPosition.theBoard[i][j];
		}
	}
}
void Position::printBoard()
{
	for(size_t i=0;i<8;i++)
	{
		for(size_t j=0;j<8;j++)
		{
			if(theBoard[i][j].piecePtr==NULL)
			{
				printf("\t.");
			}
			else
			{
				if(theBoard[i][j].piecePtr->color=='w')
				{
					printf("\t%c",(char)std::toupper(theBoard[i][j].piecePtr->type));
				}
				else
				{
					printf("\t%c",theBoard[i][j].piecePtr->type);
				}
			}
		}
		printf("\n\n\n");
	}
}
void Position::printStats()
{
	printf("Color To Move: %c\n",colorToMove);
	printf("Castling: [%d,%d,%d,%d]\n",castlingFlags[kingside_white],castlingFlags[queenside_white],castlingFlags[kingside_black],castlingFlags[queenside_black]);
	printf("En Passant: WIP\n");
	printf("Fifty Move Rule Counter: %d\n",fiftyMoveRuleCounter);
	printf("Move: %d\n",moveCounter);
}
void Position::placePiece(std::uint8_t rank, std::uint8_t file, char type, char color)
{
	if(color=='w')
	{
		for(size_t i=0;i<16;i++)
		{
			if(whitePieces[i].type=='\0')
			{
				whitePieces[i].type = type;
				whitePieces[i].color = color;
				whitePieces[i].rank = rank;
				whitePieces[i].file = file;
				whitePieces[i].squarePtr = &theBoard[rank][file];
				theBoard[rank][file].piecePtr = &whitePieces[i];
				return;
			}
		}
	}
	else
	{
		for(size_t i=0;i<16;i++)
		{
			if(blackPieces[i].type=='\0')
			{
				blackPieces[i].type = type;
				blackPieces[i].color = color;
				blackPieces[i].rank = rank;
				blackPieces[i].file = file;
				blackPieces[i].squarePtr = &theBoard[rank][file];
				theBoard[rank][file].piecePtr = &blackPieces[i];
				return;
			}
		}
	}
}
void Position::addTargets(piece* pieceInQuestion)
{
	//Cases: King,Queen,Rook,Bishop,Knight,Pawn
	//Steps:
	//1. Add square pointers for all squares targeted by the piece to "targets"
	//2. Add this piece pointer for each of the (white/black)Targeters arrays contained in the squares the piece targets
	if(pieceInQuestion->color=='w')
	{
		switch(pieceInQuestion->type)
		{
			case 'k':
			case 'q':
			case 'r':
			case 'b':
			case 'n':
			case 'p':
			default:
				return;
		}
	}
	else
	{
		switch(pieceInQuestion->type)
		{
			case 'k':
			case 'q':
			case 'r':
			case 'b':
			case 'n':
			case 'p':
			default:
				return;
		}
	}
}
void Position::removeTargets(piece* pieceInQuestion)
{
}

Position::~Position()
{
	delete lastMove;
	delete[] whitePieces;
	delete[] blackPieces;
	for(size_t i=0;i<8;i++)
	{
		delete[] theBoard[i];
	}
	delete[] theBoard;
}