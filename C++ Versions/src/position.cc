#include "position.hh"
#include "utilities.hh"

//-Constructors-
Position::Position()
{
	setupMemory();
	initPiece(0,0,'r','b');
	initPiece(0,1,'n','b');
	initPiece(0,2,'b','b');
	initPiece(0,3,'q','b');
	initPiece(0,4,'k','b');
	initPiece(0,5,'b','b');
	initPiece(0,6,'n','b');
	initPiece(0,7,'r','b');
	//
	initPiece(1,0,'p','b');
	initPiece(1,1,'p','b');
	initPiece(1,2,'p','b');
	initPiece(1,3,'p','b');
	initPiece(1,4,'p','b');
	initPiece(1,5,'p','b');
	initPiece(1,6,'p','b');
	initPiece(1,7,'p','b');
	///
	initPiece(7,0,'r','w');
	initPiece(7,1,'n','w');
	initPiece(7,2,'b','w');
	initPiece(7,3,'q','w');
	initPiece(7,4,'k','w');
	initPiece(7,5,'b','w');
	initPiece(7,6,'n','w');
	initPiece(7,7,'r','w');
	//
	initPiece(6,0,'p','w');
	initPiece(6,1,'p','w');
	initPiece(6,2,'p','w');
	initPiece(6,3,'p','w');
	initPiece(6,4,'p','w');
	initPiece(6,5,'p','w');
	initPiece(6,6,'p','w');
	initPiece(6,7,'p','w');
	colorToMove = 'w';
	for(int i=0;i<4;i++)
	{
		castlingFlags[i] = true;
	}
	for(int i=0;i<4;i++)
	{
		canCastle[i] = false;
	}
	fiftyMoveRuleCounter = 0;
	moveCounter = 1;
	resolve();
}
Position::Position(const char* FENString)
{
	//1. allocate memory
	setupMemory();
	//2. convert FENString and store in whitePieces, blackPieces, and theBoard
	//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
	int FEN_currentFile;
	char FEN_currentChar;
	size_t FEN_charIndex=0;
	for(int i=0;i<8;i++)
	{
		FEN_currentFile = 0;
		FEN_currentChar = FENString[FEN_charIndex];
		while(true)
		{
			switch(FEN_currentChar)
			{
				case 'k':
					initPiece(i,FEN_currentFile,'k','b');
					FEN_currentFile++;
					break;
				case 'q':
					initPiece(i,FEN_currentFile,'q','b');
					FEN_currentFile++;
					break;
				case 'r':
					initPiece(i,FEN_currentFile,'r','b');
					FEN_currentFile++;
					break;
				case 'b':
					initPiece(i,FEN_currentFile,'b','b');
					FEN_currentFile++;
					break;
				case 'n':
					initPiece(i,FEN_currentFile,'n','b');
					FEN_currentFile++;
					break;
				case 'p':
					initPiece(i,FEN_currentFile,'p','b');
					FEN_currentFile++;
					break;
				case 'K':
					initPiece(i,FEN_currentFile,'k','w');
					FEN_currentFile++;
					break;
				case 'Q':
					initPiece(i,FEN_currentFile,'q','w');
					FEN_currentFile++;
					break;
				case 'R':
					initPiece(i,FEN_currentFile,'r','w');
					FEN_currentFile++;
					break;
				case 'B':
					initPiece(i,FEN_currentFile,'b','w');
					FEN_currentFile++;
					break;
				case 'N':
					initPiece(i,FEN_currentFile,'n','w');
					FEN_currentFile++;
					break;
				case 'P':
					initPiece(i,FEN_currentFile,'p','w');
					FEN_currentFile++;
					break;
				case '1':
					FEN_currentFile++;
					break;
				case '2':
					FEN_currentFile=FEN_currentFile+2;
					break;
				case '3':
					FEN_currentFile=FEN_currentFile+3;
					break;
				case '4':
					FEN_currentFile=FEN_currentFile+4;
					break;
				case '5':
					FEN_currentFile=FEN_currentFile+5;
					break;
				case '6':
					FEN_currentFile=FEN_currentFile+6;
					break;
				case '7':
					FEN_currentFile=FEN_currentFile+7;
					break;
				case '8':
					FEN_currentFile=FEN_currentFile+8;
					break;
				case '/':
					break;
				case ' ':
					break;
				default:
					printf("Error 147");
					return;
			}
			if(FEN_currentChar=='/'||FEN_currentChar==' ')
			{
				FEN_charIndex++;
				break;
			}
			FEN_charIndex++;
			FEN_currentChar = FENString[FEN_charIndex];
		}
	}
	colorToMove = FENString[FEN_charIndex];
	FEN_charIndex=FEN_charIndex+2;
	FEN_currentChar = FENString[FEN_charIndex];
	//castling
	if(FEN_currentChar=='-')
	{
		FEN_charIndex=FEN_charIndex+2;
		FEN_currentChar = FENString[FEN_charIndex];
	}
	else
	{
		switch(FEN_currentChar)
		{
			case 'K':
				castlingFlags[kingside_white]=true;
				FEN_charIndex++;
				FEN_currentChar = FENString[FEN_charIndex];
			case 'Q':
				castlingFlags[queenside_white]=true;
				FEN_charIndex++;
				FEN_currentChar = FENString[FEN_charIndex];
			case 'k':
				castlingFlags[kingside_black]=true;
				FEN_charIndex++;
				FEN_currentChar = FENString[FEN_charIndex];
			case 'q':
				castlingFlags[queenside_black]=true;
				FEN_charIndex++;
		}
		FEN_charIndex++;
		FEN_currentChar = FENString[FEN_charIndex];
	}
	//en passant
	if(FEN_currentChar=='-')
	{
		FEN_charIndex=FEN_charIndex+2;
		FEN_currentChar = FENString[FEN_charIndex];
	}
	else
	{
		if(FEN_currentChar=='a') enPassantFile=0;
		if(FEN_currentChar=='b') enPassantFile=1;
		if(FEN_currentChar=='c') enPassantFile=2;
		if(FEN_currentChar=='d') enPassantFile=3;
		if(FEN_currentChar=='e') enPassantFile=4;
		if(FEN_currentChar=='f') enPassantFile=5;
		if(FEN_currentChar=='g') enPassantFile=6;
		if(FEN_currentChar=='h') enPassantFile=7;
		FEN_charIndex++;
		FEN_currentChar = FENString[FEN_charIndex];
		enPassantRank = atoi(&FEN_currentChar);
		FEN_charIndex=FEN_charIndex+2;
		FEN_currentChar = FENString[FEN_charIndex];
	}
	//fiftymove
	char* tempBuffer = new char[4];
	int j=0;
	do
	{
		tempBuffer[j] = FEN_currentChar;
		FEN_charIndex++;
		j++;
		FEN_currentChar = FENString[FEN_charIndex];
	} while(FEN_currentChar!=' ' && j<4);
	fiftyMoveRuleCounter = atoi(tempBuffer);
	delete[] tempBuffer;
	FEN_charIndex=FEN_charIndex+1;
	FEN_currentChar = FENString[FEN_charIndex];

	//movecounter
	tempBuffer = new char[4];
	j=0;
	do
	{
		tempBuffer[j] = FEN_currentChar;
		FEN_charIndex++;
		j++;
		FEN_currentChar = FENString[FEN_charIndex];
	} while(FEN_currentChar!=' ' && j<4);
	moveCounter = atoi(tempBuffer);
	delete[] tempBuffer;
	sanityCheck();
	resolve();
	//determine ability to castle
	canCastle[kingside_white] = checkCanCastle(kingside_white);
	canCastle[queenside_white] = checkCanCastle(queenside_white);
	canCastle[kingside_black] = checkCanCastle(kingside_black);
	canCastle[queenside_black] = checkCanCastle(queenside_black);
}
Position::Position(const Position& lastPosition, const move moveMade)
{
	setupMemory();
	//DEEPCOPY
	//Pieces
	whitePieces_L = lastPosition.whitePieces_L;
	blackPieces_L = lastPosition.blackPieces_L;
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(lastPosition.whitePieces[i].type=='\0')
			continue;
		//White
		whitePieces[i].type = lastPosition.whitePieces[i].type;
		whitePieces[i].color = lastPosition.whitePieces[i].color;
		whitePieces[i].rank = lastPosition.whitePieces[i].rank;
		whitePieces[i].file = lastPosition.whitePieces[i].file;
		whitePieces[i].squarePtr = &theBoard[whitePieces[i].rank][whitePieces[i].file];
		theBoard[whitePieces[i].rank][whitePieces[i].file].piecePtr = &whitePieces[i];
		//Note: targeters and moves not copied
	}
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(lastPosition.blackPieces[i].type=='\0')
			continue;
		//Black
		blackPieces[i].type = lastPosition.blackPieces[i].type;
		blackPieces[i].color = lastPosition.blackPieces[i].color;
		blackPieces[i].rank = lastPosition.blackPieces[i].rank;
		blackPieces[i].file = lastPosition.blackPieces[i].file;
		blackPieces[i].squarePtr = &theBoard[blackPieces[i].rank][blackPieces[i].file];
		theBoard[blackPieces[i].rank][blackPieces[i].file].piecePtr = &blackPieces[i];
		//Note: targeters and moves not copied
	}

	//Catch Castling Moves
	if(moveMade.startRank==7 && moveMade.startFile==4 &&
	moveMade.endRank==7 && moveMade.endFile==6 &&
	moveMade.endPieceType=='k')
	{
		castlingConstructor(lastPosition, kingside_white);
	}
	else if(moveMade.startRank==7 && moveMade.startFile==4 &&
	moveMade.endRank==7 && moveMade.endFile==2 &&
	moveMade.endPieceType=='k')
	{
		castlingConstructor(lastPosition, queenside_white);
	}
	else if(moveMade.startRank==0 && moveMade.startFile==4 &&
	moveMade.endRank==0 && moveMade.endFile==6 &&
	moveMade.endPieceType=='k')
	{
		castlingConstructor(lastPosition, kingside_black);
	}
	else if(moveMade.startRank==0 && moveMade.startFile==4 &&
	moveMade.endRank==0 && moveMade.endFile==2 &&
	moveMade.endPieceType=='k')
	{
		castlingConstructor(lastPosition, queenside_black);
	}

	//Otherwise Normal Moves Steps
	else
	{
		//Castling Flags
		for(int i=0;i<4;i++)
		{
			castlingFlags[i] = lastPosition.castlingFlags[i];
		}
		//fiftyMoveRuleCounter
		if(lastPosition.theBoard[moveMade.startRank][moveMade.startFile].piecePtr->type == 'p')
		{
			fiftyMoveRuleCounter = 0;
		}
		else if(lastPosition.theBoard[moveMade.endRank][moveMade.endFile].piecePtr!=NULL)
		{
			fiftyMoveRuleCounter = 0;
		}
		else
		{
			fiftyMoveRuleCounter = lastPosition.fiftyMoveRuleCounter+1;
		}


		//Updates based on turn
		if(lastPosition.colorToMove=='w')
		{
			moveCounter = lastPosition.moveCounter+0;
			colorToMove = 'b';
			//check castling flags based on last move to see if flags should be updated
			if(castlingFlags[kingside_white] || castlingFlags[queenside_white])
			{
				if(moveMade.startRank==7 && moveMade.startFile==4)
				{
					castlingFlags[kingside_white] = false;
					castlingFlags[queenside_white] = false;
				}
				if(moveMade.startRank==7 && moveMade.startFile==0)
				{
					castlingFlags[queenside_white] = false;
				}
				if(moveMade.startRank==7 && moveMade.startFile==7)
				{
					castlingFlags[kingside_white] = false;
				}
			}
		}
		else
		{
			moveCounter = lastPosition.moveCounter+1;
			colorToMove = 'w';
			//check castling flags based on last move to see if flags should be updated
			if(castlingFlags[kingside_black] || castlingFlags[queenside_black])
			{
				if(moveMade.startRank==0 && moveMade.startFile==4)
				{
					castlingFlags[kingside_black] = false;
					castlingFlags[queenside_black] = false;
				}
				if(moveMade.startRank==0 && moveMade.startFile==0)
				{
					castlingFlags[queenside_black] = false;
				}
				if(moveMade.startRank==0 && moveMade.startFile==7)
				{
					castlingFlags[kingside_black] = false;
				}
			}
		}

		//Make Move: update the board based on the 'moveMade'
		piece* pieceBeingMoved = theBoard[moveMade.startRank][moveMade.startFile].piecePtr;
		char type = moveMade.endPieceType;
		char color = pieceBeingMoved->color;
		removePiece(pieceBeingMoved);
		initPiece(moveMade.endRank,moveMade.endFile,type,color);

		resolve();
		//determine ability to castle
		canCastle[kingside_white] = checkCanCastle(kingside_white);
		canCastle[queenside_white] = checkCanCastle(queenside_white);
		canCastle[kingside_black] = checkCanCastle(kingside_black);
		canCastle[queenside_black] = checkCanCastle(queenside_black);
	}
}
void Position::castlingConstructor(const Position& lastPosition, const int8_t castlingCode)
{
	//Castling Flags
	for(int i=0;i<4;i++)
	{
		castlingFlags[i] = lastPosition.castlingFlags[i];
	}
	if(castlingCode == kingside_white || castlingCode == queenside_white)
	{
		castlingFlags[kingside_white] = false;
		castlingFlags[queenside_white] = false;
	}
	else if(castlingCode == kingside_black || castlingCode == queenside_black)
	{
		castlingFlags[kingside_black] = false;
		castlingFlags[queenside_black] = false;
	}
	//fiftyMoveRuleCounter
	fiftyMoveRuleCounter = lastPosition.fiftyMoveRuleCounter+1;


	//Updates based on turn
	if(lastPosition.colorToMove=='w')
	{
		moveCounter = lastPosition.moveCounter+0;
		colorToMove = 'b';
	}
	else
	{
		moveCounter = lastPosition.moveCounter+1;
		colorToMove = 'w';
	}

	//Make Move: Update the board based on the 'castlingCode'
	if(castlingCode == kingside_white)
	{
		removePiece(theBoard[7][4].piecePtr);
		initPiece(7,6,'k','w');
		removePiece(theBoard[7][7].piecePtr);
		initPiece(7,5,'r','w');
	}
	if(castlingCode == queenside_white)
	{
		removePiece(theBoard[7][4].piecePtr);
		initPiece(7,2,'k','w');
		removePiece(theBoard[7][0].piecePtr);
		initPiece(7,3,'r','w');
	}
	if(castlingCode == kingside_black)
	{
		removePiece(theBoard[0][4].piecePtr);
		initPiece(0,6,'k','b');
		removePiece(theBoard[0][7].piecePtr);
		initPiece(0,5,'r','b');
	}
	if(castlingCode == queenside_black)
	{
		removePiece(theBoard[0][4].piecePtr);
		initPiece(0,2,'k','b');
		removePiece(theBoard[0][0].piecePtr);
		initPiece(0,3,'r','b');
	}
	resolve();
	//determine ability to castle
	canCastle[kingside_white] = checkCanCastle(kingside_white);
	canCastle[queenside_white] = checkCanCastle(queenside_white);
	canCastle[kingside_black] = checkCanCastle(kingside_black);
	canCastle[queenside_black] = checkCanCastle(queenside_black);
}
Position::Position(const Position* lastPosition, const move moveMade)
{
//Pointer version of above constructor
	setupMemory();
	//--DEEPCOPY--
	//Pieces
	whitePieces_L = lastPosition->whitePieces_L;
	blackPieces_L = lastPosition->blackPieces_L;
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(lastPosition->whitePieces[i].type=='\0')
			continue;
		//White
		whitePieces[i].type = lastPosition->whitePieces[i].type;
		whitePieces[i].color = lastPosition->whitePieces[i].color;
		whitePieces[i].rank = lastPosition->whitePieces[i].rank;
		whitePieces[i].file = lastPosition->whitePieces[i].file;
		whitePieces[i].squarePtr = &theBoard[whitePieces[i].rank][whitePieces[i].file];
		theBoard[whitePieces[i].rank][whitePieces[i].file].piecePtr = &whitePieces[i];
		//Note: targeters and moves not copied
	}
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(lastPosition->blackPieces[i].type=='\0')
			continue;
		//Black
		blackPieces[i].type = lastPosition->blackPieces[i].type;
		blackPieces[i].color = lastPosition->blackPieces[i].color;
		blackPieces[i].rank = lastPosition->blackPieces[i].rank;
		blackPieces[i].file = lastPosition->blackPieces[i].file;
		blackPieces[i].squarePtr = &theBoard[blackPieces[i].rank][blackPieces[i].file];
		theBoard[blackPieces[i].rank][blackPieces[i].file].piecePtr = &blackPieces[i];
		//Note: targeters and moves not copied
	}

	//Catch Castling Moves
	if(moveMade.startRank==7 && moveMade.startFile==4 &&
	moveMade.endRank==7 && moveMade.endFile==6 &&
	moveMade.endPieceType=='k')
	{
		castlingConstructor(lastPosition, kingside_white);
	}
	else if(moveMade.startRank==7 && moveMade.startFile==4 &&
	moveMade.endRank==7 && moveMade.endFile==2 &&
	moveMade.endPieceType=='k')
	{
		castlingConstructor(lastPosition, queenside_white);
	}
	else if(moveMade.startRank==0 && moveMade.startFile==4 &&
	moveMade.endRank==0 && moveMade.endFile==6 &&
	moveMade.endPieceType=='k')
	{
		castlingConstructor(lastPosition, kingside_black);
	}
	else if(moveMade.startRank==0 && moveMade.startFile==4 &&
	moveMade.endRank==0 && moveMade.endFile==2 &&
	moveMade.endPieceType=='k')
	{
		castlingConstructor(lastPosition, queenside_black);
	}
	else
	{
		//Castling Flags
		for(int i=0;i<4;i++)
		{
			castlingFlags[i] = lastPosition->castlingFlags[i];
		}
		//fiftyMoveRuleCounter
		if(lastPosition->theBoard[moveMade.startRank][moveMade.startFile].piecePtr->type == 'p')
		{
			fiftyMoveRuleCounter = 0;
		}
		else if(lastPosition->theBoard[moveMade.endRank][moveMade.endFile].piecePtr!=NULL)
		{
			fiftyMoveRuleCounter = 0;
		}
		else
		{
			fiftyMoveRuleCounter = lastPosition->fiftyMoveRuleCounter+1;
		}

		//Updates based on turn
		if(lastPosition->colorToMove=='w')
		{
			moveCounter = lastPosition->moveCounter+0;
			colorToMove = 'b';
			//check castling flags based on last move to see if flags should be updated
			if(castlingFlags[kingside_white] || castlingFlags[queenside_white])
			{
				if(moveMade.startRank==7 && moveMade.startFile==4)
				{
					castlingFlags[kingside_white] = false;
					castlingFlags[queenside_white] = false;
				}
				if(moveMade.startRank==7 && moveMade.startFile==0)
				{
					castlingFlags[queenside_white] = false;
				}
				if(moveMade.startRank==7 && moveMade.startFile==7)
				{
					castlingFlags[kingside_white] = false;
				}
			}
		}
		else
		{
			moveCounter = lastPosition->moveCounter+1;
			colorToMove = 'w';
			//check castling flags based on last move to see if flags should be updated
			if(castlingFlags[kingside_black] || castlingFlags[queenside_black])
			{
				if(moveMade.startRank==0 && moveMade.startFile==4)
				{
					castlingFlags[kingside_black] = false;
					castlingFlags[queenside_black] = false;
				}
				if(moveMade.startRank==0 && moveMade.startFile==0)
				{
					castlingFlags[queenside_black] = false;
				}
				if(moveMade.startRank==0 && moveMade.startFile==7)
				{
					castlingFlags[kingside_black] = false;
				}
			}
		}

		//Make Move: Update the board based on the 'moveMade'
		piece* pieceBeingMoved = theBoard[moveMade.startRank][moveMade.startFile].piecePtr;
		char type = moveMade.endPieceType;
		char color = pieceBeingMoved->color;
		removePiece(pieceBeingMoved);
		initPiece(moveMade.endRank,moveMade.endFile,type,color);

		resolve();
		//determine ability to castle
		canCastle[kingside_white] = checkCanCastle(kingside_white);
		canCastle[queenside_white] = checkCanCastle(queenside_white);
		canCastle[kingside_black] = checkCanCastle(kingside_black);
		canCastle[queenside_black] = checkCanCastle(queenside_black);
	}
}
void Position::castlingConstructor(const Position* lastPosition, const int8_t castlingCode)
{
	//Castling Flags
	for(int i=0;i<4;i++)
	{
		castlingFlags[i] = lastPosition->castlingFlags[i];
	}
	if(castlingCode == kingside_white || castlingCode == queenside_white)
	{
		castlingFlags[kingside_white] = false;
		castlingFlags[queenside_white] = false;
	}
	else if(castlingCode == kingside_black || castlingCode == queenside_black)
	{
		castlingFlags[kingside_black] = false;
		castlingFlags[queenside_black] = false;
	}
	//fiftyMoveRuleCounter
	fiftyMoveRuleCounter = lastPosition->fiftyMoveRuleCounter+1;

	//Updates based on turn
	if(lastPosition->colorToMove=='w')
	{
		moveCounter = lastPosition->moveCounter+0;
		colorToMove = 'b';
	}
	else
	{
		moveCounter = lastPosition->moveCounter+1;
		colorToMove = 'w';
	}

	//Make Move: update the board based on the 'castlingCode'
	if(castlingCode == kingside_white)
	{
		removePiece(theBoard[7][4].piecePtr);
		initPiece(7,6,'k','w');
		removePiece(theBoard[7][7].piecePtr);
		initPiece(7,5,'r','w');
	}
	if(castlingCode == queenside_white)
	{
		removePiece(theBoard[7][4].piecePtr);
		initPiece(7,2,'k','w');
		removePiece(theBoard[7][0].piecePtr);
		initPiece(7,3,'r','w');
	}
	if(castlingCode == kingside_black)
	{
		removePiece(theBoard[0][4].piecePtr);
		initPiece(0,6,'k','b');
		removePiece(theBoard[0][7].piecePtr);
		initPiece(0,5,'r','b');
	}
	if(castlingCode == queenside_black)
	{
		removePiece(theBoard[0][4].piecePtr);
		initPiece(0,2,'k','b');
		removePiece(theBoard[0][0].piecePtr);
		initPiece(0,3,'r','b');
	}
	resolve();
	//determine ability to castle
	canCastle[kingside_white] = checkCanCastle(kingside_white);
	canCastle[queenside_white] = checkCanCastle(queenside_white);
	canCastle[kingside_black] = checkCanCastle(kingside_black);
	canCastle[queenside_black] = checkCanCastle(queenside_black);
}
Position::~Position()
{
	cleanupMemory();
}

