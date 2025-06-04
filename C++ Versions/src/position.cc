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
		setupMemory();
		//--DEEPCOPY--
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
			fiftyMoveRuleCounter = lastPosition.fiftyMoveRuleCounter+1;
		}
		//---
		
		
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
		
		//Update the board based on the 'moveMade'
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
	setupMemory();
	//--DEEPCOPY--
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
	//---
	
	
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
	
	//Update the board based on the 'castlingCode'
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
	//*removePiece(pieceBeingMoved);
	//*initPiece(moveMade.endRank,moveMade.endFile,type,color);
	resolve();
	//determine ability to castle
	canCastle[kingside_white] = checkCanCastle(kingside_white);
	canCastle[queenside_white] = checkCanCastle(queenside_white);
	canCastle[kingside_black] = checkCanCastle(kingside_black);
	canCastle[queenside_black] = checkCanCastle(queenside_black);
}
Position::Position(const Position* lastPosition, const move moveMade)
{
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
			fiftyMoveRuleCounter = lastPosition->fiftyMoveRuleCounter+1;
		}
		//---
		
		
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
		
		//Update the board based on the 'moveMade'
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
	//---
	
	
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
	
	//Update the board based on the 'castlingCode'
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
	//*removePiece(pieceBeingMoved);
	//*initPiece(moveMade.endRank,moveMade.endFile,type,color);
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

