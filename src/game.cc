#include "game.hh"

//-Constructors-
Game::Game()
{
	gamePositionTree = new PositionTree(new Position(),2);
}
Game::Game(const char* FENString)
{
	gamePositionTree = new PositionTree(new Position(FENString),2);
}
Game::~Game()
{
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
void Game::printBoard()
{
	gamePositionTree->getCurrentPosition()->printBoard();
}
void Game::printBestMove()
{
	char* bestMoveInStandardNotation = gamePositionTree->getCurrentPosition()->getNotation(bestMove);
	printf("%s\n",bestMoveInStandardNotation);
	delete[] bestMoveInStandardNotation;
}
void Game::updateBestMove()
{
	gamePositionTree->expandXNextBestBranches(BRANCHES_TO_EXPAND);
	bestMove = gamePositionTree->getBestMove();
}
void Game::updateBestMoveRandom()
{
	gamePositionTree->expandXNextBestBranches(BRANCHES_TO_EXPAND);
	bestMove = gamePositionTree->getBestRandomMove();
}
bool Game::makeMove(const move moveMade)
{
	bool moveMadeBool = gamePositionTree->makeMove(moveMade);
	gamePositionTree->expandFromRoot(2);
	return moveMadeBool;
}