//-Get-
int Position::getTotalMoves()
{
	int totalMoves = 0;
	if(colorToMove=='w')
	{
		for(int i=0;i<MAX_PIECES;i++)
		{
			if(whitePieces[i].type=='\0')
				continue;
			totalMoves = totalMoves+whitePieces[i].moves_L;
			for(int j=0;j<whitePieces[i].moves_L;j++)
			{
				if(whitePieces[i].type=='p' && whitePieces[i].moves[j]->rank == 0)
					totalMoves = totalMoves+3;
			}
		}
		if(canCastle[kingside_white])
			totalMoves++;
		if(canCastle[queenside_white])
			totalMoves++;
	}
	else
	{
		for(int i=0;i<MAX_PIECES;i++)
		{
			if(blackPieces[i].type=='\0')
				continue;
			totalMoves = totalMoves+blackPieces[i].moves_L;
			for(int j=0;j<blackPieces[i].moves_L;j++)
			{
				if(blackPieces[i].type=='p' && blackPieces[i].moves[j]->rank == 7)
					totalMoves = totalMoves+3;
			}
		}
		if(canCastle[kingside_black])
			totalMoves++;
		if(canCastle[queenside_black])
			totalMoves++;
	}
	return totalMoves;
}
move* Position::getAllMoves()
{
	int totalMoves = getTotalMoves();
	int remainingMoves = totalMoves;
	move* allMovesArr = new move[totalMoves];
	if(colorToMove=='w')
	{
		for(int i=0;i<MAX_PIECES;i++)
		{
			if(whitePieces[i].type=='\0')
				continue;
			for(int j=0;j<whitePieces[i].moves_L;j++)
			{
				if(whitePieces[i].type=='p' && whitePieces[i].moves[j]->rank == 0)
				{
					allMovesArr[totalMoves-remainingMoves].startRank = whitePieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = whitePieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = whitePieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = whitePieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = 'n';
					remainingMoves--;
					allMovesArr[totalMoves-remainingMoves].startRank = whitePieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = whitePieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = whitePieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = whitePieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = 'b';
					remainingMoves--;
					allMovesArr[totalMoves-remainingMoves].startRank = whitePieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = whitePieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = whitePieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = whitePieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = 'r';
					remainingMoves--;
					allMovesArr[totalMoves-remainingMoves].startRank = whitePieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = whitePieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = whitePieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = whitePieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = 'q';
					remainingMoves--;
				}
				else
				{
					allMovesArr[totalMoves-remainingMoves].startRank = whitePieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = whitePieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = whitePieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = whitePieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = whitePieces[i].type;
					remainingMoves--;
				}
			}
		}
		if(canCastle[kingside_white])
		{
			allMovesArr[totalMoves-remainingMoves].startRank = 7;
			allMovesArr[totalMoves-remainingMoves].startFile = 4;
			allMovesArr[totalMoves-remainingMoves].endRank = 7;
			allMovesArr[totalMoves-remainingMoves].endFile = 6;
			allMovesArr[totalMoves-remainingMoves].endPieceType = 'k';
			remainingMoves--;
		}
		if(canCastle[queenside_white])
		{
			allMovesArr[totalMoves-remainingMoves].startRank = 7;
			allMovesArr[totalMoves-remainingMoves].startFile = 4;
			allMovesArr[totalMoves-remainingMoves].endRank = 7;
			allMovesArr[totalMoves-remainingMoves].endFile = 2;
			allMovesArr[totalMoves-remainingMoves].endPieceType = 'k';
			remainingMoves--;
		}
	}
	else
	{
		for(int i=0;i<MAX_PIECES;i++)
		{
			if(blackPieces[i].type=='\0')
				continue;
			for(int j=0;j<blackPieces[i].moves_L;j++)
			{
				if(blackPieces[i].type=='p' && blackPieces[i].moves[j]->rank == 7)
				{
					allMovesArr[totalMoves-remainingMoves].startRank = blackPieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = blackPieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = blackPieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = blackPieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = 'n';
					remainingMoves--;
					allMovesArr[totalMoves-remainingMoves].startRank = blackPieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = blackPieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = blackPieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = blackPieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = 'b';
					remainingMoves--;
					allMovesArr[totalMoves-remainingMoves].startRank = blackPieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = blackPieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = blackPieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = blackPieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = 'r';
					remainingMoves--;
					allMovesArr[totalMoves-remainingMoves].startRank = blackPieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = blackPieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = blackPieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = blackPieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = 'q';
					remainingMoves--;
				}
				else
				{
					allMovesArr[totalMoves-remainingMoves].startRank = blackPieces[i].rank;
					allMovesArr[totalMoves-remainingMoves].startFile = blackPieces[i].file;
					allMovesArr[totalMoves-remainingMoves].endRank = blackPieces[i].moves[j]->rank;
					allMovesArr[totalMoves-remainingMoves].endFile = blackPieces[i].moves[j]->file;
					allMovesArr[totalMoves-remainingMoves].endPieceType = blackPieces[i].type;
					remainingMoves--;
				}
			}
		}
		if(canCastle[kingside_black])
		{
			allMovesArr[totalMoves-remainingMoves].startRank = 0;
			allMovesArr[totalMoves-remainingMoves].startFile = 4;
			allMovesArr[totalMoves-remainingMoves].endRank = 0;
			allMovesArr[totalMoves-remainingMoves].endFile = 6;
			allMovesArr[totalMoves-remainingMoves].endPieceType = 'k';
			remainingMoves--;
		}
		if(canCastle[queenside_black])
		{
			allMovesArr[totalMoves-remainingMoves].startRank = 0;
			allMovesArr[totalMoves-remainingMoves].startFile = 4;
			allMovesArr[totalMoves-remainingMoves].endRank = 0;
			allMovesArr[totalMoves-remainingMoves].endFile = 2;
			allMovesArr[totalMoves-remainingMoves].endPieceType = 'k';
			remainingMoves--;
		}
	}
	return allMovesArr;
}
char* Position::getNotation(const move& moveInQuestion)
{
	size_t buffer_L = 10;
	char* buffer = new char[buffer_L];
	buffer[0] = 'N';
	buffer[1] = 'U';
	buffer[2] = 'L';
	buffer[3] = 'L';
	for(int i=4;i<buffer_L;i++)
	{
		buffer[i] = '\0';
	}

	if(moveInQuestion.startRank==-1)
		return buffer;
	//1. Piece Type
	char pieceType;
	//2. Disambiguation
	char disambig[2];
	bool rankDisambig = false;
	bool fileDisambig = false;
	//3. Capture
	char capture;
	//4. Destination
	char destination[2];
	//5. Promotion
	char promotion[2];
	//6. Check or Checkmate
	char checkOrCheckmate;

	//Castling
	if(moveInQuestion.startRank==7 && moveInQuestion.startFile==4 && moveInQuestion.endRank==7 && moveInQuestion.endFile==6 && moveInQuestion.endPieceType=='k')
	{
		//kingside_white
		buffer[0] = 'O';
		buffer[1] = '-';
		buffer[2] = 'O';
		buffer[3] = ' ';
		return buffer;
	}
	else if(moveInQuestion.startRank==7 && moveInQuestion.startFile==4 && moveInQuestion.endRank==7 && moveInQuestion.endFile==2 && moveInQuestion.endPieceType=='k')
	{
		//queenside_white
		buffer[0] = 'O';
		buffer[1] = '-';
		buffer[2] = 'O';
		buffer[3] = '-';
		buffer[4] = 'O';
		return buffer;
	}
	else if(moveInQuestion.startRank==0 && moveInQuestion.startFile==4 && moveInQuestion.endRank==0 && moveInQuestion.endFile==6 && moveInQuestion.endPieceType=='k')
	{
		//kingside_black
		buffer[0] = 'O';
		buffer[1] = '-';
		buffer[2] = 'O';
		buffer[3] = ' ';
		return buffer;
	}
	else if(moveInQuestion.startRank==0 && moveInQuestion.startFile==4 && moveInQuestion.endRank==0 && moveInQuestion.endFile==2 && moveInQuestion.endPieceType=='k')
	{
		//queenside_black
		buffer[0] = 'O';
		buffer[1] = '-';
		buffer[2] = 'O';
		buffer[3] = '-';
		buffer[4] = 'O';
		return buffer;
	}
	//
	else
	{
		if(theBoard[moveInQuestion.startRank][moveInQuestion.startFile].piecePtr->color=='w')
		{
			pieceType = theBoard[moveInQuestion.startRank][moveInQuestion.startFile].piecePtr->type;
			//
			for(int i=0;i<MAX_PIECES;i++)
			{
				if(whitePieces[i].type=='\0')
					continue;
				if(whitePieces[i].type == pieceType)
				{
					if(whitePieces[i].rank == moveInQuestion.startRank && whitePieces[i].file == moveInQuestion.startFile)
						continue;
					for(int j=0;j<whitePieces[i].moves_L;j++)
					{
						if(whitePieces[i].moves[j]->rank == moveInQuestion.endRank && whitePieces[i].moves[j]->file == moveInQuestion.endFile)
						{
							if(whitePieces[i].rank == moveInQuestion.startRank)
								rankDisambig = true;
							else
								fileDisambig = true;
						}
					}
				}
			}
			if(rankDisambig && fileDisambig)
			{
				disambig[0] = engineFile_TO_notationFile(moveInQuestion.startFile);
				disambig[1] = engineRank_TO_notationRank(moveInQuestion.startRank);
			}
			else if(rankDisambig)
			{
				disambig[0] = ' ';
				disambig[1] = engineRank_TO_notationRank(moveInQuestion.startRank);
			}
			else if(fileDisambig)
			{
				disambig[0] = engineFile_TO_notationFile(moveInQuestion.startFile);
				disambig[1] = ' ';
			}
			else
			{
				disambig[0] = ' ';
				disambig[1] = ' ';
			}
			//
			if(getPieceColor(&theBoard[moveInQuestion.endRank][moveInQuestion.endFile])=='b')
			{
				capture = 'x';
			}
			else
			{
				capture = ' ';
			}
			//
			destination[0] = engineFile_TO_notationFile(moveInQuestion.endFile);
			destination[1] = engineRank_TO_notationRank(moveInQuestion.endRank);
			//
			if(pieceType=='p' && moveInQuestion.endRank==0)
			{
				promotion[0] = '=';
				promotion[1] = moveInQuestion.endPieceType;
			}
			else
			{
				promotion[0] = ' ';
				promotion[1] = ' ';
			}
			//

			//fill buffer
			int tempIndex = 0;
			buffer[tempIndex] = (char)std::toupper(pieceType);
			tempIndex++;
			if(disambig[0]!=' ')
			{
				buffer[tempIndex] = disambig[0];
				tempIndex++;
			}
			if(disambig[1]!=' ')
			{
				buffer[tempIndex] = disambig[1];
				tempIndex++;
			}
			if(capture!=' ')
			{
				buffer[tempIndex] = capture;
				tempIndex++;
			}
			buffer[tempIndex] = destination[0];
			tempIndex++;
			buffer[tempIndex] = destination[1];
			tempIndex++;
			if(promotion[0]!=' ')
			{
				buffer[tempIndex] = promotion[0];
				tempIndex++;
				buffer[tempIndex] = (char)std::toupper(promotion[1]);
				tempIndex++;
			}
			//TODO: checks and checkmates
			while(tempIndex<buffer_L)
			{
				buffer[tempIndex] = '\0';
				tempIndex++;
			}
		}
		else
		{
			pieceType = theBoard[moveInQuestion.startRank][moveInQuestion.startFile].piecePtr->type;
			//
			for(int i=0;i<MAX_PIECES;i++)
			{
				if(blackPieces[i].type=='\0')
					continue;
				if(blackPieces[i].type == pieceType)
				{
					if(blackPieces[i].rank == moveInQuestion.startRank && blackPieces[i].file == moveInQuestion.startFile)
						continue;
					for(int j=0;j<blackPieces[i].moves_L;j++)
					{
						if(blackPieces[i].moves[j]->rank == moveInQuestion.endRank && blackPieces[i].moves[j]->file == moveInQuestion.endFile)
						{
							if(blackPieces[i].rank == moveInQuestion.startRank)
								rankDisambig = true;
							else
								fileDisambig = true;
						}
					}
				}
			}
			if(rankDisambig && fileDisambig)
			{
				disambig[0] = engineFile_TO_notationFile(moveInQuestion.startFile);
				disambig[1] = engineRank_TO_notationRank(moveInQuestion.startRank);
			}
			else if(rankDisambig)
			{
				disambig[0] = ' ';
				disambig[1] = engineRank_TO_notationRank(moveInQuestion.startRank);
			}
			else if(fileDisambig)
			{
				disambig[0] = engineFile_TO_notationFile(moveInQuestion.startFile);
				disambig[1] = ' ';
			}
			else
			{
				disambig[0] = ' ';
				disambig[1] = ' ';
			}
			//
			if(getPieceColor(&theBoard[moveInQuestion.endRank][moveInQuestion.endFile])=='w')
			{
				capture = 'x';
			}
			else
			{
				capture = ' ';
			}
			//
			destination[0] = engineFile_TO_notationFile(moveInQuestion.endFile);
			destination[1] = engineRank_TO_notationRank(moveInQuestion.endRank);
			//
			if(pieceType=='p' && moveInQuestion.endRank==7)
			{
				promotion[0] = '=';
				promotion[1] = moveInQuestion.endPieceType;
			}
			else
			{
				promotion[0] = ' ';
				promotion[1] = ' ';
			}
			//

			//fill buffer
			int tempIndex = 0;
			buffer[tempIndex] = pieceType;
			tempIndex++;
			if(disambig[0]!=' ')
			{
				buffer[tempIndex] = disambig[0];
				tempIndex++;
			}
			if(disambig[1]!=' ')
			{
				buffer[tempIndex] = disambig[1];
				tempIndex++;
			}
			if(capture!=' ')
			{
				buffer[tempIndex] = capture;
				tempIndex++;
			}
			buffer[tempIndex] = destination[0];
			tempIndex++;
			buffer[tempIndex] = destination[1];
			tempIndex++;
			if(promotion[0]!=' ')
			{
				buffer[tempIndex] = promotion[0];
				tempIndex++;
				buffer[tempIndex] = promotion[1];
				tempIndex++;
			}
			//TODO: checks and checkmates
			while(tempIndex<buffer_L)
			{
				buffer[tempIndex] = '\0';
				tempIndex++;
			}
		}
	}
	return buffer;
}
move Position::getMoveFromNotation(const char* notation)
{
	move returnMove = move();
	//Castling
	if(notation[0]=='O' &&
	notation[1]=='-' &&
	notation[2]=='O' &&
	colorToMove=='w')
	{
		returnMove.startRank = 7;
		returnMove.startFile = 4;
		returnMove.endRank = 7;
		returnMove.endFile = 6;
		returnMove.endPieceType = 'k';
		return returnMove;
	}
	if(notation[0]=='O' &&
	notation[1]=='-' &&
	notation[2]=='O' &&
	notation[3]=='-' &&
	notation[4]=='O' &&
	colorToMove=='w')
	{
		returnMove.startRank = 7;
		returnMove.startFile = 4;
		returnMove.endRank = 7;
		returnMove.endFile = 2;
		returnMove.endPieceType = 'k';
		return returnMove;
	}
	if(notation[0]=='O' &&
	notation[1]=='-' &&
	notation[2]=='O' &&
	colorToMove=='b')
	{
		returnMove.startRank = 0;
		returnMove.startFile = 4;
		returnMove.endRank = 0;
		returnMove.endFile = 6;
		returnMove.endPieceType = 'k';
		return returnMove;
	}
	if(notation[0]=='O' &&
	notation[1]=='-' &&
	notation[2]=='O' &&
	notation[3]=='-' &&
	notation[4]=='O' &&
	colorToMove=='b')
	{
		returnMove.startRank = 0;
		returnMove.startFile = 4;
		returnMove.endRank = 0;
		returnMove.endFile = 2;
		returnMove.endPieceType = 'k';
		return returnMove;
	}

	char pieceType = (char)std::tolower(notation[0]);
	int8_t startRank = -1;
	int8_t startFile = -1;
	int8_t endRank = -1;
	int8_t endFile = -1;

	if(isFileOrRank(notation[1]) &&
	isFileOrRank(notation[2]))
	{
		if(!isFileOrRank(notation[3]) &&
		notation[3]!='x')
		{
			//1,2 are destination
			endRank = notationRank_TO_engineRank(notation[2]);
			endFile = notationFile_TO_engineFile(notation[1]);
			for(int i=0;i<theBoard[endRank][endFile].moves_L;i++)
			{
				if(theBoard[endRank][endFile].moves[i]->type==pieceType &&
				theBoard[endRank][endFile].moves[i]->color==colorToMove)
				{
					startRank = theBoard[endRank][endFile].moves[i]->rank;
					startFile = theBoard[endRank][endFile].moves[i]->file;
					break;
				}
			}
			//promotion
			if(notation[3]=='=')
			{
				pieceType = (char)std::tolower(notation[4]);
			}
		}
		else if(isFileOrRank(notation[3]) &&
		!isFileOrRank(notation[4]))
		{
			//single disambig
			endRank = notationRank_TO_engineRank(notation[3]);
			endFile = notationFile_TO_engineFile(notation[2]);
			//rank or file?
			if(isFile(notation[1]))
			{
				//file
				for(int i=0;i<theBoard[endRank][endFile].moves_L;i++)
				{
					if(theBoard[endRank][endFile].moves[i]->type==pieceType &&
					theBoard[endRank][endFile].moves[i]->file==notationFile_TO_engineFile(notation[1]) &&
					theBoard[endRank][endFile].moves[i]->color==colorToMove)
					{
						startRank = theBoard[endRank][endFile].moves[i]->rank;
						startFile = theBoard[endRank][endFile].moves[i]->file;
						break;
					}
				}
			}
			else if(isRank(notation[1]))
			{
				//rank
				for(int i=0;i<theBoard[endRank][endFile].moves_L;i++)
				{
					if(theBoard[endRank][endFile].moves[i]->type==pieceType &&
					theBoard[endRank][endFile].moves[i]->rank==notationRank_TO_engineRank(notation[1]) &&
					theBoard[endRank][endFile].moves[i]->color==colorToMove)
					{
						startRank = theBoard[endRank][endFile].moves[i]->rank;
						startFile = theBoard[endRank][endFile].moves[i]->file;
						break;
					}
				}
			}
			//promotion
			if(notation[4]=='=')
			{
				pieceType = (char)std::tolower(notation[5]);
			}
		}
		else if(isFileOrRank(notation[3]) &&
		isFileOrRank(notation[4]))
		{
			//double disambig
			startRank = notationRank_TO_engineRank(notation[2]);
			startFile = notationFile_TO_engineFile(notation[1]);
			endRank = notationRank_TO_engineRank(notation[4]);
			endFile = notationFile_TO_engineFile(notation[3]);
			//promotion
			if(notation[5]=='=')
			{
				pieceType = (char)std::tolower(notation[6]);
			}
		}
		else if(notation[3]=='x' &&
		isFileOrRank(notation[4]) &&
		isFileOrRank(notation[5]))
		{
			//double disambig capture
			startRank = notationRank_TO_engineRank(notation[2]);
			startFile = notationFile_TO_engineFile(notation[1]);
			endRank = notationRank_TO_engineRank(notation[5]);
			endFile = notationFile_TO_engineFile(notation[4]);
			//promotion
			if(notation[6]=='=')
			{
				pieceType = (char)std::tolower(notation[7]);
			}
		}
	}
	else if(notation[1]=='x' &&
	isFileOrRank(notation[2]) &&
	isFileOrRank(notation[3]))
	{
		//capture then destination
		endRank = notationRank_TO_engineRank(notation[3]);
		endFile = notationFile_TO_engineFile(notation[2]);
		for(int i=0;i<theBoard[endRank][endFile].moves_L;i++)
		{
			if(theBoard[endRank][endFile].moves[i]->type==pieceType &&
			theBoard[endRank][endFile].moves[i]->color==colorToMove)
			{
				startRank = theBoard[endRank][endFile].moves[i]->rank;
				startFile = theBoard[endRank][endFile].moves[i]->file;
				break;
			}
		}
		//promotion
		if(notation[4]=='=')
		{
			pieceType = (char)std::tolower(notation[5]);
		}
	}
	else if(isFileOrRank(notation[1]) &&
	notation[2]=='x' &&
	isFileOrRank(notation[3]) &&
	isFileOrRank(notation[4]))
	{
		//single disambig capture
		endRank = notationRank_TO_engineRank(notation[4]);
		endFile = notationFile_TO_engineFile(notation[3]);
		//rank or file?
		if(isFile(notation[1]))
		{
			//file
			for(int i=0;i<theBoard[endRank][endFile].moves_L;i++)
			{
				if(theBoard[endRank][endFile].moves[i]->type==pieceType &&
				theBoard[endRank][endFile].moves[i]->file==notationFile_TO_engineFile(notation[1]) &&
				theBoard[endRank][endFile].moves[i]->color==colorToMove)
				{
					startRank = theBoard[endRank][endFile].moves[i]->rank;
					startFile = theBoard[endRank][endFile].moves[i]->file;
					break;
				}
			}
		}
		else if(isRank(notation[1]))
		{
			//rank
			for(int i=0;i<theBoard[endRank][endFile].moves_L;i++)
			{
				if(theBoard[endRank][endFile].moves[i]->type==pieceType &&
				theBoard[endRank][endFile].moves[i]->rank==notationRank_TO_engineRank(notation[1]) &&
				theBoard[endRank][endFile].moves[i]->color==colorToMove)
				{
					startRank = theBoard[endRank][endFile].moves[i]->rank;
					startFile = theBoard[endRank][endFile].moves[i]->file;
					break;
				}
			}
		}
		//promotion
		if(notation[5]=='=')
		{
			pieceType = (char)std::tolower(notation[6]);
		}
	}

	if(startRank==-1)
	{
		return returnMove;
	}
	returnMove.startRank = startRank;
	returnMove.startFile = startFile;
	returnMove.endRank = endRank;
	returnMove.endFile = endFile;
	returnMove.endPieceType = pieceType;
	return returnMove;
}
int Position::getTotalTargeters(square* squarePtr, char color)
{
	int total = 0;
	for(int i=0;i<squarePtr->targeters_L;i++)
	{
		if(squarePtr->targeters[i]->color==color)
			total++;
	}
	return total;
}
int Position::getTotalMovers(square* squarePtr, char color)
{
	int total = 0;
	for(int i=0;i<squarePtr->moves_L;i++)
	{
		if(squarePtr->moves[i]->color==color)
			total++;
	}
	return total;
}
char Position::getPieceColor(square* squarePtr)
{
	if(squarePtr->piecePtr==NULL)
	{
		return '\0';
	}
	else
	{
		return squarePtr->piecePtr->color;
	}
}
Position::piece* Position::getKingPtr(char color)
{
	if(color=='w')
	{
		for(int i=0;i<MAX_PIECES;i++)
		{
			if(whitePieces[i].type=='k')
				return &whitePieces[i];
		}
	}
	if(color=='b')
	{
		for(int i=0;i<MAX_PIECES;i++)
		{
			if(blackPieces[i].type=='k')
				return &blackPieces[i];
		}
	}
	return NULL;
}
bool Position::instantEval_passedPawn(int8_t rank, char color, int j)
{
	bool passedPawn;
	if(color=='w')
	{
		passedPawn=true;
		for(int k=rank-1;k>0;k--)
		{
			if(onBoard(k,j-1))
			{
				if(getPieceColor(&theBoard[k][j-1])=='b')
				{
					if(theBoard[k][j-1].piecePtr->type=='p')
					{
						passedPawn = false;
					}
				}
			}
			if(onBoard(k,j+1))
			{
				if(getPieceColor(&theBoard[k][j+1])=='b')
				{
					if(theBoard[k][j+1].piecePtr->type=='p')
					{
						passedPawn = false;
					}
				}
			}
			if(getPieceColor(&theBoard[k][j])=='b')
			{
				if(theBoard[k][j].piecePtr->type=='p')
				{
					passedPawn = false;
				}
			}
		}
		return passedPawn;
	}
	else
	{
		passedPawn=true;
		for(int k=rank+1;k<7;k++)
		{
			if(onBoard(k,j-1))
			{
				if(getPieceColor(&theBoard[k][j-1])=='w')
				{
					if(theBoard[k][j-1].piecePtr->type=='p')
					{
						passedPawn = false;
					}
				}
			}
			if(onBoard(k,j+1))
			{
				if(getPieceColor(&theBoard[k][j+1])=='w')
				{
					if(theBoard[k][j+1].piecePtr->type=='p')
					{
						passedPawn = false;
					}
				}
			}
			if(getPieceColor(&theBoard[k][j])=='w')
			{
				if(theBoard[k][j].piecePtr->type=='p')
				{
					passedPawn = false;
				}
			}
		}
		return passedPawn;
	}
}
double Position::instantEval()
{
	double materialEval=0;
	double positionEval=0;

	if(positionState==positionstate_draw)
		return 0;
	if(positionState==positionstate_blackWin)
		return -200;
	if(positionState==positionstate_whiteWin)
		return 200;
	//Material
	piece* currentPiece;
	for(int i=0;i<MAX_PIECES;i++)
	{
		currentPiece = &whitePieces[i];
		if(currentPiece->type=='\0')
			continue;
		if(currentPiece->type=='q')
			materialEval=materialEval+9;
		if(currentPiece->type=='r')
			materialEval=materialEval+5;
		if(currentPiece->type=='b')
			materialEval=materialEval+3;
		if(currentPiece->type=='n')
			materialEval=materialEval+3;
		if(currentPiece->type=='p')
			materialEval=materialEval+1;
	}
	for(int i=0;i<MAX_PIECES;i++)
	{
		currentPiece = &blackPieces[i];
		if(currentPiece->type=='\0')
			continue;
		if(currentPiece->type=='q')
			materialEval=materialEval-9;
		if(currentPiece->type=='r')
			materialEval=materialEval-5;
		if(currentPiece->type=='b')
			materialEval=materialEval-3;
		if(currentPiece->type=='n')
			materialEval=materialEval-3;
		if(currentPiece->type=='p')
			materialEval=materialEval-1;
	}
	//Positional
	int tempWhiteTargeters;
	int tempBlackTargeters;
	double pieceValue;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			pieceValue=0;
			tempWhiteTargeters=getTotalTargeters(&theBoard[i][j],'w');
			tempBlackTargeters=getTotalTargeters(&theBoard[i][j],'b');
			if(getPieceColor(&theBoard[i][j])!='\0')
			{
				if(theBoard[i][j].piecePtr->type=='q')
					pieceValue=9;
				else if(theBoard[i][j].piecePtr->type=='r')
				{
					pieceValue=5;
				}
				else if(theBoard[i][j].piecePtr->type=='b')
				{
					pieceValue=3;
				}
				else if(theBoard[i][j].piecePtr->type=='n')
				{
					pieceValue=3;
				}
				else if(theBoard[i][j].piecePtr->type=='p')
				{
					pieceValue=1;
				}
				else
				{
					pieceValue=0;
				}
			}
			if(tempWhiteTargeters>tempBlackTargeters)
			{
				positionEval=positionEval+(pieceValue*PIECE_CONTROL_WEIGHT)+SQUARE_CONTROL_WEIGHT;
			}
			else if(tempWhiteTargeters<tempBlackTargeters)
			{
				positionEval=positionEval-(pieceValue*PIECE_CONTROL_WEIGHT)-SQUARE_CONTROL_WEIGHT;
			}
			if(tempWhiteTargeters>=tempBlackTargeters)
			{
				if(getPieceColor(&theBoard[i][j])=='w')
				{
					if(theBoard[i][j].piecePtr->type=='p')
					{
						if(i==5)
						{
							positionEval=positionEval+0.02;
							if(instantEval_passedPawn(5,'w',j))
								positionEval=positionEval+1;
						}
						else if(i==4)
						{
							positionEval=positionEval+0.08;
							if(instantEval_passedPawn(4,'w',j))
								positionEval=positionEval+1;
						}
						else if(i==3)
						{
							positionEval=positionEval+0.15;
							if(instantEval_passedPawn(3,'w',j))
								positionEval=positionEval+1;
						}
						else if(i==2)
						{
							positionEval=positionEval+0.5;
							if(instantEval_passedPawn(2,'w',j))
								positionEval=positionEval+1;
						}
						else if(i==1)
						{
							positionEval=positionEval+2;
						}
					}
				}
			}
			if(tempWhiteTargeters<=tempBlackTargeters)
			{
				if(getPieceColor(&theBoard[i][j])=='b')
				{
					if(theBoard[i][j].piecePtr->type=='p')
					{
						if(i==2)
						{
							positionEval=positionEval-0.02;
							if(instantEval_passedPawn(2,'b',j))
								positionEval=positionEval-1;
						}
						else if(i==3)
						{
							positionEval=positionEval-0.08;
							if(instantEval_passedPawn(3,'b',j))
								positionEval=positionEval-1;
						}
						else if(i==4)
						{
							positionEval=positionEval-0.15;
							if(instantEval_passedPawn(4,'b',j))
								positionEval=positionEval-1;
						}
						else if(i==5)
						{
							positionEval=positionEval-0.5;
							if(instantEval_passedPawn(5,'b',j))
								positionEval=positionEval-1;
						}
						else if(i==6)
						{
							positionEval=positionEval-2;
						}
					}
				}
			}
		}
	}
	return materialEval+positionEval;
}