//-Debug Information-
void Position::printBoard()
{
	for(int i=0;i<8;i++)
	{
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
Position::move* Position::getAllMoves()
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
	char* buffer = new char[10];
	buffer[0] = 'N';
	buffer[1] = 'U';
	buffer[2] = 'L';
	buffer[3] = 'L';
	
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
			//TODO: check and checkmate
			pieceType = (char)std::toupper(pieceType);
			//fill buffer
			buffer[0] = pieceType;
			buffer[1] = disambig[0];
			buffer[2] = disambig[1];
			buffer[3] = capture;
			buffer[4] = destination[0];
			buffer[5] = destination[1];
			buffer[6] = promotion[0];
			buffer[7] = promotion[1];
			buffer[8] = ' ';
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
			//TODO: check and checkmate
			
			//fill buffer
			buffer[0] = pieceType;
			buffer[1] = disambig[0];
			buffer[2] = disambig[1];
			buffer[3] = capture;
			buffer[4] = destination[0];
			buffer[5] = destination[1];
			buffer[6] = promotion[0];
			buffer[7] = promotion[1];
			buffer[8] = ' ';
		}
	}
	return buffer;
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
void Position::resolve_targets_whiteKing(piece* kingPtr, int8_t rank, int8_t file)
{
	if(onBoard(rank,file))
	{
		pushTarget(kingPtr,&theBoard[rank][file]);
		//if square has no black targeters and is not occupied by the same color piece then assume the king can move there
		if(getTotalTargeters(&theBoard[rank][file],'b')==0 && getPieceColor(&theBoard[rank][file])!='w')
		{
			pushMove(kingPtr,&theBoard[rank][file]);
		}
	}
}
void Position::resolve_targets_blackKing(piece* kingPtr, int8_t rank, int8_t file)
{
	if(onBoard(rank,file))
	{
		pushTarget(kingPtr,&theBoard[rank][file]);
		//if square has no white targeters and is not occupied by the same color piece then assume the king can move there
		if(getTotalTargeters(&theBoard[rank][file],'w')==0 && getPieceColor(&theBoard[rank][file])!='b')
		{
			pushMove(kingPtr,&theBoard[rank][file]);
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
				resolve_targets_whiteKing(currentPiecePtr,rank-1,file-1);
				resolve_targets_whiteKing(currentPiecePtr,rank-1,file);
				resolve_targets_whiteKing(currentPiecePtr,rank-1,file+1);
				resolve_targets_whiteKing(currentPiecePtr,rank,file-1);
				resolve_targets_whiteKing(currentPiecePtr,rank,file+1);
				resolve_targets_whiteKing(currentPiecePtr,rank+1,file-1);
				resolve_targets_whiteKing(currentPiecePtr,rank+1,file);
				resolve_targets_whiteKing(currentPiecePtr,rank+1,file+1);
			}
			if(type=='r'||type=='q')
			{
				//scan down
				for(int i=1;i<8 && (rank+i<8);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank+i][file]);
					if(theBoard[rank+i][file].piecePtr!=NULL)
					{
						break; //if there is a piece on this square stop scanning
					}
				}
				//scan up
				for(int i=1;i<8 && (rank-i>=0);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank-i][file]);
					if(theBoard[rank-i][file].piecePtr!=NULL)
					{
						break; 
					}
				}
				//scan right
				for(int i=1;i<8 && (file+i<8);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank][file+i]);
					if(theBoard[rank][file+i].piecePtr!=NULL)
					{
						break; 
					}
				}
				//scan left
				for(int i=1;i<8 && (file-i>=0);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank][file-i]);
					if(theBoard[rank][file-i].piecePtr!=NULL)
					{
						break; 
					}
				}
			}
			if(type=='b'||type=='q')
			{
				//scan southeast
				for(int i=1;i<8 && (rank+i<8 && file+i<8);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank+i][file+i]);
					if(theBoard[rank+i][file+i].piecePtr!=NULL)
					{
						break; //if there is a piece on this square stop scanning
					}
				}
				//scan southwest
				for(int i=1;i<8 && (rank+i<8 && file-i>=0);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank+i][file-i]);
					if(theBoard[rank+i][file-i].piecePtr!=NULL)
					{
						break; 
					}
				}
				//scan northeast
				for(int i=1;i<8 && (rank-i>=0 && file+i<8);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank-i][file+i]);
					if(theBoard[rank-i][file+i].piecePtr!=NULL)
					{
						break; 
					}
				}
				//scan northwest
				for(int i=1;i<8 && (rank-i>=0 && file-i>=0);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank-i][file-i]);
					if(theBoard[rank-i][file-i].piecePtr!=NULL)
					{
						break; 
					}
				}
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
				resolve_targets_blackKing(currentPiecePtr,rank-1,file-1);
				resolve_targets_blackKing(currentPiecePtr,rank-1,file);
				resolve_targets_blackKing(currentPiecePtr,rank-1,file+1);
				resolve_targets_blackKing(currentPiecePtr,rank,file-1);
				resolve_targets_blackKing(currentPiecePtr,rank,file+1);
				resolve_targets_blackKing(currentPiecePtr,rank+1,file-1);
				resolve_targets_blackKing(currentPiecePtr,rank+1,file);
				resolve_targets_blackKing(currentPiecePtr,rank+1,file+1);
			}
			if(type=='r'||type=='q')
			{
				//scan down
				for(int i=1;i<8 && (rank+i<8);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank+i][file]);
					if(theBoard[rank+i][file].piecePtr!=NULL)
					{
						break; //if there is a piece on this square stop scanning
					}
				}
				//scan up
				for(int i=1;i<8 && (rank-i>=0);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank-i][file]);
					if(theBoard[rank-i][file].piecePtr!=NULL)
					{
						break; 
					}
				}
				//scan right
				for(int i=1;i<8 && (file+i<8);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank][file+i]);
					if(theBoard[rank][file+i].piecePtr!=NULL)
					{
						break; 
					}
				}
				//scan left
				for(int i=1;i<8 && (file-i>=0);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank][file-i]);
					if(theBoard[rank][file-i].piecePtr!=NULL)
					{
						break; 
					}
				}
			}
			if(type=='b'||type=='q')
			{
				//scan southeast
				for(int i=1;i<8 && (rank+i<8 && file+i<8);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank+i][file+i]);
					if(theBoard[rank+i][file+i].piecePtr!=NULL)
					{
						break; //if there is a piece on this square stop scanning
					}
				}
				//scan southwest
				for(int i=1;i<8 && (rank+i<8 && file-i>=0);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank+i][file-i]);
					if(theBoard[rank+i][file-i].piecePtr!=NULL)
					{
						break; 
					}
				}
				//scan northeast
				for(int i=1;i<8 && (rank-i>=0 && file+i<8);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank-i][file+i]);
					if(theBoard[rank-i][file+i].piecePtr!=NULL)
					{
						break; 
					}
				}
				//scan northwest
				for(int i=1;i<8 && (rank-i>=0 && file-i>=0);i++)
				{
					pushTarget(currentPiecePtr,&theBoard[rank-i][file-i]);
					if(theBoard[rank-i][file-i].piecePtr!=NULL)
					{
						break; 
					}
				}
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
					pushTarget(currentPiecePtr,&theBoard[rank-1][file-1]);
				}
				if(onBoard(rank+1,file+1))
				{
					pushTarget(currentPiecePtr,&theBoard[rank-1][file+1]);
				}
			}
		}
	}
}
void Position::resolve_pins(char color)
{
	piece* tempSavedPiecePtr;
	square* currentSquarePtr;
	bool pieceIsSaved = false;
	if(color=='w')
	{
		piece* whiteKingPtr = getKingPtr('w');
		int8_t whiteKR = whiteKingPtr->rank;
		int8_t whiteKF = whiteKingPtr->file;
		//scan down
		for(int i=1;i<8 && (whiteKR+i<8);i++)
		{
			currentSquarePtr = &theBoard[whiteKR+i][whiteKF];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='w')
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
					if(currentSquarePtr->piecePtr->color=='b' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan up
		for(int i=1;i<8 && (whiteKR-i>=0);i++)
		{
			currentSquarePtr = &theBoard[whiteKR-i][whiteKF];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='w')
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
					if(currentSquarePtr->piecePtr->color=='b' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan right
		for(int i=1;i<8 && (whiteKF+i<8);i++)
		{
			currentSquarePtr = &theBoard[whiteKR][whiteKF+i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='w')
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
					if(currentSquarePtr->piecePtr->color=='b' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan left
		for(int i=1;i<8 && (whiteKF-i>=0);i++)
		{
			currentSquarePtr = &theBoard[whiteKR][whiteKF-i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='w')
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
					if(currentSquarePtr->piecePtr->color=='b' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan southeast
		for(int i=1;i<8 && (whiteKR+i<8 && whiteKF+i<8);i++)
		{
			currentSquarePtr = &theBoard[whiteKR+i][whiteKF+i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='w')
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
					if(currentSquarePtr->piecePtr->color=='b' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan southwest
		for(int i=1;i<8 && (whiteKR+i<8 && whiteKF-i>=0);i++)
		{
			currentSquarePtr = &theBoard[whiteKR+i][whiteKF-i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='w')
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
					if(currentSquarePtr->piecePtr->color=='b' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan northeast
		for(int i=1;i<8 && (whiteKR-i>=0 && whiteKF+i<8);i++)
		{
			currentSquarePtr = &theBoard[whiteKR-i][whiteKF+i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='w')
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
					if(currentSquarePtr->piecePtr->color=='b' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan northwest
		for(int i=1;i<8 && (whiteKR-i>=0 && whiteKF-i>=0);i++)
		{
			currentSquarePtr = &theBoard[whiteKR-i][whiteKF-i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='w')
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
					if(currentSquarePtr->piecePtr->color=='b' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
	}
	if(color=='b')
	{
		piece* blackKingPtr = getKingPtr('w');
		int8_t blackKR = blackKingPtr->rank;
		int8_t blackKF = blackKingPtr->file;
		//scan down
		for(int i=1;i<8 && (blackKR+i<8);i++)
		{
			currentSquarePtr = &theBoard[blackKR+i][blackKF];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='b')
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
					if(currentSquarePtr->piecePtr->color=='w' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan up
		for(int i=1;i<8 && (blackKR-i>=0);i++)
		{
			currentSquarePtr = &theBoard[blackKR-i][blackKF];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='b')
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
					if(currentSquarePtr->piecePtr->color=='w' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan right
		for(int i=1;i<8 && (blackKF+i<8);i++)
		{
			currentSquarePtr = &theBoard[blackKR][blackKF+i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='b')
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
					if(currentSquarePtr->piecePtr->color=='w' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan left
		for(int i=1;i<8 && (blackKF-i>=0);i++)
		{
			currentSquarePtr = &theBoard[blackKR][blackKF-i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='b')
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
					if(currentSquarePtr->piecePtr->color=='w' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='r'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan southeast
		for(int i=1;i<8 && (blackKR+i<8 && blackKF+i<8);i++)
		{
			currentSquarePtr = &theBoard[blackKR+i][blackKF+i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='b')
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
					if(currentSquarePtr->piecePtr->color=='w' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan southwest
		for(int i=1;i<8 && (blackKR+i<8 && blackKF-i>=0);i++)
		{
			currentSquarePtr = &theBoard[blackKR+i][blackKF-i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='b')
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
					if(currentSquarePtr->piecePtr->color=='w' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan northeast
		for(int i=1;i<8 && (blackKR-i>=0 && blackKF+i<8);i++)
		{
			currentSquarePtr = &theBoard[blackKR-i][blackKF+i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='b')
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
					if(currentSquarePtr->piecePtr->color=='w' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
		//scan northwest
		for(int i=1;i<8 && (blackKR-i>=0 && blackKF-i>=0);i++)
		{
			currentSquarePtr = &theBoard[blackKR-i][blackKF-i];
			if(!pieceIsSaved)
			{
				if(currentSquarePtr->piecePtr!=NULL)
				{
					if(currentSquarePtr->piecePtr->color=='b')
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
					if(currentSquarePtr->piecePtr->color=='w' &&
					(currentSquarePtr->piecePtr->type=='q' || currentSquarePtr->piecePtr->type=='b'))
					{
						tempSavedPiecePtr->pinned[pin_file];
					}
					else
						break;
				}
			}
		}
		tempSavedPiecePtr = NULL;
		pieceIsSaved = false;
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
			if(type=='r'||type=='q')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan down
					for(int i=1;i<8 && (rank+i<8);i++)
					{
						if(getPieceColor(&theBoard[rank+i][file])!='w')
							pushMove(currentPiecePtr,&theBoard[rank+i][file]);
						if(theBoard[rank+i][file].piecePtr!=NULL)
						{
							break; //if there is a piece on this square stop scanning
						}
					}
					//scan up
					for(int i=1;i<8 && (rank-i>=0);i++)
					{
						if(getPieceColor(&theBoard[rank-i][file])!='w')
							pushMove(currentPiecePtr,&theBoard[rank-i][file]);
						if(theBoard[rank-i][file].piecePtr!=NULL)
						{
							break; 
						}
					}
				}
				if(!currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan right
					for(int i=1;i<8 && (file+i<8);i++)
					{
						if(getPieceColor(&theBoard[rank][file+i])!='w')
							pushMove(currentPiecePtr,&theBoard[rank][file+i]);
						if(theBoard[rank][file+i].piecePtr!=NULL)
						{
							break; 
						}
					}
					//scan left
					for(int i=1;i<8 && (file-i>=0);i++)
					{
						if(getPieceColor(&theBoard[rank][file-i])!='w')
							pushMove(currentPiecePtr,&theBoard[rank][file-i]);
						if(theBoard[rank][file-i].piecePtr!=NULL)
						{
							break; 
						}
					}
				}
			}
			if(type=='b'||type=='q')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan southeast
					for(int i=1;i<8 && (rank+i<8 && file+i<8);i++)
					{
						if(getPieceColor(&theBoard[rank+i][file+i])!='w')
							pushMove(currentPiecePtr,&theBoard[rank+i][file+i]);
						if(theBoard[rank+i][file+i].piecePtr!=NULL)
						{
							break; //if there is a piece on this square stop scanning
						}
					}
					//scan northwest
					for(int i=1;i<8 && (rank-i>=0 && file-i>=0);i++)
					{
						if(getPieceColor(&theBoard[rank-i][file-i])!='w')
							pushMove(currentPiecePtr,&theBoard[rank-i][file-i]);
						if(theBoard[rank-i][file-i].piecePtr!=NULL)
						{
							break; 
						}
					}
				}
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					//scan southwest
					for(int i=1;i<8 && (rank+i<8 && file-i>=0);i++)
					{
						if(getPieceColor(&theBoard[rank+i][file-i])!='w')
							pushMove(currentPiecePtr,&theBoard[rank+i][file-i]);
						if(theBoard[rank+i][file-i].piecePtr!=NULL)
						{
							break; 
						}
					}
					//scan northeast
					for(int i=1;i<8 && (rank-i>=0 && file+i<8);i++)
					{
						if(getPieceColor(&theBoard[rank-i][file+i])!='w')
							pushMove(currentPiecePtr,&theBoard[rank-i][file+i]);
						if(theBoard[rank-i][file+i].piecePtr!=NULL)
						{
							break; 
						}
					}
				}
			}
			if(type=='n')
			{
				//add moves for knight
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					if(onBoard(rank-2,file-1))
					{
						if(getPieceColor(&theBoard[rank-2][file-1])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank-2][file-1]);
						}
					}
					if(onBoard(rank-1,file-2))
					{
						if(getPieceColor(&theBoard[rank-1][file-2])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank-1][file-2]);
						}
					}
					if(onBoard(rank-2,file+1))
					{
						if(getPieceColor(&theBoard[rank-2][file+1])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank-2][file+1]);
						}
					}
					if(onBoard(rank-1,file+2))
					{
						if(getPieceColor(&theBoard[rank-1][file+2])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank-1][file+2]);
						}
					}
					if(onBoard(rank+2,file-1))
					{
						if(getPieceColor(&theBoard[rank+2][file-1])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank+2][file-1]);
						}
					}
					if(onBoard(rank+1,file-2))
					{
						if(getPieceColor(&theBoard[rank+1][file-2])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank+1][file-2]);
						}
					}
					if(onBoard(rank+2,file+1))
					{
						if(getPieceColor(&theBoard[rank+2][file+1])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank+2][file+1]);
						}
					}
					if(onBoard(rank+1,file+2))
					{
						if(getPieceColor(&theBoard[rank+1][file+2])!='w')
						{
							pushMove(currentPiecePtr,&theBoard[rank+1][file+2]);
						}
					}
				}
			}
			if(type=='p')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
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
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
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
			if(type=='r'||type=='q')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan down
					for(int i=1;i<8 && (rank+i<8);i++)
					{
						if(getPieceColor(&theBoard[rank+i][file])!='b')
							pushMove(currentPiecePtr,&theBoard[rank+i][file]);
						if(theBoard[rank+i][file].piecePtr!=NULL)
						{
							break; //if there is a piece on this square stop scanning
						}
					}
					//scan up
					for(int i=1;i<8 && (rank-i>=0);i++)
					{
						if(getPieceColor(&theBoard[rank-i][file])!='b')
							pushMove(currentPiecePtr,&theBoard[rank-i][file]);
						if(theBoard[rank-i][file].piecePtr!=NULL)
						{
							break; 
						}
					}
				}
				if(!currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan right
					for(int i=1;i<8 && (file+i<8);i++)
					{
						if(getPieceColor(&theBoard[rank][file+i])!='b')
							pushMove(currentPiecePtr,&theBoard[rank][file+i]);
						if(theBoard[rank][file+i].piecePtr!=NULL)
						{
							break; 
						}
					}
					//scan left
					for(int i=1;i<8 && (file-i>=0);i++)
					{
						if(getPieceColor(&theBoard[rank][file-i])!='b')
							pushMove(currentPiecePtr,&theBoard[rank][file-i]);
						if(theBoard[rank][file-i].piecePtr!=NULL)
						{
							break; 
						}
					}
				}
			}
			if(type=='b'||type=='q')
			{
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag])
				{
					//scan southeast
					for(int i=1;i<8 && (rank+i<8 && file+i<8);i++)
					{
						if(getPieceColor(&theBoard[rank+i][file+i])!='b')
							pushMove(currentPiecePtr,&theBoard[rank+i][file+i]);
						if(theBoard[rank+i][file+i].piecePtr!=NULL)
						{
							break; //if there is a piece on this square stop scanning
						}
					}
					//scan northwest
					for(int i=1;i<8 && (rank-i>=0 && file-i>=0);i++)
					{
						if(getPieceColor(&theBoard[rank-i][file-i])!='b')
							pushMove(currentPiecePtr,&theBoard[rank-i][file-i]);
						if(theBoard[rank-i][file-i].piecePtr!=NULL)
						{
							break; 
						}
					}
				}
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					//scan southwest
					for(int i=1;i<8 && (rank+i<8 && file-i>=0);i++)
					{
						if(getPieceColor(&theBoard[rank+i][file-i])!='b')
							pushMove(currentPiecePtr,&theBoard[rank+i][file-i]);
						if(theBoard[rank+i][file-i].piecePtr!=NULL)
						{
							break; 
						}
					}
					//scan northeast
					for(int i=1;i<8 && (rank-i>=0 && file+i<8);i++)
					{
						if(getPieceColor(&theBoard[rank-i][file+i])!='b')
							pushMove(currentPiecePtr,&theBoard[rank-i][file+i]);
						if(theBoard[rank-i][file+i].piecePtr!=NULL)
						{
							break; 
						}
					}
				}
			}
			if(type=='n')
			{
				//add moves for knight
				if(!currentPiecePtr->pinned[pin_rank] && !currentPiecePtr->pinned[pin_file] && !currentPiecePtr->pinned[pin_positiveDiag] && !currentPiecePtr->pinned[pin_negativeDiag])
				{
					if(onBoard(rank-2,file-1))
					{
						if(getPieceColor(&theBoard[rank-2][file-1])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank-2][file-1]);
						}
					}
					if(onBoard(rank-1,file-2))
					{
						if(getPieceColor(&theBoard[rank-1][file-2])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank-1][file-2]);
						}
					}
					if(onBoard(rank-2,file+1))
					{
						if(getPieceColor(&theBoard[rank-2][file+1])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank-2][file+1]);
						}
					}
					if(onBoard(rank-1,file+2))
					{
						if(getPieceColor(&theBoard[rank-1][file+2])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank-1][file+2]);
						}
					}
					if(onBoard(rank+2,file-1))
					{
						if(getPieceColor(&theBoard[rank+2][file-1])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank+2][file-1]);
						}
					}
					if(onBoard(rank+1,file-2))
					{
						if(getPieceColor(&theBoard[rank+1][file-2])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank+1][file-2]);
						}
					}
					if(onBoard(rank+2,file+1))
					{
						if(getPieceColor(&theBoard[rank+2][file+1])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank+2][file+1]);
						}
					}
					if(onBoard(rank+1,file+2))
					{
						if(getPieceColor(&theBoard[rank+1][file+2])!='b')
						{
							pushMove(currentPiecePtr,&theBoard[rank+1][file+2]);
						}
					}
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
void Position::resolve()
{
	resolve_targets('w');
	resolve_targets('b');
	resolve_pins('w');
	resolve_pins('b');
	resolve_moves('w');
	resolve_moves('b');
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

//-Utility-
bool Position::onBoard(int8_t rank, int8_t file)
{
	if(rank>=0 && rank<8 && file>=0 && file<8)
		return true;
	return false;
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

char Position::getPieceColor(square* squarePtr)
{
	if(squarePtr->piecePtr==NULL)
		return '\0';
	else
		return squarePtr->piecePtr->color;
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
