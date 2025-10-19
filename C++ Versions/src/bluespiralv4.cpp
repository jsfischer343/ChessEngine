#include <iostream>
#include <chrono>
#include <sys/resource.h>
#include "utilities.hh"
#include "position.hh"
#include "game.hh"


void printMemoryUsage()
{
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	double peakUsage = usage.ru_maxrss/1000;
	std::cout << "Peak memory usage: " << peakUsage << "MB" << std::endl;
}

void printGameResult(Game& gameObj)
{
	if(gameObj.gameState==Game::gamestate_draw50)
	{
		std::cout << "Draw by 50 rule.\n";
	}
	else if(gameObj.gameState==Game::gamestate_drawStalemate)
	{
		std::cout << "Draw by stalemate.\n";
	}
	else if(gameObj.gameState==Game::gamestate_drawRepetition)
	{
		std::cout << "Draw by repetition.\n";
	}
	else if(gameObj.gameState==Game::gamestate_whiteWin)
	{
		std::cout << "White win.\n";
	}
	else if(gameObj.gameState==Game::gamestate_blackWin)
	{
		std::cout << "Black win.\n";
	}
}

void runInteractiveGame_HumanVsHuman()
{
	bool validMove;
	char* buffer = new char[10];
	Game humanGame = Game();
	while(humanGame.gameState==0)
	{
		humanGame.printBoard();

		do
		{
			std::cout << "White to move\n";
			std::cout << ">> ";
			scanf("%s",buffer);
			if(buffer[0]=='q')
			{
				delete[] buffer;
				return;
			}
			validMove = humanGame.makeMove(humanGame.currentPosition->getMoveFromNotation(buffer));
			if(validMove==false)
				std::cout << "Invalid move\n";
		} while(validMove==false);

		humanGame.printBoard();
		if(humanGame.gameState!=0)
			break;
		do
		{
			std::cout << "Black to move\n";
			std::cout << ">> ";
			scanf("%s",buffer);
			if(buffer[0]=='q')
			{
				delete[] buffer;
				return;
			}
			validMove = humanGame.makeMove(humanGame.currentPosition->getMoveFromNotation(buffer));
			if(validMove==false)
				std::cout << "Invalid move\n";
		} while(validMove==false);
	}
	printGameResult(humanGame);
	delete[] buffer;
}

void runInteractiveGame_HumanVsBot(char humanColor)
{
	bool validMove;
	char* buffer = new char[10];
	Game botGame = Game();
	if(humanColor=='w')
	{
		while(botGame.gameState==0)
		{
			botGame.printBoard();
			do
			{
				std::cout << "White to move\n";
				std::cout << ">> ";
				scanf("%s",buffer);
				if(buffer[0]=='q' && buffer[1]=='\0')
				{
					delete[] buffer;
					return;
				}
				validMove = botGame.makeMove(botGame.currentPosition->getMoveFromNotation(buffer));
				if(validMove==false)
					std::cout << "Invalid move\n";
			} while(validMove==false);
			if(botGame.gameState!=0)
			{
				botGame.printBoard();
				break;
			}
			do
			{
				botGame.printBestMove();
				validMove = botGame.makeMove(botGame.bestMove);
				if(validMove==false)
					std::cout << "Invalid move\n";
			} while(validMove==false);
		}
	}
	else
	{
		while(botGame.gameState==0)
		{

			do
			{
				botGame.printBestMove();
				validMove = botGame.makeMove(botGame.bestMove);
				if(validMove==false)
					std::cout << "Invalid move\n";
			} while(validMove==false);
			botGame.printBoard();
			if(botGame.gameState!=0)
				break;
			do
			{
				std::cout << "Black to move\n";
				std::cout << ">> ";
				scanf("%s",buffer);
				if(buffer[0]=='q' && buffer[1]=='\0')
				{
					delete[] buffer;
					return;
				}
				validMove = botGame.makeMove(botGame.currentPosition->getMoveFromNotation(buffer));
				if(validMove==false)
					std::cout << "Invalid move\n";
			} while(validMove==false);
		}
	}
	printGameResult(botGame);
	delete[] buffer;
}


int main()
{
	//DEBUG
	//1. Start Clock
	auto startTime = std::chrono::high_resolution_clock::now();
	//2. Debug

	//debug1
	char* buffer = new char[80];
	std::cout << "FENString: ";
	std::cin.getline(buffer,80);
	while(buffer[0] != '1')
	{
		Position currentPosition = Position(buffer);
		currentPosition.printBoard();
		printf("Instant Eval: %f\n",currentPosition.instantEval());
		std::cout << "FENString: ";
		std::cin.getline(buffer,80);
	}
	delete[] buffer;

	//debug2
	//runInteractiveGame_HumanVsBot('b');

	//debug3
	// Position* currentPosition = new Position();
	// MoveTree* movetreetest = new MoveTree(currentPosition,3,true);
	// movetreetest->expandXNextBestBranches(500);
	// movetreetest->printMoveTree();
	// delete movetreetest;
	// delete currentPosition;

	//3. Stop Clock
	auto endTime = std::chrono::high_resolution_clock::now();
	//4. Print Time & Analytics
	std::chrono::duration<double, std::milli> duration_milli = endTime-startTime;
	std::cout << "Execution time: " << duration_milli.count() << " milliseconds" << std::endl;
	printMemoryUsage();
	return 0;
}