//-Debug Information-
void Position::printBoard()
{
	printf("\ta\tb\tc\td\te\tf\tg\th\n\n\n");
	for(int i=0;i<8;i++)
	{
		printf("%d",8-i);
		for(int j=0;j<8;j++)
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
	printf("Castling Flags: [%d,%d,%d,%d]\n",castlingFlags[kingside_white],castlingFlags[queenside_white],castlingFlags[kingside_black],castlingFlags[queenside_black]);
	printf("Can Castle: [%d,%d,%d,%d]\n",canCastle[kingside_white],canCastle[queenside_white],canCastle[kingside_black],canCastle[queenside_black]);
	printf("En Passant: (%d,%d)\n",enPassantRank,enPassantFile);
	printf("Fifty Move Rule Counter: %d\n",fiftyMoveRuleCounter);
	printf("Move: %d\n",moveCounter);
}
void Position::printAllTargets()
{
	piece* currentPiecePtr;
	char type;
	char rank;
	char file;
	printf("\n-------White Pieces-------\n");
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(whitePieces[i].type=='\0')
			continue;
		currentPiecePtr = &whitePieces[i];
		type = (char)std::toupper(currentPiecePtr->type);
		rank = engineRank_TO_notationRank(currentPiecePtr->rank);
		file = engineFile_TO_notationFile(currentPiecePtr->file);
		printf("\twhitePieces[%d]: %c%c%c\n",i,type,file,rank);
		for(int j=0;j<currentPiecePtr->targets_L;j++)
		{
			rank = engineRank_TO_notationRank(currentPiecePtr->targets[j]->rank);
			file = engineFile_TO_notationFile(currentPiecePtr->targets[j]->file);
			printf("\t\ttargets[%d]: %c%c\n",j,file,rank);
		}
	}
	printf("\n-------Black Pieces-------\n");
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(whitePieces[i].type=='\0')
			continue;
		currentPiecePtr = &blackPieces[i];
		type = currentPiecePtr->type;
		rank = engineRank_TO_notationRank(currentPiecePtr->rank);
		file = engineFile_TO_notationFile(currentPiecePtr->file);
		printf("\tblackPieces[%d]: %c%c%c\n",i,type,file,rank);
		for(int j=0;j<currentPiecePtr->targets_L;j++)
		{
			rank = engineRank_TO_notationRank(currentPiecePtr->targets[j]->rank);
			file = engineFile_TO_notationFile(currentPiecePtr->targets[j]->file);
			printf("\t\ttargets[%d]: %c%c\n",j,file,rank);
		}
	}
}
void Position::printAllMoves()
{
	piece* currentPiecePtr;
	char type;
	char rank;
	char file;
	printf("\n-------White Pieces-------\n");
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(whitePieces[i].type=='\0')
			continue;
		currentPiecePtr = &whitePieces[i];
		type = (char)std::toupper(currentPiecePtr->type);
		rank = engineRank_TO_notationRank(currentPiecePtr->rank);
		file = engineFile_TO_notationFile(currentPiecePtr->file);
		printf("\twhitePieces[%d]: %c%c%c\n",i,type,file,rank);
		for(int j=0;j<currentPiecePtr->moves_L;j++)
		{
			rank = engineRank_TO_notationRank(currentPiecePtr->moves[j]->rank);
			file = engineFile_TO_notationFile(currentPiecePtr->moves[j]->file);
			printf("\t\tmoves[%d]: %c%c\n",j,file,rank);
		}
	}
	printf("\n-------Black Pieces-------\n");
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(whitePieces[i].type=='\0')
			continue;
		currentPiecePtr = &blackPieces[i];
		type = currentPiecePtr->type;
		rank = engineRank_TO_notationRank(currentPiecePtr->rank);
		file = engineFile_TO_notationFile(currentPiecePtr->file);
		printf("\tblackPieces[%d]: %c%c%c\n",i,type,file,rank);
		for(int j=0;j<currentPiecePtr->moves_L;j++)
		{
			rank = engineRank_TO_notationRank(currentPiecePtr->moves[j]->rank);
			file = engineFile_TO_notationFile(currentPiecePtr->moves[j]->file);
			printf("\t\tmoves[%d]: %c%c\n",j,file,rank);
		}
	}
}

