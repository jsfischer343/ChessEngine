#include "utilities.hh"
#include "game.hh"

//-Constructors-
Game::Game()
{
	startingPosition = new Position();
	gamePositionTree = new PositionTree(startingPosition,2,true);
	bestMove = move();
}
Game::Game(const char* FENString)
{
	startingPosition = new Position(FENString);
	gamePositionTree = new PositionTree(startingPosition,2,true);
	bestMove = move();
}
Game::~Game()
{
	delete startingPosition;
	delete gamePositionTree;
}

int8_t Game::getGameState()
{
	return gamePositionTree->getGameState();
}

Position* Game::getCurrentPosition()
{
	return gamePositionTree->getCurrentPosition();
}

void Game::printBestMove()
{
	char* bestMoveInStandardNotation = gamePositionTree->getCurrentPosition()->getNotation(bestMove);
	printf("%s\n",bestMoveInStandardNotation);
	delete[] bestMoveInStandardNotation;
}

void Game::calculateBestMove()
{
	gamePositionTree->expandXNextBestBranches(400);
	bestMove = gamePositionTree->getBestRandomMove();
}

bool Game::makeMove(const move moveMade)
{
	bool moveMadeBool = gamePositionTree->makeMove(moveMade);
	gamePositionTree->expandFromRoot(2,true);
	return moveMadeBool;
}

void Game::printBoard()
{
	gamePositionTree->getCurrentPosition()->printBoard();
}
