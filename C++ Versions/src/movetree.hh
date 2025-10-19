#ifndef __MOVETREE_HH__
#define __MOVETREE_HH__
#include <sys/resource.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include "position.hh"

#define TREE_MEMORY_LIMIT 820*1024 //820MiB
#define MAX_DEPTH 5

class MoveTree
{
	private:
		struct treenode
		{
			//Parent
			treenode* parent;
			//Data
			Position* position;
			move moveMade;
            int depth;
			double instantEval = 0;
			double branchRecursiveAvg = 0; //The average of all this node's childrens' branchRecursiveAverages
			double branchBest = 0; //The best "response" if this move (node) was selected
			//Children
			int children_L = 0;
			treenode** children; //random note: most positions don't have more than 60 legal moves and almost none have more than 120
		};
		typedef struct treenode treenode;

        //Data
		treenode* root;
        int currentMaxDepth = 0;

        //Flags
        bool exceededMemoryLimit = false;

    public:
		MoveTree(Position* startingPosition, int depth, bool computeEvals);
		~MoveTree();

    //--Tree Managment--
    private:
        //Create
        treenode* generateMoveTreeRecursive_computeEvalsNo(treenode* node, int depth);
        treenode* generateMoveTreeRecursive_computeEvalsYes(treenode* node, int depth);
        //Expand
        void expandTree(treenode* startingNode, int depth, bool computeEvals);
	public:
		bool expandNextBestBranch();
		bool expandXNextBestBranches(int numberOfBranches); //run expandNextBestBranch X number of times
	private:
			treenode* expandNextBestBranch_findExpansionBranchRecursive(treenode* currentNode);
			void expandNextBestBranch_refreshTreeCalculationsRecursive(treenode* node);
        //Destroy
		void destroyEntireTree();
			void destroyEntireTree_recursive(treenode* node);
		void destroySubTree(treenode* node);
			void destroySubTree_recursiveLayer1(treenode* node);
			void destroySubTree_recursiveLayer2(treenode* node);
			void destroySubTree_popChildFromParent(treenode* parent, treenode* child);

	//--Utility--
		void sortChildrenByBranchBest(treenode* node);

    //--Memory--
		long getMemoryUsage();
		void treeMemoryOverflow();

    public:
	//--Get--
		move getBestMove();
		bool isValidMove(move moveToBeMade);
		bool nextMoveExists();

    //--Debug--
		void printMoveTree();
		void printMoveTree(int depth);
	private:
			void printMoveTree_recursive(treenode* node);
			void printMoveTree_recursive(treenode* node, int depth);
};
#endif