//-Utility-
bool Position::onBoard(int8_t rank, int8_t file)
{
	if(rank>=0 && rank<8 && file>=0 && file<8)
		return true;
	return false;
}


void Position::setupMemory()
{
	//create space for storing piece information for this position
	whitePieces = new piece[MAX_PIECES];
	whitePieces_L = 0;
	blackPieces = new piece[MAX_PIECES];
	blackPieces_L = 0;
	//create space for storing board information for this position
	theBoard = new square*[8];
	for(int i=0;i<8;i++)
	{
		theBoard[i] = new square[8];
		for(int j=0;j<8;j++)
		{
			theBoard[i][j].rank = i;
			theBoard[i][j].file = j;
		}
	}
}

void Position::cleanupMemory()
{
	//remove created heap space
	delete[] whitePieces;
	delete[] blackPieces;
	for(int i=0;i<8;i++)
	{
		delete[] theBoard[i];
	}
	delete[] theBoard;
}

void Position::sanityCheck()
{

}

//-Position Calculation-
void Position::resolve_targets_king(piece* kingPtr, int8_t rank, int8_t file)
{
	if(onBoard(rank,file))
	{
		pushTarget(kingPtr,&theBoard[rank][file]);
	}
}
void Position::resolve_targets_scan(piece* currentPiecePtr, int8_t startingRank, int8_t startingFile, int rankDirection, int fileDirection)
{
	int currentRank;
	int currentFile;
	for(int i=1;i<8 && onBoard(startingRank+(rankDirection*i),startingFile+(fileDirection*i));i++)
	{
		currentRank = startingRank+(rankDirection*i);
		currentFile = startingFile+(fileDirection*i);
		pushTarget(currentPiecePtr,&theBoard[currentRank][currentFile]);
		if(theBoard[currentRank][currentFile].piecePtr!=NULL) //if there is a piece on this square stop scanning
		{
			if(theBoard[currentRank][currentFile].piecePtr->type!='k' ||
			theBoard[currentRank][currentFile].piecePtr->color==theBoard[startingRank][startingFile].piecePtr->color)
			//if on this scan line the scan hits the king of opposite color then we can 'act' as though this piece will target square behind the enemy king
			{
				break;
			}
		}
	}
}
void Position::resolve_targets(char color)
{
	piece* currentPiecePtr;
	if(color=='w')
	{
	//White
		for(int pieceIterator=0;pieceIterator<MAX_PIECES;pieceIterator++)
		{
			currentPiecePtr = &whitePieces[pieceIterator];
			if(currentPiecePtr->type == '\0')
				continue;
			char type = currentPiecePtr->type;
			int8_t rank = currentPiecePtr->rank;
			int8_t file = currentPiecePtr->file;
			//update all targets and moves depending on piece type
			if(type=='k')
			{
				//add target and moveTarget squares for king (total 8)
				resolve_targets_king(currentPiecePtr,rank-1,file-1);
				resolve_targets_king(currentPiecePtr,rank-1,file);
				resolve_targets_king(currentPiecePtr,rank-1,file+1);
				resolve_targets_king(currentPiecePtr,rank,file-1);
				resolve_targets_king(currentPiecePtr,rank,file+1);
				resolve_targets_king(currentPiecePtr,rank+1,file-1);
				resolve_targets_king(currentPiecePtr,rank+1,file);
				resolve_targets_king(currentPiecePtr,rank+1,file+1);
			}
			if(type=='r'||type=='q')
			{
				//scan down
				resolve_targets_scan(currentPiecePtr,rank,file,1,0);
				//scan up
				resolve_targets_scan(currentPiecePtr,rank,file,-1,0);
				//scan right
				resolve_targets_scan(currentPiecePtr,rank,file,0,1);
				//scan left
				resolve_targets_scan(currentPiecePtr,rank,file,0,-1);
			}
			if(type=='b'||type=='q')
			{
				//scan southeast
				resolve_targets_scan(currentPiecePtr,rank,file,1,1);
				//scan southwest
				resolve_targets_scan(currentPiecePtr,rank,file,1,-1);
				//scan northeast
				resolve_targets_scan(currentPiecePtr,rank,file,-1,1);
				//scan northwest
				resolve_targets_scan(currentPiecePtr,rank,file,-1,-1);
			}
			if(type=='n')
			{
				//add target squares for knight
				if(onBoard(rank-2,file-1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-2][file-1]);
				}
				if(onBoard(rank-1,file-2))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-1][file-2]);
				}
				if(onBoard(rank-2,file+1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-2][file+1]);
				}
				if(onBoard(rank-1,file+2))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-1][file+2]);
				}
				if(onBoard(rank+2,file-1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+2][file-1]);
				}
				if(onBoard(rank+1,file-2))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+1][file-2]);
				}
				if(onBoard(rank+2,file+1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+2][file+1]);
				}
				if(onBoard(rank+1,file+2))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+1][file+2]);
				}
			}
			if(type=='p')
			{
				//add target squares for pawn
				if(onBoard(rank-1,file-1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-1][file-1]);
				}
				if(onBoard(rank-1,file+1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-1][file+1]);
				}
			}
		}
	}

	if(color=='b')
	{
	//Black
		for(int pieceIterator=0;pieceIterator<MAX_PIECES;pieceIterator++)
		{
			currentPiecePtr = &blackPieces[pieceIterator];
			if(currentPiecePtr->type == '\0')
				continue;
			char type = currentPiecePtr->type;
			int8_t rank = currentPiecePtr->rank;
			int8_t file = currentPiecePtr->file;
			//update all targets and moves depending on piece type
			if(type=='k')
			{
				//add target and moveTarget squares for king (total 8)
				resolve_targets_king(currentPiecePtr,rank-1,file-1);
				resolve_targets_king(currentPiecePtr,rank-1,file);
				resolve_targets_king(currentPiecePtr,rank-1,file+1);
				resolve_targets_king(currentPiecePtr,rank,file-1);
				resolve_targets_king(currentPiecePtr,rank,file+1);
				resolve_targets_king(currentPiecePtr,rank+1,file-1);
				resolve_targets_king(currentPiecePtr,rank+1,file);
				resolve_targets_king(currentPiecePtr,rank+1,file+1);
			}
			if(type=='r'||type=='q')
			{
				//scan down
				resolve_targets_scan(currentPiecePtr,rank,file,1,0);
				//scan up
				resolve_targets_scan(currentPiecePtr,rank,file,-1,0);
				//scan right
				resolve_targets_scan(currentPiecePtr,rank,file,0,1);
				//scan left
				resolve_targets_scan(currentPiecePtr,rank,file,0,-1);
			}
			if(type=='b'||type=='q')
			{
				//scan southeast
				resolve_targets_scan(currentPiecePtr,rank,file,1,1);
				//scan southwest
				resolve_targets_scan(currentPiecePtr,rank,file,1,-1);
				//scan northeast
				resolve_targets_scan(currentPiecePtr,rank,file,-1,1);
				//scan northwest
				resolve_targets_scan(currentPiecePtr,rank,file,-1,-1);
			}
			if(type=='n')
			{
				//add target squares for knight
				if(onBoard(rank-2,file-1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-2][file-1]);
				}
				if(onBoard(rank-1,file-2))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-1][file-2]);
				}
				if(onBoard(rank-2,file+1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-2][file+1]);
				}
				if(onBoard(rank-1,file+2))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-1][file+2]);
				}
				if(onBoard(rank+2,file-1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+2][file-1]);
				}
				if(onBoard(rank+1,file-2))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+1][file-2]);
				}
				if(onBoard(rank+2,file+1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+2][file+1]);
				}
				if(onBoard(rank+1,file+2))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+1][file+2]);
				}
			}
			if(type=='p')
			{
				//add target squares for pawn
				if(onBoard(rank+1,file-1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+1][file-1]);
				}
				if(onBoard(rank+1,file+1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank+1][file+1]);
				}
			}
		}
	}
}
void Position::resolve_pins_scan(piece* kingPtr, int rankDirection, int fileDirection)
{
	piece* tempSavedPiecePtr = NULL;
	square* currentSquarePtr = NULL;
	bool pieceIsSaved = false;
	char kingColor = kingPtr->color;
	int8_t kingRank = kingPtr->rank;
	int8_t kingFile = kingPtr->file;

	for(int i=1;i<8 && onBoard(kingRank+(rankDirection*i),kingFile+(fileDirection*i));i++)
	{
		currentSquarePtr = &theBoard[kingRank+(rankDirection*i)][kingFile+(fileDirection*i)];
		if(!pieceIsSaved)
		{
			if(currentSquarePtr->piecePtr!=NULL)
			{
				if(currentSquarePtr->piecePtr->color==kingColor)
				{
					tempSavedPiecePtr = currentSquarePtr->piecePtr;
					pieceIsSaved = true;
				}
				else
					break;
			}
		}
		else
		{
			if(currentSquarePtr->piecePtr!=NULL)
			{
				if(currentSquarePtr->piecePtr->color!=kingColor)
				{
					if(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r')
					{
						if((rankDirection==1 || rankDirection==-1) && (fileDirection==0)) //file pin
						{
							tempSavedPiecePtr->pinned[pin_file]=true;
							break;
						}
						else if((fileDirection==1 || fileDirection==-1) && (rankDirection==0)) //rank pin
						{
							tempSavedPiecePtr->pinned[pin_rank]=true;
							break;
						}
					}
					if(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b')
					{
						if((rankDirection==1 && fileDirection==1) || (rankDirection==-1 && fileDirection==-1)) //positive diagonal pin
						{
							tempSavedPiecePtr->pinned[pin_positiveDiag]=true;
							break;
						}
						else if((rankDirection==1 && fileDirection==-1) || (rankDirection==-1 && fileDirection==1)) //negative diagonal pin
						{
							tempSavedPiecePtr->pinned[pin_negativeDiag]=true;
							break;
						}
					}
				}
				else
					break;
			}
		}
	}
}
void Position::resolve_pins(char color)
{
	if(color=='w')
	{
		piece* whiteKingPtr = getKingPtr('w');
		//scan down
		resolve_pins_scan(whiteKingPtr,1,0);
		//scan up
		resolve_pins_scan(whiteKingPtr,-1,0);
		//scan right
		resolve_pins_scan(whiteKingPtr,0,1);
		//scan left
		resolve_pins_scan(whiteKingPtr,0,-1);
		//scan southeast
		resolve_pins_scan(whiteKingPtr,1,1);
		//scan southwest
		resolve_pins_scan(whiteKingPtr,1,-1);
		//scan northeast
		resolve_pins_scan(whiteKingPtr,-1,1);
		//scan northwest
		resolve_pins_scan(whiteKingPtr,-1,-1);
	}
	if(color=='b')
	{
		piece* blackKingPtr = getKingPtr('b');
		//scan down
		resolve_pins_scan(blackKingPtr,1,0);
		//scan up
		resolve_pins_scan(blackKingPtr,-1,0);
		//scan right
		resolve_pins_scan(blackKingPtr,0,1);
		//scan left
		resolve_pins_scan(blackKingPtr,0,-1);
		//scan southeast
		resolve_pins_scan(blackKingPtr,1,1);
		//scan southwest
		resolve_pins_scan(blackKingPtr,1,-1);
		//scan northeast
		resolve_pins_scan(blackKingPtr,-1,1);
		//scan northwest
		resolve_pins_scan(blackKingPtr,-1,-1);
	}

}

void Position::resolve_moves_whiteKing(piece* kingPtr, int8_t rank, int8_t file)
{
	if(onBoard(rank,file))
	{
		//if square has no black targeters and is not occupied by the same color piece then assume the king can move there
		if(getTotalTargeters(&theBoard[rank][file],'b')==0 && getPieceColor(&theBoard[rank][file])!='w')
		{
			pushMove(kingPtr,&theBoard[rank][file]);
		}
	}
}
void Position::resolve_moves_blackKing(piece* kingPtr, int8_t rank, int8_t file)
{
	if(onBoard(rank,file))
	{
		//if square has no white targeters and is not occupied by the same color piece then assume the king can move there
		if(getTotalTargeters(&theBoard[rank][file],'w')==0 && getPieceColor(&theBoard[rank][file])!='b')
		{
			pushMove(kingPtr,&theBoard[rank][file]);
		}
	}
}
void Position::resolve_moves_knight(piece* currentPiecePtr, int rankOffset, int fileOffset)
{
	char pieceColor = currentPiecePtr->color;
	int8_t startingRank = currentPiecePtr->rank;
	int8_t startingFile = currentPiecePtr->file;
	if(onBoard(startingRank+rankOffset,startingFile+fileOffset))
	{
		if(getPieceColor(&theBoard[startingRank+rankOffset][startingFile+fileOffset])!=pieceColor)
		{
			pushMove(currentPiecePtr,&theBoard[startingRank+rankOffset][startingFile+fileOffset]);
		}
	}
}
void Position::resolve_moves_scan(piece* currentPiecePtr, int8_t startingRank, int8_t startingFile, int rankDirection, int fileDirection)
{
	int currentRank;
	int currentFile;
	char pieceColor = currentPiecePtr->color;
	for(int i=1;i<8 && onBoard(startingRank+(rankDirection*i),startingFile+(fileDirection*i));i++)
	{
		currentRank = startingRank+(rankDirection*i);
		currentFile = startingFile+(fileDirection*i);
		if(getPieceColor(&theBoard[currentRank][currentFile])!=pieceColor)
			pushMove(currentPiecePtr,&theBoard[currentRank][currentFile]);
		if(theBoard[currentRank][currentFile].piecePtr!=NULL)
		{
			break; //if there is a piece on this square stop scanning
		}
	}
}
void Position::resolve_moves(char color)
{
	piece* currentPiecePtr;
	if(color=='w')
	{
	//White
		for(int pieceIterator=0;pieceIterator<MAX_PIECES;pieceIterator++)
		{
			currentPiecePtr = &whitePieces[pieceIterator];
			if(currentPiecePtr->type == '\0')
				continue;
			char type = currentPiecePtr->type;
			int8_t rank = currentPiecePtr->rank;
			int8_t file = currentPiecePtr->file;
			//update all moves and moves depending on piece type
			if(type=='k')
			{
				//add target and moveTarget squares for king (total 8)
				resolve_moves_whiteKing(currentPiecePtr,rank-1,file-1);
				resolve_moves_whiteKing(currentPiecePtr,rank-1,file);
				resolve_moves_whiteKing(currentPiecePtr,rank-1,file+1);
				resolve_moves_whiteKing(currentPiecePtr,rank,file-1);
				resolve_moves_whiteKing(currentPiecePtr,rank,file+1);
				resolve_moves_whiteKing(currentPiecePtr,rank+1,file-1);
				resolve_moves_whiteKing(currentPiecePtr,rank+1,file);
				resolve_moves_whiteKing(currentPiecePtr,rank+1,file+1);
			}
			if(type=='r'||type=='q')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan down
					resolve_moves_scan(currentPiecePtr,rank,file,1,0);
					//scan up
					resolve_moves_scan(currentPiecePtr,rank,file,-1,0);
				}
				if(!currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan right
					resolve_moves_scan(currentPiecePtr,rank,file,0,1);
					//scan left
					resolve_moves_scan(currentPiecePtr,rank,file,0,-1);
				}
			}
			if(type=='b'||type=='q')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					//scan southeast
					resolve_moves_scan(currentPiecePtr,rank,file,1,1);
					//scan northwest
					resolve_moves_scan(currentPiecePtr,rank,file,-1,-1);
				}
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan southwest
					resolve_moves_scan(currentPiecePtr,rank,file,1,-1);
					//scan northeast
					resolve_moves_scan(currentPiecePtr,rank,file,-1,1);
				}
			}
			if(type=='n')
			{
				//add moves for knight
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					resolve_moves_knight(currentPiecePtr,-2,-1);
					resolve_moves_knight(currentPiecePtr,-1,-2);
					resolve_moves_knight(currentPiecePtr,-2,1);
					resolve_moves_knight(currentPiecePtr,-1,2);
					resolve_moves_knight(currentPiecePtr,2,-1);
					resolve_moves_knight(currentPiecePtr,1,-2);
					resolve_moves_knight(currentPiecePtr,2,1);
					resolve_moves_knight(currentPiecePtr,1,2);
				}
			}
			if(type=='p')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					if(onBoard(rank-1,file-1))
					{
						if((theBoard[rank-1][file-1].piecePtr!=NULL || (enPassantFile==file-1 && enPassantRank==rank-1)) &&
						getPieceColor(&theBoard[rank-1][file-1])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank-1][file-1]);
						}
					}
				}
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					if(onBoard(rank-1,file+1))
					{
						if((theBoard[rank-1][file+1].piecePtr!=NULL || (enPassantFile==file+1 && enPassantRank==rank-1)) &&
						getPieceColor(&theBoard[rank-1][file+1])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank-1][file+1]);
						}
					}
				}
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					if(onBoard(rank-1,file))
					{
						 if(theBoard[rank-1][file].piecePtr==NULL)
						 {
							 pushMove(currentPiecePtr,&theBoard[rank-1][file]);
							 if(rank==6 && theBoard[rank-2][file].piecePtr==NULL)
							 {
								pushMove(currentPiecePtr,&theBoard[rank-2][file]);
							 }
						 }
					}
				}
			}
		}
	}
	if(color=='b')
	{
	//Black
		for(int pieceIterator=0;pieceIterator<MAX_PIECES;pieceIterator++)
		{
			currentPiecePtr = &blackPieces[pieceIterator];
			if(currentPiecePtr->type == '\0')
				continue;
			char type = currentPiecePtr->type;
			int8_t rank = currentPiecePtr->rank;
			int8_t file = currentPiecePtr->file;
			//update all moves and moves depending on piece type
			if(type=='k')
			{
				//add target and moveTarget squares for king (total 8)
				resolve_moves_blackKing(currentPiecePtr,rank-1,file-1);
				resolve_moves_blackKing(currentPiecePtr,rank-1,file);
				resolve_moves_blackKing(currentPiecePtr,rank-1,file+1);
				resolve_moves_blackKing(currentPiecePtr,rank,file-1);
				resolve_moves_blackKing(currentPiecePtr,rank,file+1);
				resolve_moves_blackKing(currentPiecePtr,rank+1,file-1);
				resolve_moves_blackKing(currentPiecePtr,rank+1,file);
				resolve_moves_blackKing(currentPiecePtr,rank+1,file+1);
			}
			if(type=='r'||type=='q')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan down
					resolve_moves_scan(currentPiecePtr,rank,file,1,0);
					//scan up
					resolve_moves_scan(currentPiecePtr,rank,file,-1,0);
				}
				if(!currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan right
					resolve_moves_scan(currentPiecePtr,rank,file,0,1);
					//scan left
					resolve_moves_scan(currentPiecePtr,rank,file,0,-1);
				}
			}
			if(type=='b'||type=='q')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					//scan southeast
					resolve_moves_scan(currentPiecePtr,rank,file,1,1);
					//scan northwest
					resolve_moves_scan(currentPiecePtr,rank,file,-1,-1);
				}
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan southwest
					resolve_moves_scan(currentPiecePtr,rank,file,1,-1);
					//scan northeast
					resolve_moves_scan(currentPiecePtr,rank,file,-1,1);
				}
			}
			if(type=='n')
			{
				//add moves for knight
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					resolve_moves_knight(currentPiecePtr,-2,-1);
					resolve_moves_knight(currentPiecePtr,-1,-2);
					resolve_moves_knight(currentPiecePtr,-2,1);
					resolve_moves_knight(currentPiecePtr,-1,2);
					resolve_moves_knight(currentPiecePtr,2,-1);
					resolve_moves_knight(currentPiecePtr,1,-2);
					resolve_moves_knight(currentPiecePtr,2,1);
					resolve_moves_knight(currentPiecePtr,1,2);
				}
			}
			if(type=='p')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					if(onBoard(rank+1,file-1))
					{
						if((theBoard[rank+1][file-1].piecePtr!=NULL || (enPassantFile==file-1 && enPassantRank==rank+1)) &&
						getPieceColor(&theBoard[rank+1][file-1])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank+1][file-1]);
						}
					}
				}
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					if(onBoard(rank+1,file+1))
					{
						if((theBoard[rank+1][file+1].piecePtr!=NULL || (enPassantFile==file+1 && enPassantRank==rank+1)) &&
						getPieceColor(&theBoard[rank+1][file+1])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank+1][file+1]);
						}
					}
				}
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					if(onBoard(rank+1,file))
					{
						 if(theBoard[rank+1][file].piecePtr==NULL)
						 {
							 pushMove(currentPiecePtr,&theBoard[rank+1][file]);
							 if(rank==1 && theBoard[rank+2][file].piecePtr==NULL)
							 {
								pushMove(currentPiecePtr,&theBoard[rank+2][file]);
							 }
						 }
					}
				}
			}
		}
	}
}
bool Position::resolve_movesInCheck_blocksCheck(const piece* kingPtr, const piece* checkingPiecePtr, int8_t endRank, int8_t endFile)
{
	if(checkingPiecePtr->type=='r'||checkingPiecePtr->type=='q')
	{
		if(kingPtr->rank==checkingPiecePtr->rank)
		{
			if(kingPtr->file>checkingPiecePtr->file && endFile>checkingPiecePtr->file && endFile<kingPtr->file && endRank==kingPtr->rank)
			{
				return true;
			}
			if(kingPtr->file<checkingPiecePtr->file && endFile<checkingPiecePtr->file && endFile>kingPtr->file && endRank==kingPtr->rank)
			{
				return true;
			}
		}
		else if(kingPtr->file==checkingPiecePtr->file)
		{
			if(kingPtr->rank>checkingPiecePtr->rank && endRank>checkingPiecePtr->rank && endRank<kingPtr->rank && endFile==kingPtr->file)
			{
				return true;
			}
			if(kingPtr->rank<checkingPiecePtr->rank && endRank<checkingPiecePtr->rank && endRank>kingPtr->rank && endFile==kingPtr->file)
			{
				return true;
			}
		}
	}
	if(checkingPiecePtr->type=='b'||checkingPiecePtr->type=='q')
	{
		//Positive Axis
		if((kingPtr->rank-checkingPiecePtr->rank)==(kingPtr->file-checkingPiecePtr->file))
		{
			//checking piece above, king below
			if(kingPtr->rank>checkingPiecePtr->rank)
			{
				if(endRank<kingPtr->rank &&
				endRank>checkingPiecePtr->rank &&
				endFile<kingPtr->file &&
				endFile>checkingPiecePtr->file &&
				(endRank-kingPtr->rank)==(endFile-kingPtr->file))
				{
					return true;
				}
			}
			//checking piece below, king above
			else if(kingPtr->rank<checkingPiecePtr->rank)
			{
				if(endRank>kingPtr->rank &&
				endRank<checkingPiecePtr->rank &&
				endFile>kingPtr->file &&
				endFile<checkingPiecePtr->file &&
				(endRank-kingPtr->rank)==(endFile-kingPtr->file))
				{
					return true;
				}
			}
		}
		//Negative Axis
		else if((kingPtr->rank-checkingPiecePtr->rank)==(kingPtr->file-checkingPiecePtr->file)*-1)
		{
			//checking piece above, king below
			if(kingPtr->rank>checkingPiecePtr->rank)
			{
				if(endRank<kingPtr->rank &&
				endRank>checkingPiecePtr->rank &&
				endFile>kingPtr->file &&
				endFile<checkingPiecePtr->file &&
				(endRank-kingPtr->rank)==(endFile-kingPtr->file)*-1)
				{
					return true;
				}
			}
			//checking piece below, king above
			else if(kingPtr->rank<checkingPiecePtr->rank)
			{
				if(endRank>kingPtr->rank &&
				endRank<checkingPiecePtr->rank &&
				endFile<kingPtr->file &&
				endFile>checkingPiecePtr->file &&
				(endRank-kingPtr->rank)==(endFile-kingPtr->file)*-1)
				{
					return true;
				}
			}
		}
	}
	return false;
}
void Position::resolve_movesInCheck_scan_blocksAndCaptures(piece* kingPtr, piece* currentPiecePtr, piece* checkingPiece, int rankDirection, int fileDirection)
{
	int currentRank;
	int currentFile;
	int8_t startingRank = currentPiecePtr->rank;
	int8_t startingFile = currentPiecePtr->file;
	char pieceColor = currentPiecePtr->color;
	for(int i=1;i<8 && onBoard(startingRank+(rankDirection*i),startingFile+(fileDirection*i));i++)
	{
		currentRank = startingRank+(rankDirection*i);
		currentFile = startingFile+(fileDirection*i);
		if(getPieceColor(&theBoard[currentRank][currentFile])!=pieceColor)
		{
			if((currentRank==checkingPiece->rank && currentFile==checkingPiece->file)||resolve_movesInCheck_blocksCheck(kingPtr,checkingPiece,currentRank,currentFile))
			{
				pushMove(currentPiecePtr,&theBoard[currentRank][currentFile]);
			}
		}
		if(theBoard[currentRank][currentFile].piecePtr!=NULL)
		{
			break; //if there is a piece on this square stop scanning
		}
	}
}
void Position::resolve_movesInCheck_knight_capturesOnly(piece* currentPiecePtr, piece* checkingPiece, int rankOffset, int fileOffset)
{
	char pieceColor = currentPiecePtr->color;
	int8_t startingRank = currentPiecePtr->rank;
	int8_t startingFile = currentPiecePtr->file;
	if(onBoard(startingRank+rankOffset,startingFile+fileOffset) &&
	startingRank+rankOffset==checkingPiece->rank &&
	startingFile+fileOffset==checkingPiece->file)
	{
		if(getPieceColor(&theBoard[startingRank+rankOffset][startingFile+fileOffset])!=pieceColor)
		{
			pushMove(currentPiecePtr,&theBoard[startingRank+rankOffset][startingFile+fileOffset]);
		}
	}
}
void Position::resolve_movesInCheck_knight_blocksAndCaptures(piece* kingPtr, piece* currentPiecePtr, piece* checkingPiece, int rankOffset, int fileOffset)
{
	char pieceColor = currentPiecePtr->color;
	int8_t startingRank = currentPiecePtr->rank;
	int8_t startingFile = currentPiecePtr->file;
	if(onBoard(startingRank+rankOffset,startingFile+fileOffset) &&
	((startingRank+rankOffset==checkingPiece->rank && startingFile+fileOffset==checkingPiece->file) ||
	resolve_movesInCheck_blocksCheck(kingPtr,checkingPiece,startingRank+rankOffset,startingFile+fileOffset)))
	{
		if(getPieceColor(&theBoard[startingRank+rankOffset][startingFile+fileOffset])!=pieceColor)
		{
			pushMove(currentPiecePtr,&theBoard[startingRank+rankOffset][startingFile+fileOffset]);
		}
	}
}
void Position::resolve_movesInCheck_scan_capturesOnly(piece* currentPiecePtr, piece* checkingPiece, int rankDirection, int fileDirection)
{
	int currentRank;
	int currentFile;
	int8_t startingRank = currentPiecePtr->rank;
	int8_t startingFile = currentPiecePtr->file;
	char pieceColor = currentPiecePtr->color;
	for(int i=1;i<8 && onBoard(startingRank+(rankDirection*i),startingFile+(fileDirection*i));i++)
	{
		currentRank = startingRank+(rankDirection*i);
		currentFile = startingFile+(fileDirection*i);
		if(getPieceColor(&theBoard[currentRank][currentFile])!=pieceColor)
		{
			if((currentRank==checkingPiece->rank && currentFile==checkingPiece->file))
			{
				pushMove(currentPiecePtr,&theBoard[currentRank][currentFile]);
			}
		}
		if(theBoard[currentRank][currentFile].piecePtr!=NULL)
		{
			break; //if there is a piece on this square stop scanning
		}
	}
}
void Position::resolve_movesInCheck(char color)
{
	piece* currentPiecePtr;
	if(color=='w')
	{
	//White
		piece* whiteKingPtr = getKingPtr('w');
		//Moving the king is always an option so long as the square is not blocked and has no targeters
		resolve_moves_whiteKing(whiteKingPtr,whiteKingPtr->rank-1,whiteKingPtr->file-1);
		resolve_moves_whiteKing(whiteKingPtr,whiteKingPtr->rank-1,whiteKingPtr->file);
		resolve_moves_whiteKing(whiteKingPtr,whiteKingPtr->rank-1,whiteKingPtr->file+1);
		resolve_moves_whiteKing(whiteKingPtr,whiteKingPtr->rank,whiteKingPtr->file-1);
		resolve_moves_whiteKing(whiteKingPtr,whiteKingPtr->rank,whiteKingPtr->file+1);
		resolve_moves_whiteKing(whiteKingPtr,whiteKingPtr->rank+1,whiteKingPtr->file-1);
		resolve_moves_whiteKing(whiteKingPtr,whiteKingPtr->rank+1,whiteKingPtr->file);
		resolve_moves_whiteKing(whiteKingPtr,whiteKingPtr->rank+1,whiteKingPtr->file+1);
		//1. Double check: only valid move is king moves
		if(getTotalTargeters(whiteKingPtr->squarePtr,'b')==2)
		{
			//do nothing, legal king moves are already resolved
		}
		//2. Single check: blocks, captures, king moves
		else
		{
			piece* checkingPiece; //find piece causing check and save it
			for(int i=0;i<whiteKingPtr->squarePtr->targeters_L;i++)
			{
				if(whiteKingPtr->squarePtr->targeters[i]->color=='b')
				{
					checkingPiece = whiteKingPtr->squarePtr->targeters[i];
					break;
				}
			}
			int8_t checkingPieceRank = checkingPiece->rank;
			int8_t checkingPieceFile = checkingPiece->file;

			if(checkingPiece->type=='n'||checkingPiece->type=='p') //only captures and moves
			{
				for(int pieceIterator=0;pieceIterator<MAX_PIECES;pieceIterator++)
				{
					currentPiecePtr = &whitePieces[pieceIterator];
					if(currentPiecePtr->type == '\0')
						continue;
					char type = currentPiecePtr->type;
					int8_t rank = currentPiecePtr->rank;
					int8_t file = currentPiecePtr->file;
					//update all moves and moves depending on piece type
					if(type=='r'||type=='q')
					{
						if(currentPiecePtr->file==checkingPieceFile)
						{
							if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
							{
								//scan down
								resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,1,0);
								//scan up
								resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,-1,0);
							}
						}
						if(currentPiecePtr->rank==checkingPieceRank)
						{
							if(!currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
							{
								//scan right
								resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,0,1);
								//scan left
								resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,0,-1);
							}
						}
					}
					if(type=='b'||type=='q')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							//scan southeast
							resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,1,1);
							//scan northwest
							resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,-1,-1);
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							//scan southwest
							resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,1,-1);
							//scan northeast
							resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,-1,1);
						}
					}
					if(type=='n')
					{
						//add moves for knight
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							resolve_movesInCheck_knight_capturesOnly(currentPiecePtr,checkingPiece,-2,-1);
							resolve_movesInCheck_knight_capturesOnly(currentPiecePtr,checkingPiece,-1,-2);
							resolve_movesInCheck_knight_capturesOnly(currentPiecePtr,checkingPiece,-2,1);
							resolve_movesInCheck_knight_capturesOnly(currentPiecePtr,checkingPiece,-1,2);
							resolve_movesInCheck_knight_capturesOnly(currentPiecePtr,checkingPiece,2,-1);
							resolve_movesInCheck_knight_capturesOnly(currentPiecePtr,checkingPiece,1,-2);
							resolve_movesInCheck_knight_capturesOnly(currentPiecePtr,checkingPiece,2,1);
							resolve_movesInCheck_knight_capturesOnly(currentPiecePtr,checkingPiece,1,2);
						}
					}
					if(type=='p')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							if(onBoard(rank-1,file-1) && rank-1==checkingPieceRank && file-1==checkingPieceFile)
							{
								if((theBoard[rank-1][file-1].piecePtr!=NULL || (enPassantFile==file-1 && enPassantRank==rank-1)) &&
								getPieceColor(&theBoard[rank-1][file-1])!='w')
								{
									pushMove(currentPiecePtr,&theBoard[rank-1][file-1]);
								}
							}
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							if(onBoard(rank-1,file+1) && rank-1==checkingPieceRank && file+1==checkingPieceFile)
							{
								if((theBoard[rank-1][file+1].piecePtr!=NULL || (enPassantFile==file+1 && enPassantRank==rank-1)) &&
								getPieceColor(&theBoard[rank-1][file+1])!='w')
								{
									pushMove(currentPiecePtr,&theBoard[rank-1][file+1]);
								}
							}
						}
					}
				}
			}

			else //blocking, captures, and moves
			{
				for(int pieceIterator=0;pieceIterator<MAX_PIECES;pieceIterator++)
				{
					currentPiecePtr = &whitePieces[pieceIterator];
					if(currentPiecePtr->type == '\0')
						continue;
					char type = currentPiecePtr->type;
					int8_t rank = currentPiecePtr->rank;
					int8_t file = currentPiecePtr->file;
					//update all moves and moves depending on piece type
					if(type=='r'||type=='q')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							//scan down
							resolve_movesInCheck_scan_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,1,0);
							//scan up
							resolve_movesInCheck_scan_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,-1,0);
						}
						if(!currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							//scan right
							resolve_movesInCheck_scan_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,0,1);
							//scan left
							resolve_movesInCheck_scan_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,0,-1);
						}
					}
					if(type=='b'||type=='q')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							//scan southeast
							resolve_movesInCheck_scan_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,1,1);
							//scan northwest
							resolve_movesInCheck_scan_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,-1,-1);
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							//scan southwest
							resolve_movesInCheck_scan_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,1,-1);
							//scan northeast
							resolve_movesInCheck_scan_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,-1,1);
						}
					}
					if(type=='n')
					{
						//add moves for knight
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							resolve_movesInCheck_knight_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,-2,-1);
							resolve_movesInCheck_knight_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,-1,-2);
							resolve_movesInCheck_knight_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,-2,1);
							resolve_movesInCheck_knight_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,-1,2);
							resolve_movesInCheck_knight_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,2,-1);
							resolve_movesInCheck_knight_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,1,-2);
							resolve_movesInCheck_knight_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,2,1);
							resolve_movesInCheck_knight_blocksAndCaptures(whiteKingPtr,currentPiecePtr,checkingPiece,1,2);
						}
					}
					if(type=='p')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							if(onBoard(rank-1,file))
							{
								if(theBoard[rank-1][file].piecePtr==NULL)
								{
									if(resolve_movesInCheck_blocksCheck(whiteKingPtr,checkingPiece,rank-1,file))
									{
										pushMove(currentPiecePtr,&theBoard[rank-1][file]);
									}
									if(rank==6 && theBoard[rank-2][file].piecePtr==NULL)
									{
										if(resolve_movesInCheck_blocksCheck(whiteKingPtr,checkingPiece,rank-2,file))
										{
											pushMove(currentPiecePtr,&theBoard[rank-2][file]);
										}
									}
								}
							}
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							if(onBoard(rank-1,file-1) && rank-1==checkingPieceRank && file-1==checkingPieceFile)
							{
								if((theBoard[rank-1][file-1].piecePtr!=NULL || (enPassantFile==file-1 && enPassantRank==rank-1)) &&
								getPieceColor(&theBoard[rank-1][file-1])!='w')
								{
									pushMove(currentPiecePtr,&theBoard[rank-1][file-1]);
								}
							}
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							if(onBoard(rank-1,file+1) && rank-1==checkingPieceRank && file+1==checkingPieceFile)
							{
								if((theBoard[rank-1][file+1].piecePtr!=NULL || (enPassantFile==file+1 && enPassantRank==rank-1)) &&
								getPieceColor(&theBoard[rank-1][file+1])!='w')
								{
									pushMove(currentPiecePtr,&theBoard[rank-1][file+1]);
								}
							}
						}
					}
				}
			}
		}
	}
	if(color=='b')
	{
		//Black
		piece* blackKingPtr = getKingPtr('b');
		//Moving the king is always an option so long as the square is not blocked and has no targeters
		resolve_moves_blackKing(blackKingPtr,blackKingPtr->rank-1,blackKingPtr->file-1);
		resolve_moves_blackKing(blackKingPtr,blackKingPtr->rank-1,blackKingPtr->file);
		resolve_moves_blackKing(blackKingPtr,blackKingPtr->rank-1,blackKingPtr->file+1);
		resolve_moves_blackKing(blackKingPtr,blackKingPtr->rank,blackKingPtr->file-1);
		resolve_moves_blackKing(blackKingPtr,blackKingPtr->rank,blackKingPtr->file+1);
		resolve_moves_blackKing(blackKingPtr,blackKingPtr->rank+1,blackKingPtr->file-1);
		resolve_moves_blackKing(blackKingPtr,blackKingPtr->rank+1,blackKingPtr->file);
		resolve_moves_blackKing(blackKingPtr,blackKingPtr->rank+1,blackKingPtr->file+1);
		//1. Double check: only valid move is king moves
		if(getTotalTargeters(blackKingPtr->squarePtr,'w')==2)
		{
			//do nothing, legal king moves are already resolved
		}
		//2. Single check: blocks, captures, king moves
		else
		{
			piece* checkingPiece; //find piece causing check and save it
			for(int i=0;i<blackKingPtr->squarePtr->targeters_L;i++)
			{
				if(blackKingPtr->squarePtr->targeters[i]->color=='w')
				{
					checkingPiece = blackKingPtr->squarePtr->targeters[i];
					break;
				}
			}
			int8_t checkingPieceRank = checkingPiece->rank;
			int8_t checkingPieceFile = checkingPiece->file;
			if(checkingPiece->type=='n'||checkingPiece->type=='p') //only captures and moves
			{
				for(int pieceIterator=0;pieceIterator<MAX_PIECES;pieceIterator++)
				{
					currentPiecePtr = &blackPieces[pieceIterator];
					if(currentPiecePtr->type == '\0')
						continue;
					char type = currentPiecePtr->type;
					int8_t rank = currentPiecePtr->rank;
					int8_t file = currentPiecePtr->file;
					//update all moves and moves depending on piece type
					if(type=='r'||type=='q')
					{
						if(currentPiecePtr->file==checkingPieceFile)
						{
							if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
							{
								//scan down
								resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,1,0);
								//scan up
								resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,-1,0);
							}
						}
						if(currentPiecePtr->rank==checkingPieceRank)
						{
							if(!currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
							{
								//scan right
								resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,0,1);
								//scan left
								resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,0,-1);
							}
						}
					}
					if(type=='b'||type=='q')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							//scan southeast
							resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,1,1);
							//scan northwest
							resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,-1,-1);
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							//scan southwest
							resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,1,-1);
							//scan northeast
							resolve_movesInCheck_scan_capturesOnly(currentPiecePtr,checkingPiece,-1,1);
						}
					}
					if(type=='n')
					{
						//add moves for knight
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-2,-1);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-1,-2);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-2,1);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-1,2);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,2,-1);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,1,-2);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,2,1);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,1,2);
						}
					}
					if(type=='p')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							if(onBoard(rank+1,file-1) && rank+1==checkingPieceRank && file-1==checkingPieceFile)
							{
								if((theBoard[rank+1][file-1].piecePtr!=NULL || (enPassantFile==file-1 && enPassantRank==rank+1)) &&
								getPieceColor(&theBoard[rank+1][file-1])!='w')
								{
									pushMove(currentPiecePtr,&theBoard[rank+1][file-1]);
								}
							}
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							if(onBoard(rank+1,file+1) && rank+1==checkingPieceRank && file+1==checkingPieceFile)
							{
								if((theBoard[rank+1][file+1].piecePtr!=NULL || (enPassantFile==file+1 && enPassantRank==rank+1)) &&
								getPieceColor(&theBoard[rank+1][file+1])!='w')
								{
									pushMove(currentPiecePtr,&theBoard[rank+1][file+1]);
								}
							}
						}
					}
				}
			}
			else //blocking, captures, and moves
			{
				for(int pieceIterator=0;pieceIterator<MAX_PIECES;pieceIterator++)
				{
					currentPiecePtr = &blackPieces[pieceIterator];
					if(currentPiecePtr->type == '\0')
						continue;
					char type = currentPiecePtr->type;
					int8_t rank = currentPiecePtr->rank;
					int8_t file = currentPiecePtr->file;
					//update all moves and moves depending on piece type
					if(type=='r'||type=='q')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							//scan down
							resolve_movesInCheck_scan_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,1,0);
							//scan up
							resolve_movesInCheck_scan_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-1,0);
						}
						if(!currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							//scan right
							resolve_movesInCheck_scan_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,0,1);
							//scan left
							resolve_movesInCheck_scan_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,0,-1);
						}
					}
					if(type=='b'||type=='q')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							//scan southeast
							resolve_movesInCheck_scan_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,1,1);
							//scan northwest
							resolve_movesInCheck_scan_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-1,-1);
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							//scan southwest
							resolve_movesInCheck_scan_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,1,-1);
							//scan northeast
							resolve_movesInCheck_scan_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-1,1);
						}
					}
					if(type=='n')
					{
						//add moves for knight
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-2,-1);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-1,-2);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-2,1);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,-1,2);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,2,-1);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,1,-2);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,2,1);
							resolve_movesInCheck_knight_blocksAndCaptures(blackKingPtr,currentPiecePtr,checkingPiece,1,2);
						}
					}
					if(type=='p')
					{
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							if(onBoard(rank+1,file))
							{
								if(theBoard[rank+1][file].piecePtr==NULL)
								{
									if(resolve_movesInCheck_blocksCheck(blackKingPtr,checkingPiece,rank+1,file))
									{
										pushMove(currentPiecePtr,&theBoard[rank+1][file]);
									}
									if(rank==1 && theBoard[rank+2][file].piecePtr==NULL)
									{
										if(resolve_movesInCheck_blocksCheck(blackKingPtr,checkingPiece,rank+2,file))
										{
											pushMove(currentPiecePtr,&theBoard[rank+2][file]);
										}
									}
								}
							}
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
						{
							if(onBoard(rank+1,file-1) && rank+1==checkingPieceRank && file-1==checkingPieceFile)
							{
								if((theBoard[rank+1][file-1].piecePtr!=NULL || (enPassantFile==file-1 && enPassantRank==rank+1)) &&
								getPieceColor(&theBoard[rank+1][file-1])!='w')
								{
									pushMove(currentPiecePtr,&theBoard[rank+1][file-1]);
								}
							}
						}
						if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
						{
							if(onBoard(rank+1,file+1) && rank+1==checkingPieceRank && file+1==checkingPieceFile)
							{
								if((theBoard[rank+1][file+1].piecePtr!=NULL || (enPassantFile==file+1 && enPassantRank==rank+1)) &&
								getPieceColor(&theBoard[rank+1][file+1])!='w')
								{
									pushMove(currentPiecePtr,&theBoard[rank+1][file+1]);
								}
							}
						}
					}
				}
			}
		}
	}
}
void Position::resolve_positionState()
{
	if(fiftyMoveRuleCounter==100)
	{
		positionState = positionstate_draw;
		return;
	}
	piece currentPiece;
	if(colorToMove=='w')
	{
		for(int i=0;i<MAX_PIECES;i++)
		{
			currentPiece = whitePieces[i];
			if(currentPiece.moves_L!=0)
			{
				positionState=positionstate_inplay;
				return;
			}
		}
		if(getTotalTargeters(getKingPtr('w')->squarePtr,'b')==0)
		{
			positionState=positionstate_draw;
			return;
		}
		else
		{
			positionState=positionstate_blackWin;
			return;
		}
	}
	else
	{
		for(int i=0;i<MAX_PIECES;i++)
		{
			currentPiece = blackPieces[i];
			if(currentPiece.moves_L!=0)
			{
				positionState=positionstate_inplay;
				return;
			}
		}
		if(getTotalTargeters(getKingPtr('b')->squarePtr,'w')==0)
		{
			positionState=positionstate_draw;
			return;
		}
		else
		{
			positionState=positionstate_whiteWin;
			return;
		}
	}
}
void Position::resolve()
{
	//Targets
	resolve_targets('w');
	resolve_targets('b');

	//Pins
	resolve_pins('w');
	resolve_pins('b');

	//Moves
	if(colorToMove=='w' && getTotalTargeters(getKingPtr('w')->squarePtr,'b')!=0) //white to move with king in check
	{
		resolve_movesInCheck('w');
		resolve_moves('b');
	}
	else if(colorToMove=='b' && getTotalTargeters(getKingPtr('b')->squarePtr,'w')!=0) //black to move with king in check
	{
		resolve_movesInCheck('b');
		resolve_moves('w');
	}
	else
	{
		resolve_moves('w');
		resolve_moves('b');
	}
	resolve_positionState();
}
void Position::clean()
{
	for(int i=0;i<MAX_PIECES;i++)
	{
		if(whitePieces[i].type!='\0')
		{
			while(popTarget(&whitePieces[i]));
			while(popMove(&whitePieces[i]));
			memset(whitePieces[i].pinned,0,sizeof(whitePieces[i].pinned));
		}
		if(blackPieces[i].type!='\0')
		{
			while(popTarget(&blackPieces[i]));
			while(popMove(&blackPieces[i]));
			memset(blackPieces[i].pinned,0,sizeof(blackPieces[i].pinned));
		}
	}
}

