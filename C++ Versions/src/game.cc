#include "utilities.hh"
#include "game.hh"

//-Constructors-
Game::Game()
{
	previousPositions_L = 0;
	previousPositions = new Position*[TOTAL_PREVIOUS_POSITIONS];
	currentPosition = new Position();
	game_movetree = new MoveTree(currentPosition, 2, true);
	updateBestMove();
	resolveGameState();
}
Game::Game(const char* FENString)
{
	previousPositions_L = 0;
	previousPositions = new Position*[TOTAL_PREVIOUS_POSITIONS];
	currentPosition = new Position(FENString);
	game_movetree = new MoveTree(currentPosition, 2, true);
	updateBestMove();
	resolveGameState();
}
Game::~Game()
{
	delete currentPosition;
	for(int i=0;i<previousPositions_L;i++)
	{
		delete previousPositions[i];
	}
	delete[] previousPositions;
	delete game_movetree;
}

//-Actions-
bool Game::makeMove(const move moveMade)
{
	bool validMove = false;
	//1. check that the move passed is not a "null" move and game is in-play
	if(moveMade.endPieceType=='\0')
		return false;
	if(gameState!=gamestate_inplay)
		return false;
	//2. Check "moveMade" to against tree to see if it is legal
	validMove = game_movetree->isValidMove(moveMade);
	//3. Make move if it is valid
	if(validMove)
	{
		queuePreviousPosition(currentPosition);
		currentPosition = new Position(currentPosition, moveMade);
		delete game_movetree;
		game_movetree = new MoveTree(currentPosition, 2, true);
	}
	//4. Calculate best move
	updateBestMove();
	resolveGameState();
	//5. return whether or not the move was made
	return validMove;
}

//-Debug Information-
void Game::printBestMove()
{
	if(currentPosition!=NULL)
	{
		char* moveNotation = currentPosition->getNotation(bestMove);
		printf("%s\n",moveNotation);
		delete moveNotation;
	}
}
void Game::printBoard()
{
	if(currentPosition!=NULL)
	{
		currentPosition->printBoard();
	}
	else
	{
		printf("There is no initialized current position.");
	}
}

//-Comparision
bool Game::positionIdentical(Position* position1, Position* position2) //Compares that two positions have the same pieces on the same squares (as opposed to comparing memory)
{
	if(position1->whitePieces_L!=position2->whitePieces_L || position1->blackPieces_L!=position2->blackPieces_L)
	{
		return false;
	}
	else
	{
		for(int i=0;i<8;i++)
		{
			for(int j=0;j<8;j++)
			{
				if(position1->theBoard[i][j].piecePtr!=NULL && position2->theBoard[i][j].piecePtr!=NULL)
				{}
				else if(position1->theBoard[i][j].piecePtr==NULL && position2->theBoard[i][j].piecePtr==NULL)
				{}
				else
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

//--Calculation--
void Game::updateBestMove()
{
	game_movetree->expandXNextBestBranches(300);
	bestMove = game_movetree->getBestMove();
}

//-Game State-
void Game::queuePreviousPosition(Position* previousPosition)
{
	if(previousPositions_L!=TOTAL_PREVIOUS_POSITIONS)
	{
		for(int i=previousPositions_L;i>0;i--)
		{
			previousPositions[i] = previousPositions[i-1];
		}
		previousPositions[0] = previousPosition;
		previousPositions_L++;
	}
	else
	{
		delete previousPositions[TOTAL_PREVIOUS_POSITIONS-1];
		previousPositions_L--;
		for(int i=previousPositions_L;i>0;i--)
		{
			previousPositions[i] = previousPositions[i-1];
		}
		previousPositions[0] = previousPosition;
		previousPositions_L++;
	}
}
bool Game::resolveGameState_repetition()
{
	short repetitionCount = 0;
	for(int i=0;i<previousPositions_L;i++)
	{
		if(positionIdentical(currentPosition,previousPositions[i]))
		{
			repetitionCount++;
			if(repetitionCount==2)
			{
				return true;
			}
		}
	}
	return false;
}
void Game::resolveGameState()
{
	if(currentPosition->fiftyMoveRuleCounter!=100)
	{
		if(game_movetree->nextMoveExists())
		{
			if(resolveGameState_repetition())
			{
				gameState = gamestate_drawRepetition;
				return;
			}
			else
			{
				gameState = gamestate_inplay;
				return;
			}
		}
		else
		{
			Position::piece* tempKingPtr;
			if(currentPosition->colorToMove=='w')
			{
				tempKingPtr = currentPosition->getKingPtr('w');
				if(currentPosition->getTotalTargeters(tempKingPtr->squarePtr,'b')!=0)
				{
					gameState = gamestate_blackWin;
					return;
				}
				else
				{
					gameState = gamestate_drawStalemate;
					return;
				}
			}
			else
			{
				tempKingPtr = currentPosition->getKingPtr('b');
				if(currentPosition->getTotalTargeters(tempKingPtr->squarePtr,'w')!=0)
				{
					gameState = gamestate_whiteWin;
					return;
				}
				else
				{
					gameState = gamestate_drawStalemate;
					return;
				}
			}
		}
	}
	else
	{
		gameState = gamestate_draw50;
		return;
	}
}
