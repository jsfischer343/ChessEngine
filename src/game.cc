#include "game.hh"

//-Constructors-
Game::Game()
{
	startingPosition = new Position();
	gamePositionTree = new PositionTree(startingPosition,2);
}
Game::Game(const char* FENString)
{
	startingPosition = new Position(FENString);
	gamePositionTree = new PositionTree(startingPosition,2);
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
void Game::updateBestMove()
{
	gamePositionTree->expandXNextBestBranches(5000);
	bestMove = gamePositionTree->getBestMove();
}
void Game::updateBestMoveRandom()
{
	gamePositionTree->expandXNextBestBranches(5000);
	bestMove = gamePositionTree->getBestRandomMove();
}
bool Game::makeMove(const move moveMade)
{
	bool moveMadeBool = gamePositionTree->makeMove(moveMade);
	gamePositionTree->expandFromRoot(2);
	return moveMadeBool;
}
void Game::printBoard()
{
	gamePositionTree->getCurrentPosition()->printBoard();
}
