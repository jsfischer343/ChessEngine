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
	/*Debug 1
	//7k/6p1/3b3p/8/8/7P/1B4P1/7K w - - 0 1
	//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
	//r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3
	//2br2k1/6pp/p7/2bP1pP1/2P2P2/1R2Q2P/q7/5RK1 w - - 0 35
	std::cout << "---DEBUG---\n";
	Position debugPosition = Position();
	debugPosition.printBoard();
	debugPosition.printStats();
	debugPosition.printAllMoves();
	Position::move debugMove = {6,4,4,4,'p'};
	std::cout << "---DEBUG---\n";
	Position debugPosition2 = Position(debugPosition, debugMove);
	debugPosition2.printBoard();
	debugPosition2.printStats();
	debugPosition2.printAllMoves();
	*/
	/*
	char* buffer = new char[80];
	std::cout << "FENString: ";
	std::cin.getline(buffer,80);
	while(buffer[0] != '1')
	{
		Game gameObj = Game(buffer);
		gameObj.printBoard();
		gameObj.printAllLegalMoves();
		std::cout << "FENString: ";
		std::cin.getline(buffer,80);
	}
	delete[] buffer;
	*/

	runInteractiveGame_HumanVsBot('b');

	//Game gameObj = Game("Rn1k1b1r/1pp1p3/5p1p/8/1Pb4P/3P4/1P3P2/3K3R w - - 0 26");
	//Game gameObj = Game("rR1qk1nr/1b3pp1/1p2p3/pP1pP1B1/P2P3p/3B1Q2/3N1PPP/5RK1 b - - 0 1");
	//gameObj.printBoard();
	//gameObj.makeMove(gameObj.currentPosition->getMoveFromNotation("Rae1"));
	//gameObj.printBoard();
	//gameObj.printBoard();
	//gameObj.currentPosition->printStats();
	//gameObj.printMoveTree(1);
	//gameObj.currentPosition->printAllMoves();


	// Game gameObj = Game();
	// gameObj.generateMoveTree(4);


	auto endTime = std::chrono::high_resolution_clock::now();
	//4. Print Time & Analytics
	std::chrono::duration<double, std::milli> duration_milli = endTime-startTime;
	std::cout << "Execution time: " << duration_milli.count() << " milliseconds" << std::endl;
	printMemoryUsage();
	return 0;
}
