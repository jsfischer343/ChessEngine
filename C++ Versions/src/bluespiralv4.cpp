#include <iostream>
#include <chrono>
#include "position.hh"

int main()
{
	//DEBUG
	//1. Start Clock
	auto startTime = std::chrono::high_resolution_clock::now();
	//2. Debug
	std::cout << "---DEBUG---\n";
	//7k/6p1/3b3p/8/8/7P/1B4P1/7K w - - 0 1
	//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
	Position debugPosition = Position();
	debugPosition.printBoard();
	debugPosition.printStats();
	//3. Stop Clock
	auto endTime = std::chrono::high_resolution_clock::now();
	//4. Print Time
	std::chrono::duration<double, std::milli> duration_milli = endTime-startTime;
	std::cout << "Execution time: " << duration_milli.count() << " milliseconds" << std::endl;
	return 0;
}