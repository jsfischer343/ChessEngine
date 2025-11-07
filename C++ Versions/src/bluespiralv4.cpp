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
	Game botGame = Game();
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
	Position* debugPosition = new Position("r1bqkb1r/pp3ppp/2B2n2/2pp4/8/2N1P1P1/PPPP3P/1RBQK1NR b Kkq - 0 9");
	debugPosition->printBoard();
	debugPosition->printStats();
	debugPosition->printInstantEvalBreakdown();
 //
	// move debugMove = move();
	// debugMove.startRank=3;
	// debugMove.startFile=5;
	// debugMove.endRank=2;
	// debugMove.endFile=4;
	// debugMove.endPieceType='p';
	// Position* debugPosition2 = new Position(debugPosition,debugMove);
	// debugPosition2->printAllMoves();
	// debugPosition2->printBoard();
 //
	// delete debugPosition;
	// delete debugPosition2;
	//Game testGame = Game("r1bqkb1r/ppp2pp1/2n4p/3p4/4n2P/2N1P3/PPPPQPP1/1RB1KB1R");
	//testGame.startingPosition->printAllMoves();

	//3. Stop Clock
	auto endTime = std::chrono::high_resolution_clock::now();
	//4. Print Time & Analytics
	std::chrono::duration<double, std::milli> duration_milli = endTime-startTime;
	std::cout << "Execution time: " << duration_milli.count() << " milliseconds" << std::endl;
	printMemoryUsage();
	return 0;
}
