#ifndef __GAME_HH__
#define __GAME_HH__
#include <sys/resource.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include "position.hh"
#include "movetree.hh"

#define TOTAL_PREVIOUS_POSITIONS 25	//Total previous position objects that will be stored (queue fashion), mostly for the purpose of determining draw by repetition

class Game
{
	public:
		//DATA
		Position* currentPosition;
		MoveTree* game_movetree;
		move bestMove = move();
		int gameState = 0;
		enum GameState
		{
			gamestate_inplay,
			gamestate_draw50,
			gamestate_drawStalemate,
			gamestate_drawRepetition,
			gamestate_whiteWin,
			gamestate_blackWin
		};
		int previousPositions_L;
		Position** previousPositions; //Used to keep a record of previous positions for eval of 'repetition' type draws

		//FUNCTIONS
		//-Constructors-
		Game();
		Game(const char* FENString);
		~Game();
		
		//-Actions-
		bool makeMove(const move moveMade);

		//-Debug Information-
		void printBestMove();
		void printBoard();
	private:
		//-Comparision
		bool positionIdentical(Position* position1, Position* position2);

		//--Calculation--
		void updateBestMove();

		//-Game State-
		void queuePreviousPosition(Position* previousPosition);
		void resolveGameState();
			bool resolveGameState_repetition();
};

#endif
