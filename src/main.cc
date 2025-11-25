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

	//debug
	// PositionTree* debugPositionTree = new PositionTree("rnbqkbnr/ppp2ppp/4p3/3p4/8/4PN2/PPPP1PPP/RNBQKB1R w KQkq - 0 3",2);
	// std::chrono::time_point<std::chrono::steady_clock> searchStartTime = std::chrono::steady_clock::now();
	// while(true)
	// {
	// 	std::chrono::duration duration = std::chrono::steady_clock::now() - searchStartTime;
	// 	std::chrono::duration duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	// 	if(15000<duration_milliseconds.count())
	// 	{
	// 		break;
	// 	}
	// 	debugPositionTree->expandNextBestBranch();
	// }
	// debugPositionTree->printPositionTree(1);
	// debugPositionTree->printTreeInfo();
	// debugPositionTree->getCurrentPosition()->printBoard();
	// debugPositionTree->getCurrentPosition()->printInstantEvalBreakdown();
	// char* bestMoveNotation = debugPositionTree->getCurrentPosition()->getNotation(debugPositionTree->getBestMove());
	// printf("Bestmove: %s\n",bestMoveNotation);
	// delete bestMoveNotation;
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
