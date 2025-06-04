#include <iostream>
#include <chrono>
#include <sys/resource.h>
#include "position.hh"
#include "utilities.hh"
#include "game.hh"


void printMemoryUsage()
{
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	double peakUsage = usage.ru_maxrss/1000;
	std::cout << "Peak memory usage: " << peakUsage << "MB" << std::endl;
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
	auto endTime = std::chrono::high_resolution_clock::now();
	//4. Print Time & Analytics
	std::chrono::duration<double, std::milli> duration_milli = endTime-startTime;
	std::cout << "Execution time: " << duration_milli.count() << " milliseconds" << std::endl;
	printMemoryUsage();
	return 0;
}