//-Piece Setup-
void Position::initPiece(int8_t rank, int8_t file, char type, char color)
{
	if(theBoard[rank][file].piecePtr!=NULL)
		removePiece(theBoard[rank][file].piecePtr);
	piece* pieceBeingInitialized;
	if(color=='w')
	{
		int i=0;
		while(whitePieces[i].type!='\0')
		{
			i++;
		}
		pieceBeingInitialized = &whitePieces[i];
		whitePieces_L++;
	}
	else if(color=='b')
	{
		int i=0;
		while(blackPieces[i].type!='\0')
		{
			i++;
		}
		pieceBeingInitialized = &blackPieces[i];
		blackPieces_L++;
	}
	pieceBeingInitialized->type = type;
	pieceBeingInitialized->color = color;
	pieceBeingInitialized->rank = rank;
	pieceBeingInitialized->file = file;
	pieceBeingInitialized->squarePtr = &theBoard[rank][file];
	theBoard[rank][file].piecePtr = pieceBeingInitialized;
}

void Position::removePiece(piece* piecePtr)
{
	if(piecePtr->color=='w')
		whitePieces_L--;
	if(piecePtr->color=='b')
		blackPieces_L--;

	//remove targets
	while(popTarget(piecePtr));
	//remove targets
	while(popMove(piecePtr));
	piecePtr->rank=-1;
	piecePtr->file=-1;
	piecePtr->type='\0';
	piecePtr->color='\0';
	piecePtr->squarePtr->piecePtr=NULL;
	piecePtr->squarePtr=NULL;
}


