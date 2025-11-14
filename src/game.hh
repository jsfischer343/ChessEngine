#ifndef __GAME_HH__
#define __GAME_HH__
#include <sys/resource.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include "global.hh"
#include "position.hh"
#include "positiontree.hh"

#define BRANCHES_TO_EXPAND 3000 //Total branches that will be expanded to determine best move in deterministic approach
#define TOTAL_PREVIOUS_POSITIONS 25	//Total previous position objects that will be stored (queue fashion), mostly for the purpose of determining draw by repetition

class Game
{
	//DATA
	public:
		PositionTree* gamePositionTree;
		move bestMove;
	//FUNCTIONS
		Game();
		Game(const char* FENString);
		~Game();

		int8_t getGameState();
		Position* getCurrentPosition();
		void printBoard();
		void printBestMove();
		void updateBestMove();
		void updateBestMoveRandom();
		bool makeMove(const move moveMade);
};

#endif
