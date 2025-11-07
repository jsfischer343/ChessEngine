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

void printGameResult(int8_t gameState)
{
	switch(gameState)
	{
		case 0:
			printf("Error 22\n");
			break;
		case 1:
			printf("Draw by 50 move rule\n");
			break;
		case 2:
			printf("Draw by stalemate.\n");
			break;
		case 3:
			printf("Draw by insufficient material.\n");
			break;
		case 4:
			printf("White win.\n");
			break;
		case 5:
			printf("Black win.\n");
			break;
		case 6:
			printf("Draw by threefold repetition.\n");
			break;
		default:
			printf("Error 36\n");
			break;
	}
}

void runInteractiveGame_HumanVsBot(char humanColor)
{
	bool validMove;
	char* buffer = new char[10];
	Game botGame = Game("r1b1kb1r/pp4pp/2n2p2/8/8/1R2PpB1/n1PK1PPP/5BR1 w - - 0 18");
	if(humanColor=='w')
	{
		while(botGame.getGameState()==0)
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
				validMove = botGame.makeMove(botGame.getCurrentPosition()->getMoveFromNotation(buffer));
				if(validMove==false)
					std::cout << "Invalid move\n";
			} while(validMove==false);
			if(botGame.getGameState()!=0)
			{
				botGame.printBoard();
				break;
			}
			do
			{
				botGame.calculateBestMove();
				botGame.printBestMove();
				validMove = botGame.makeMove(botGame.bestMove);
				if(validMove==false)
					std::cout << "Invalid move\n";
			} while(validMove==false);
		}
	}
	else
	{
		while(botGame.getGameState()==0)
		{
			do
			{
				botGame.calculateBestMove();
				botGame.printBestMove();
				validMove = botGame.makeMove(botGame.bestMove);
				if(validMove==false)
					std::cout << "Invalid move\n";
			} while(validMove==false);
			botGame.printBoard();
			if(botGame.getGameState()!=0)
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
				validMove = botGame.makeMove(botGame.getCurrentPosition()->getMoveFromNotation(buffer));
				if(validMove==false)
					std::cout << "Invalid move\n";
			} while(validMove==false);
		}
	}
	printGameResult(botGame.getGameState());
	delete[] buffer;
}

int main()
{
	//DEBUG
	//1. Start Clock
	auto startTime = std::chrono::high_resolution_clock::now();
	//2. Debug
	//r1bqkb1r/pp3ppp/2B2n2/2pp4/8/2N1P1P1/PPPP3P/1RBQK1NR b Kkq - 0 9
 //
	// Position* debugPosition = new Position("rnbqkb1r/pp2pppp/5n2/1NP5/3p4/8/PPP1PPPP/1RBQKBNR b Kkq - 1 5");
	// PositionTree* debugPositionTree = new PositionTree(debugPosition,2,true);
	// debugPositionTree->expandXNextBestBranches(200);
	// debugPositionTree->printPositionTree(2);
	// delete debugPositionTree;
	// delete debugPosition;

	// Position* debugPosition = new Position("rnbqkb1r/pp2pppp/5n2/1NP5/3p4/8/PPP1PPPP/1RBQKBNR b Kkq - 1 5");
	// debugPosition->printBoard();
	// debugPosition->printInfo();
	// debugPosition->printInstantEvalBreakdown();
	// delete debugPosition;

	runInteractiveGame_HumanVsBot('b');

	//3. Stop Clock
	auto endTime = std::chrono::high_resolution_clock::now();
	//4. Print Time & Analytics
	std::chrono::duration<double, std::milli> duration_milli = endTime-startTime;
	std::cout << "Execution time: " << duration_milli.count() << " milliseconds" << std::endl;
	printMemoryUsage();
	return 0;
}
