#include <iostream>
#include <chrono>
#include <sys/resource.h>
#include "global.hh"
#include "position.hh"
#include "positiontree.hh"
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
	printf("Bluespiral Chess Engine, Version %s, 2025, %s\n",GLOBAL_BLUESPIRAL_VERSION,GLOBAL_CODE_SOURCE);
	return;
}

int main()
{
	auto startTime = std::chrono::high_resolution_clock::now();

	// //debug
	// PositionTree* debugPositionTree = new PositionTree(new Position("1rb1kbnr/pppp1ppp/1qn5/3P4/8/2P2N2/PP2BPPP/RNBQK2R w KQk - 1 8"),4);
	// //debugPositionTree->expandXNextBestBranches(500);
	// debugPositionTree->printPositionTree(1);
	// delete debugPositionTree;

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
