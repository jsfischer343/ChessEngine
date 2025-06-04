#ifndef __GAME_HH__
#define __GAME_HH__
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include "position.hh"

class Game
{
	private:
		struct tree
		{
			//Parent
			tree* parent;
			//Data
			Position* position;
			Position::move moveMade;
			//Children
			int children_L = 0;
			tree** children;			//most positions don't have more than 60 legal moves and almost none have more than 120
		};
		typedef struct tree tree;
		/* (future implementation)
		struct moveline				//used for recording and displaying lines of moves ('lines')
		{
			moveline* parent;
			Position::move moveMade;
			moveline* child;
		}
		typedef struct line moveline;
		*/
		
		//DATA
		Position* currentPosition;
		tree* root;
		Position::move bestMove = Position::move();
		
	public:
		//--Functions--
		//-Constructors-
		Game();
		Game(const char* FENString);
		~Game();
		
		//-Actions-
		void makeMove(int8_t startRank, int8_t startFile, int8_t endFile, int8_t endRank, char endPieceType);
		//-Information-
		void printBestMove();
		
		//-Debug-
		void printBoard();
		void printAllLegalMoves();
		void generateMoveTree(int depth);
		void printMoveTree(int depth);
		
	private:
		//-Tree-
		tree* generateMoveTree(tree* rootNode, Position* startingPosition, int depth);
		tree* generateMoveTree_recursive(tree* parent, tree* node, Position* position, Position::move moveMade, int depth);
		void printMoveTree_recursive(tree* node, int depth);
		void destroyTree(tree* node);
		
		//-Calculation- 			//Note: These functions are the most computationally expensive and should only be called when needed
		Position::move calculateBestMove();
};

#endif