//-Updating Targets & Moves-
bool Position::pushTarget(piece* piecePtr, square* squareTarget)
{
	if(piecePtr->targets_L==MAX_PIECE_TARGETS||squareTarget->targeters_L==MAX_SQUARE_TARGETS)
		return false;
	else
	{
		piecePtr->targets[piecePtr->targets_L] = squareTarget;
		piecePtr->targets_L++;
		squareTarget->targeters[squareTarget->targeters_L] = piecePtr;
		squareTarget->targeters_L++;
		return true;
	}
}
bool Position::pushMove(piece* piecePtr, square* squareMove)
{
	if(piecePtr->moves_L==MAX_PIECE_TARGETS||squareMove->moves_L==MAX_SQUARE_TARGETS)
		return false;
	else
	{
		piecePtr->moves[piecePtr->moves_L] = squareMove;
		piecePtr->moves_L++;
		squareMove->moves[squareMove->moves_L] = piecePtr;
		squareMove->moves_L++;
		return true;
	}
}
bool Position::popTarget(piece* piecePtr)
{
	square* tempSquarePtr;
	if(piecePtr->targets_L==0)
		return false;
	else
	{
		piecePtr->targets_L--;
		tempSquarePtr = piecePtr->targets[piecePtr->targets_L];
		piecePtr->targets[piecePtr->targets_L] = NULL;

		bool foundPiece = false;
		for(int i=0;i<MAX_SQUARE_TARGETS;i++)
		{
			if(!foundPiece)
			{
				if(piecePtr==tempSquarePtr->targeters[i])
				{
					tempSquarePtr->targeters[i]==NULL;
					foundPiece = true;
				}
			}
			else
			{
				if(tempSquarePtr->targeters[i]!=NULL)
				{
					tempSquarePtr->targeters[i-1] = tempSquarePtr->targeters[i];
					tempSquarePtr->targeters[i] = NULL;
				}
				else
				{
					break;
				}
			}
		}
		tempSquarePtr->targeters_L--;
		return true;
	}
}
bool Position::popMove(piece* piecePtr)
{
	square* tempSquarePtr;
	if(piecePtr->moves_L==0)
		return false;
	else
	{
		piecePtr->moves_L--;
		tempSquarePtr = piecePtr->moves[piecePtr->moves_L];
		piecePtr->moves[piecePtr->moves_L] = NULL;

		bool foundPiece = false;
		for(int i=0;i<MAX_SQUARE_MOVES;i++)
		{
			if(!foundPiece)
			{
				if(piecePtr==tempSquarePtr->moves[i])
				{
					tempSquarePtr->moves[i]==NULL;
					foundPiece = true;
				}
			}
			else
			{
				if(tempSquarePtr->moves[i]!=NULL)
				{
					tempSquarePtr->moves[i-1] = tempSquarePtr->moves[i];
					tempSquarePtr->moves[i] = NULL;
				}
				else
				{
					break;
				}
			}
		}
		tempSquarePtr->moves_L--;
		return true;
	}
}
void Position::removeTarget(piece* piecePtr, square* squareTarget)
{
	bool foundSquare = false;
	for(int i=0;i<MAX_PIECE_TARGETS;i++)
	{
		if(!foundSquare)
		{
			if(squareTarget==piecePtr->targets[i])
			{
				piecePtr->targets[i]==NULL;
				foundSquare = true;
			}
		}
		else
		{
			if(piecePtr->targets[i]!=NULL)
			{
				piecePtr->targets[i-1] = piecePtr->targets[i];
				piecePtr->targets[i] = NULL;
			}
			else
			{
				break;
			}
		}
	}
	piecePtr->targets_L--;

	bool foundPiece = false;
	for(int i=0;i<MAX_SQUARE_TARGETS;i++)
	{
		if(!foundPiece)
		{
			if(piecePtr==squareTarget->targeters[i])
			{
				squareTarget->targeters[i]==NULL;
				foundPiece = true;
			}
		}
		else
		{
			if(squareTarget->targeters[i]!=NULL)
			{
				squareTarget->targeters[i-1] = squareTarget->targeters[i];
				squareTarget->targeters[i] = NULL;
			}
			else
			{
				break;
			}
		}
	}
	squareTarget->targeters_L--;
}
void Position::removeMove(piece* piecePtr, square* squareMove)
{
	bool foundSquare = false;
	for(int i=0;i<MAX_PIECE_MOVES;i++)
	{
		if(!foundSquare)
		{
			if(squareMove==piecePtr->moves[i])
			{
				piecePtr->moves[i]==NULL;
				foundSquare = true;
			}
		}
		else
		{
			if(piecePtr->moves[i]!=NULL)
			{
				piecePtr->moves[i-1] = piecePtr->moves[i];
				piecePtr->moves[i] = NULL;
			}
			else
			{
				break;
			}
		}
	}
	piecePtr->moves_L--;

	bool foundPiece = false;
	for(int i=0;i<MAX_SQUARE_MOVES;i++)
	{
		if(!foundPiece)
		{
			if(piecePtr==squareMove->moves[i])
			{
				squareMove->moves[i]==NULL;
				foundPiece = true;
			}
		}
		else
		{
			if(squareMove->moves[i]!=NULL)
			{
				squareMove->moves[i-1] = squareMove->moves[i];
				squareMove->moves[i] = NULL;
			}
			else
			{
				break;
			}
		}
	}
	squareMove->moves_L--;
}

