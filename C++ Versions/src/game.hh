#ifndef __GAME_HH__
#define __GAME_HH__
#include <sys/resource.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include "position.hh"

#define TREE_MEMORY_LIMIT 3820*1024 //3820MiB in KiB
#define TOTAL_PREVIOUS_POSITIONS 25	//Total previous position objects that will be stored (queue fashion) for evaluation purposes
#define TREE_CHILDREN_MAX 4 //The max number of children that will be used when using 'reducedRecursiveExpansion' (automatically picks top 5 moves)
#define TREE_DEPTH 4 //The depth used when using 'reducedRecursiveExpansion'

class Game
{
	private:
		struct tree
		{
			//Parent
			tree* parent;
			//Data
			Position* position;
			move moveMade;
			double instantEval;
			double branchBest = 0;
			//Children
			int children_L = 0;
			tree** children;			//most positions don't have more than 60 legal moves and almost none have more than 120
		};
		typedef struct tree tree;

		//DATA (private)
		tree* root = NULL;
		
	public:
		//DATA
		Position* currentPosition;
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
		
		//-Tree- (public)
		void generateMoveTree(int depth);

		//-Debug Information-
		void printBestMove();
		void printBoard();
		void printAllLegalMoves();
			//Tree Debug

		void printMoveTree(int depth);
	private:
			void printMoveTree_recursive(tree* node, int depth);


		//-Comparision
		bool positionObjCompare(Position* position1, Position* position2);

		//-Game State-
		void queuePreviousPosition(Position* previousPosition);
		void resolveGameState();
			bool resolveGameState_repetition();

		//-Tree-
			//Create
		void generateMoveTreeFromRoot(Position* startingPosition, int depth);
			tree* generateMoveTreeFromRoot_recursive(tree* parent, tree* node, Position* position, move moveMade, int depth);
		void expandMoveTreeFromNode(tree* node, int depth);
			tree* expandMoveTreeFromNode_recursive(tree* parent, tree* node, Position* position, move moveMade, int depth);
		void reducedRecursiveExpansion(const int numberOfBranches, int depth);
			void expandBestBranches(tree* baseNode, const int numberOfBranches);
				void expandBestBranches_sort(tree** bestBranches, const int bestBranches_L);
			void reducedRecursiveExpansion_recursive(tree* node, const int numberOfBranches, int depth);
			//Destroy
		void destroyEntireTree();
			void destroyEntireTree_recursive(tree* node);
		void destroySubTree(tree* node);
			void destroySubTree_recursiveLayer1(tree* node);
			void destroySubTree_recursiveLayer2(tree* node);
			void destroySubTree_popChildFromParent(tree* parent, tree* child);
			//Misc
		void refreshBranchBest(tree* node, int depth);
		
		//-Calculation-
		void calculateBestMove();
			// ~deprecated~ void calculateBestMove_recursiveHalfExpansion(int depth);
				// ~deprecated~ void calculateBestMove_recursiveHalfExpansion_recursive(tree* node, int depth);

		//-Memory-
		long getMemoryUsage();
		bool treeWarningTriggered = false;
		void treeMemoryOverflow();
};

#endif
