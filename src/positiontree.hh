#ifndef __POSITIONTREE_HH__
#define __POSITIONTREE_HH__
#include <sys/resource.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include <ctime>
#include <vector>
#include "global.hh"
#include "move.hh"
#include "position.hh"

#define POSITIONTREE_MEMORY_LIMIT 820*1024 //820MiB
#define POSITION_OBJ_EPHEMERAL_DEPTH 3 //nodes at this depth will no longer save their position obj and will instead only instantiate it when needed (purpose being to save memory as position objects take up excessive memory)
#define MAX_DEPTH 6 //Maximum depth that the tree will be expanded to when using expandNextBestBranch()
#define EVALUATION_EQUIVALENCY_THRESHOLD 0.02 //The differential threshold that is used to determine if two moves have 'essentially' equal evaluation


class PositionTree
{
	private:
		//STRUCTS
		struct treenode
		{
			//-Parent-
			treenode* parent;
			//-Data-
			Position* position;
			move moveMade;
			char colorToMove;
            int depth;
			float instantEval = 0; //A biased evaluation that only takes the current position into account
			float branchRecursiveAvg = 0; //The average of all this node's childrens' branchRecursiveAverages (recursive metric)
			float branchBest = 0; //The best "response" if this move (node) was selected (recursive metric)
			bool drawByRepetition = false; //Since draw by repetition is special in that you need contextual information to determine it, this must be determined for each node in relation to the tree rather than as a property of the 'position' object.
			//-Children-
			int children_L = 0;
			treenode** children; //random note: most positions don't have more than 60 legal moves and almost none have more than 120
		};
		typedef struct treenode treenode;

        //DATA
		treenode* initNode; //Initial node which is the true root of the tree
		treenode* root; //The root of the tree which acts at the 'current position'

        //FLAGS
        bool exceededMemoryLimit = false;

    public:
		//FUNCTIONS
		PositionTree(int depth);
		PositionTree(const char* startingPositionFEN, int depth);
		PositionTree(Position* startingPosition, int depth);
		~PositionTree();

    //--Tree Managment--
    private:
        //Create
        treenode* generatePositionTreeRecursive(treenode* node, int depth);
			void generatePositionTreeRecursive_reinstantiatePositionObjsRecursiveUpwards(treenode* node);
			void generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(treenode* node);
        //Expand
        void expandTree(treenode* startingNode, int depth);
		void expandFromRoot(int depth);
	public:
		bool expandNextBestBranch();
		bool expandXNextBestBranches(int numberOfBranches); //run expandNextBestBranch X number of times
	private:
			treenode* expandNextBestBranch_findExpansionBranchRecursive(treenode* currentNode);
		//Refresh
		void refreshTreeCalculationsRecursiveUpwards(treenode* node);
        //Destroy
		void destroyEntireTree();
			void destroyEntireTree_recursive(treenode* node);
		void destroySubTree(treenode* node);

	//--Utility--
		void sortChildrenByBranchBest(treenode* node);
		bool positionsIdentical(treenode* node1, treenode* node2);
		bool checkForRepetition(treenode* node);

    //--Memory--
		long getMemoryUsage();
		void warnTreeMemoryOverflow();

    public:
	//--Get--
		//Moves
		move getBestMove();
		move getBestRandomMove(); //if there are multiple moves with less than EVALUATION_EQUIVALENCY_THRESHOLD of difference then randomly pick one
		bool nextMoveExists();
		bool isValidMove(move moveToBeMade);
		//Game
		int8_t getGameState();
		Position* getCurrentPosition();

	//--Actions--
		bool makeMove(const move moveMade);
	private:
			void makeMove_shiftTree(const move moveMade);

	public:
    //--Debug--
		void printPositionTree();
		void printPositionTree(int depth);
	private:
			void printPositionTree_recursive(treenode* node);
			void printPositionTree_recursive(treenode* node, int depth);
};
#endif
