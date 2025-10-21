#ifndef __POSITIONTREE_HH__
#define __POSITIONTREE_HH__
#include <sys/resource.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include "position.hh"

#define PROGRAM_MEMORY_LIMIT 820*1024 //820MiB
#define MAX_DEPTH 5 //Maximum depth that the tree will be expanded to when using expandNextBestBranch()
#define PREVIOUS_NODE_QUEUE_SIZE 30 //Number of previous moves that will be saved (mostly for evaluating draw by repetition)

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
            int depth;
			double instantEval = 0; //A biased evaluation that only takes the current position into account
			double branchRecursiveAvg = 0; //The average of all this node's childrens' branchRecursiveAverages
			double branchBest = 0; //The best "response" if this move (node) was selected
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
		PositionTree(Position* startingPosition, int depth, bool computeEvals);
		~PositionTree();

    //--Tree Managment--
    private:
        //Create
        treenode* generatePositionTreeRecursive_computeEvalsNo(treenode* node, int depth);
        treenode* generatePositionTreeRecursive_computeEvalsYes(treenode* node, int depth);
        //Expand
        void expandTree(treenode* startingNode, int depth, bool computeEvals);
	public:
		void expandFromRoot(int depth, bool computeEvals);
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
		void treeMemoryOverflow();

    public:
	//--Get--
		move getBestMove();
		int8_t getGameState();
		Position* getCurrentPosition();
		bool isValidMove(move moveToBeMade);
		bool nextMoveExists();

	//--Actions--
		bool makeMove(const move moveMade);
			void makeMove_shiftTree(const move moveMade);
		void debugFunction();

    //--Debug--
		void printPositionTree();
		void printPositionTree(int depth);
	private:
			void printPositionTree_recursive(treenode* node);
			void printPositionTree_recursive(treenode* node, int depth);
};
#endif