//-Castling-
bool Position::checkCanCastle(int castlingEnumVal)
{
	if(castlingEnumVal == kingside_white)
	{
		if(castlingFlags[kingside_white] &&					//castling flag is valid
		getPieceColor(&theBoard[7][4]) == 'w' &&
		getPieceColor(&theBoard[7][7]) == 'w' &&
		theBoard[7][5].piecePtr==NULL &&
		theBoard[7][6].piecePtr==NULL)
		{
			if(theBoard[7][4].piecePtr->type == 'k' &&		//king on correct square
			theBoard[7][7].piecePtr->type == 'r' &&			//rook on correct square
			getTotalTargeters(&theBoard[7][4],'b')+
			getTotalTargeters(&theBoard[7][5],'b')+
			getTotalTargeters(&theBoard[7][6],'b')==0)	//no targeters on the squares the king will pass over (or in check)
			{
				return true;
			}
		}
	}
	else if(castlingEnumVal == queenside_white)
	{
		if(castlingFlags[queenside_white] &&					//castling flag is valid
		getPieceColor(&theBoard[7][4]) == 'w' &&
		getPieceColor(&theBoard[7][0]) == 'w' &&
		theBoard[7][3].piecePtr==NULL &&
		theBoard[7][2].piecePtr==NULL &&
		theBoard[7][1].piecePtr==NULL)
		{
			if(theBoard[7][4].piecePtr->type == 'k' &&		//king on correct square
			theBoard[7][0].piecePtr->type == 'r' &&			//rook on correct square
			getTotalTargeters(&theBoard[7][4],'b')+
			getTotalTargeters(&theBoard[7][3],'b')+
			getTotalTargeters(&theBoard[7][2],'b')==0)	//no targeters on the squares the king will pass over (or in check)
			{
				return true;
			}
		}
	}
	if(castlingEnumVal == kingside_black)
	{
		if(castlingFlags[kingside_white] &&					//castling flag is valid
		getPieceColor(&theBoard[0][4]) == 'b' &&
		getPieceColor(&theBoard[0][7]) == 'b' &&
		theBoard[0][5].piecePtr==NULL &&
		theBoard[0][6].piecePtr==NULL)
		{
			if(theBoard[0][4].piecePtr->type == 'k' &&		//king on correct square
			theBoard[0][7].piecePtr->type == 'r' &&			//rook on correct square
			getTotalTargeters(&theBoard[0][4],'w')+
			getTotalTargeters(&theBoard[0][5],'w')+
			getTotalTargeters(&theBoard[0][6],'w')==0)	//no targeters on the squares the king will pass over (or in check)
			{
				return true;
			}
		}
	}
	else if(castlingEnumVal == queenside_black)
	{
		if(castlingFlags[queenside_white] &&					//castling flag is valid
		getPieceColor(&theBoard[0][4]) == 'b' &&
		getPieceColor(&theBoard[0][0]) == 'b' &&
		theBoard[0][3].piecePtr==NULL &&
		theBoard[0][2].piecePtr==NULL &&
		theBoard[0][1].piecePtr==NULL)
		{
			if(theBoard[0][4].piecePtr->type == 'k' &&		//king on correct square
			theBoard[0][0].piecePtr->type == 'r' &&			//rook on correct square
			getTotalTargeters(&theBoard[0][4],'w')+
			getTotalTargeters(&theBoard[0][3],'w')+
			getTotalTargeters(&theBoard[0][2],'w')==0)	//no targeters on the squares the king will pass over (or in check)
			{
				return true;
			}
		}
	}
	else
		return false;
	return false;
}
