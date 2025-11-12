#ifndef __GAME_HH__
#define __GAME_HH__
#include <sys/resource.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include "position.hh"
#include "positiontree.hh"

#define TOTAL_PREVIOUS_POSITIONS 25	//Total previous position objects that will be stored (queue fashion), mostly for the purpose of determining draw by repetition

class Game
{
	public:
	//DATA
		Position* startingPosition;
		PositionTree* gamePositionTree;
		move bestMove;
	//FUNCTIONS
		Game();
		Game(const char* FENString);
		~Game();

		int8_t getGameState();
		Position* getCurrentPosition();
		void printBestMove();
		void calculateBestMove();
		bool makeMove(const move moveMade);
		void printBoard();
};

#endif
