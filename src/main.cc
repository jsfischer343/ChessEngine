#include <iostream>
#include <chrono>
#include <sys/resource.h>
#include "global.hh"
#include "position.hh"
#include "positiontree.hh"
#include "game.hh"
#include "uci.hh"
#include "cli.hh"


void printMemoryUsage()
{
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	double peakUsage = usage.ru_maxrss/1000;
	std::cout << "peak memory usage: " << peakUsage << "MB" << std::endl;
}

void printWelcomeText()
{
	printf("Bluespiral Chess Engine, Version %s, 2025\n",GLOBAL_BLUESPIRAL_VERSION);
	printf("%s\n",GLOBAL_CODE_SOURCE);
	return;
}

int main()
{
	auto startTime = std::chrono::high_resolution_clock::now();

	// //debug
	// Position* debugPosition = new Position("rnbqkbnr/ppp3pp/3p1p2/4p3/P1PP4/7P/1P2PPP1/RNBQKBNR b KQkq a3 0 4");
	// PositionTree* debugPositionTree = new PositionTree(debugPosition,4);
	// debugPositionTree->expandXNextBestBranches(5000);
	// debugPositionTree->printPositionTree(2);
	// delete debugPositionTree;
	// delete debugPosition;

	printWelcomeText();
	std::string userInput;
	CLI bluespiralCLI;
	do {
		getline(std::cin, userInput);
		bluespiralCLI.parseCommand(userInput);
	} while(userInput!="exit" && userInput!="quit");


	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration_milli = endTime-startTime;
	std::cout << "execution time: " << duration_milli.count() << " milliseconds" << std::endl;
	printMemoryUsage();
	return 0;
}
