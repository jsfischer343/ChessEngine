#include "movetree.hh"
#include "utilities.hh"

MoveTree::MoveTree(Position* startingPosition, int depth, bool computeEvals)
{
    this->root = new treenode();
    this->root->parent = NULL;
    this->root->position = startingPosition;
    this->root->moveMade = move();
    this->root->depth = 0;
    if(computeEvals)
    {
        this->root = generateMoveTreeRecursive_computeEvalsYes(this->root, depth);
    }
    else
    {
        this->root = generateMoveTreeRecursive_computeEvalsNo(this->root, depth);
    }
}
MoveTree::~MoveTree()
{
    destroyEntireTree();
}

//--Tree Managment--
//Create
MoveTree::treenode* MoveTree::generateMoveTreeRecursive_computeEvalsNo(treenode* node, int depth)
{
	move* currentMoveArr;
    if(exceededMemoryLimit)
    {
        return node;
    }
	else if(getMemoryUsage()>TREE_MEMORY_LIMIT)
	{
        exceededMemoryLimit = true;
		treeMemoryOverflow();
	}
	else if(depth==0)
	{
		return node;
	}
	else
	{
		node->children_L = node->position->getTotalMoves();
		if(node->children_L==0)
		{
			return node;
		}
		else
		{
			node->children = new treenode*[node->children_L];
			currentMoveArr = node->position->getAllMoves();
			for(int i=0;i<node->children_L;i++)
			{
                node->children[i] = new treenode();
				Position* resultingPosition = new Position(node->position, currentMoveArr[i]);
				node->children[i]->parent = node;
				node->children[i]->position = resultingPosition;
				node->children[i]->moveMade = currentMoveArr[i];
				//node->children[i]->instantEval = resultingPosition->instantEval();
                node->children[i]->depth = node->children[i]->parent->depth+1;
				node->children[i] = generateMoveTreeRecursive_computeEvalsNo(node->children[i], depth-1);
			}
			delete[] currentMoveArr;
		}
		return node;
	}
	return node;
}
MoveTree::treenode* MoveTree::generateMoveTreeRecursive_computeEvalsYes(treenode* node, int depth)
{
	move* currentMoveArr;
    if(exceededMemoryLimit)
    {
        return node;
    }
	else if(getMemoryUsage()>TREE_MEMORY_LIMIT)
	{
        exceededMemoryLimit = true;
		treeMemoryOverflow();
	}
	else if(depth==0)
	{
		node->branchRecursiveAvg = node->instantEval; //if this is a leaf node then it is assumed that the branchAverage is the instant eval
		node->branchBest = node->instantEval; //if this is a leaf node then it is assumed that the branchBest is the instant eval
		return node;
	}
	else
	{
		node->children_L = node->position->getTotalMoves();
		if(node->children_L==0)
		{
            node->branchRecursiveAvg = node->instantEval; //if this is a leaf node then it is assumed that the branchAverage is the instant eval
            node->branchBest = node->instantEval; //if this is a leaf node then it is assumed that the branchBest is the instant eval
			return node;
		}
		else
		{
			node->children = new treenode*[node->children_L];
			currentMoveArr = node->position->getAllMoves();
			for(int i=0;i<node->children_L;i++)
			{
                node->children[i] = new treenode();
				Position* resultingPosition = new Position(node->position, currentMoveArr[i]);
				node->children[i]->parent = node;
				node->children[i]->position = resultingPosition;
				node->children[i]->moveMade = currentMoveArr[i];
				node->children[i]->instantEval = resultingPosition->instantEval();
                node->children[i]->depth = node->children[i]->parent->depth+1;
				node->children[i] = generateMoveTreeRecursive_computeEvalsYes(node->children[i], depth-1);
                node->branchRecursiveAvg += (double)node->children[i]->branchRecursiveAvg;
			}
			node->branchRecursiveAvg = node->branchRecursiveAvg/node->children_L;
			sortChildrenByBranchBest(node);
			node->branchBest = node->children[0]->branchBest;
			delete[] currentMoveArr;
		}
		return node;
	}
	return node;
}
//Expand
void MoveTree::expandTree(treenode* startingNode, int depth, bool computeEvals)
{
	if(startingNode->children_L!=0)
	{
		return; //return if startingNode is not a leaf node
	}
	if(computeEvals)
	{
		startingNode = generateMoveTreeRecursive_computeEvalsYes(startingNode, depth);
	}
	else
	{
		startingNode = generateMoveTreeRecursive_computeEvalsNo(startingNode, depth);
	}

}
bool MoveTree::expandNextBestBranch()
{
	treenode* nextNodeToExpand = expandNextBestBranch_findExpansionBranchRecursive(this->root);
	if(nextNodeToExpand==NULL)
	{
		return false;
	}
	expandTree(nextNodeToExpand,1,true);
	expandNextBestBranch_refreshTreeCalculationsRecursive(nextNodeToExpand->parent);
	return true;
}
bool MoveTree::expandXNextBestBranches(int numberOfBranches)
{
	for(int i=0;i<numberOfBranches;i++)
	{
		if(expandNextBestBranch()==false)
		{
			return false;
		}
	}
	return true;
}
MoveTree::treenode* MoveTree::expandNextBestBranch_findExpansionBranchRecursive(treenode* currentNode)
{
	if(currentNode->children_L==0)
	{
		if(currentNode->depth<MAX_DEPTH)
		{
			return currentNode;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		treenode* nextNodeToExpand;
		for(int i=0;i<currentNode->children_L;i++)
		{
			nextNodeToExpand = expandNextBestBranch_findExpansionBranchRecursive(currentNode->children[i]);
			if(nextNodeToExpand!=NULL)
			{
				return nextNodeToExpand;
			}
		}
	}
	return NULL;
}
void MoveTree::expandNextBestBranch_refreshTreeCalculationsRecursive(treenode* node)
{
	if(node==NULL)
	{
		return;
	}
	else
	{
		sortChildrenByBranchBest(node);
		node->branchBest = node->children[0]->branchBest;
		node->branchRecursiveAvg = 0;
		for(int i=0;i<node->children_L;i++)
		{
			node->branchRecursiveAvg += node->children[i]->branchRecursiveAvg;
		}
		node->branchRecursiveAvg = node->branchRecursiveAvg/node->children_L;
		expandNextBestBranch_refreshTreeCalculationsRecursive(node->parent);
	}
}
//Destroy
void MoveTree::destroyEntireTree_recursive(treenode* node)
{
	if(node->children_L==0)
	{
		delete node->position;
		delete node;
	}
	else
	{
		for(int i=0;i<node->children_L;i++)
		{
			destroyEntireTree_recursive(node->children[i]);
		}
		delete[] node->children;
		delete node->position;
		delete node;
	}
}
void MoveTree::destroyEntireTree()
{
    if(root->children_L==0)
    {
        delete root;
        root = NULL;
    }
    else
    {
        for(int i=0;i<root->children_L;i++)
        {
            destroyEntireTree_recursive(root->children[i]);
        }
        delete[] root->children;
        delete root;
        root = NULL;
    }
}
void MoveTree::destroySubTree_popChildFromParent(treenode* parent, treenode* child)
{
	for(int i=0;i<parent->children_L;i++)
	{
		if(parent->children[i]==child)
		{
			parent->children[i]=NULL;
			for(int j=i+1;j<parent->children_L;j++)
			{
				if(parent->children[j]!=NULL)
				{
					parent->children[j-1]=parent->children[j];
				}
				else
				{
					break;
				}
			}
			break;
		}
	}
	parent->children_L--;
}
void MoveTree::destroySubTree_recursiveLayer1(treenode* node)
{
	if(node->children_L==0)
	{
		destroySubTree_popChildFromParent(node->parent,node);
		delete node->position;
		delete node;
	}
	else
	{
		for(int i=0;i<node->children_L;i++)
		{
			destroySubTree_recursiveLayer2(node->children[i]);
		}
		delete[] node->children;
		destroySubTree_popChildFromParent(node->parent,node);
		delete node->position;
		delete node;
	}
}
void MoveTree::destroySubTree_recursiveLayer2(treenode* node)
{
	if(node->children_L==0)
	{
		delete node->position;
		delete node;
	}
	else
	{
		for(int i=0;i<node->children_L;i++)
		{
			destroySubTree_recursiveLayer2(node->children[i]);
		}
		delete[] node->children;
		delete node->position;
		delete node;
	}
}
void MoveTree::destroySubTree(treenode* node)
{
	if(node!=NULL && node!=root)
	{
		destroySubTree_recursiveLayer1(node);
	}
}

//--Utility--
void MoveTree::sortChildrenByBranchBest(treenode* node)
{
	treenode* temp;
	if(node->position->colorToMove=='w')
	{
		for(int i=0;i<node->children_L;i++)
		{
			for(int j=i+1;j<node->children_L;j++)
			{
				if(node->children[i]->branchBest < node->children[j]->branchBest)
				{
					temp = node->children[i];
					node->children[i] = node->children[j];
					node->children[j] = temp;
				}
			}
		}
	}
	else
	{
		for(int i=0;i<node->children_L;i++)
		{
			for(int j=i+1;j<node->children_L;j++)
			{
				if(node->children[i]->branchBest > node->children[j]->branchBest)
				{
					temp = node->children[i];
					node->children[i] = node->children[j];
					node->children[j] = temp;
				}
			}
		}
	}
}
//--Memory--
long MoveTree::getMemoryUsage()
{
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return usage.ru_maxrss;
}
void MoveTree::treeMemoryOverflow()
{
		fprintf(stderr,"Error: Tree expansion exceeded specified memory limits. Raise limits or reduce expansion.\n");
		struct rusage usage;
		getrusage(RUSAGE_SELF, &usage);
		double peakUsage = usage.ru_maxrss/1000;
		fprintf(stderr,"Peak memory usage: %.1f MB\n", peakUsage);
}

//--Get--
move MoveTree::getBestMove()
{
	if(this->root->children_L==0)
	{
		return move();
	}
	return this->root->children[0]->moveMade;
}
bool MoveTree::isValidMove(move moveToBeMade)
{
	for(int i=0;i<this->root->children_L;i++)
	{
		if(this->root->children[i]->moveMade.startRank == moveToBeMade.startRank &&
		this->root->children[i]->moveMade.startFile == moveToBeMade.startFile &&
		this->root->children[i]->moveMade.endRank == moveToBeMade.endRank &&
		this->root->children[i]->moveMade.endFile == moveToBeMade.endFile &&
		this->root->children[i]->moveMade.endPieceType == moveToBeMade.endPieceType
		)
		{
			return true;
		}
	}
	return false;
}
bool MoveTree::nextMoveExists()
{
	if(this->root->children_L!=0)
	{
		return true;
	}
	return false;
}
//--Debug--
void MoveTree::printMoveTree_recursive(MoveTree::treenode* node)
{
	char* moveNotation;
	if(node->parent==NULL)
	{
		moveNotation = new char[5];
		sprintf(moveNotation,"NULL");
	}
	else
	{
		moveNotation = node->parent->position->getNotation(node->moveMade);
	}
    for(int i=0;i<node->depth;i++)
    {
        printf("\t");
    }
    printf("[%d]: %s  Instant Eval: %.3f  Branch Recursive Avg: %.3f  Branch Best: %.3f\n",node->depth,moveNotation,node->instantEval,node->branchRecursiveAvg,node->branchBest);
	if(node->children_L!=0)
	{
		for(int i=0;i<node->children_L;i++)
		{
			printMoveTree_recursive(node->children[i]);
		}
	}
    delete[] moveNotation;
}
void MoveTree::printMoveTree()
{
	printMoveTree_recursive(this->root);
}
void MoveTree::printMoveTree_recursive(MoveTree::treenode* node, int depth)
{
	char* moveNotation;
	if(node->parent==NULL)
	{
		moveNotation = new char[5];
		sprintf(moveNotation,"NULL");
	}
	else
	{
		moveNotation = node->parent->position->getNotation(node->moveMade);
	}
    for(int i=0;i<node->depth;i++)
    {
        printf("\t");
    }
    printf("[%d]: %s  Instant Eval: %.3f  Branch Recursive Avg: %.3f  Branch Best: %.3f\n",node->depth,moveNotation,node->instantEval,node->branchRecursiveAvg,node->branchBest);
	if(node->children_L!=0&&depth!=0)
	{
		for(int i=0;i<node->children_L;i++)
		{
			printMoveTree_recursive(node->children[i],depth-1);
		}
	}
    delete[] moveNotation;
}
void MoveTree::printMoveTree(int depth)
{
	printMoveTree_recursive(this->root, depth);
}
