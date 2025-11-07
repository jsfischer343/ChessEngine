#include "positiontree.hh"
#include "utilities.hh"

PositionTree::PositionTree(Position* startingPosition, int depth, bool computeEvals)
{
    this->root = new treenode();
	this->initNode = this->root;
    this->root->parent = NULL;
    this->root->position = startingPosition;
    this->root->moveMade = move();
    this->root->depth = 0;
    if(computeEvals)
    {
        this->root = generatePositionTreeRecursive_computeEvalsYes(this->root, depth);
    }
    else
    {
        this->root = generatePositionTreeRecursive_computeEvalsNo(this->root, depth);
    }
}
PositionTree::~PositionTree()
{
    destroyEntireTree();
}

//--Tree Managment--
//Create
PositionTree::treenode* PositionTree::generatePositionTreeRecursive_computeEvalsNo(treenode* node, int depth)
{
	move* currentMoveArr;
    if(exceededMemoryLimit)
    {
        return node;
    }
	else if(getMemoryUsage()>PROGRAM_MEMORY_LIMIT)
	{
        exceededMemoryLimit = true;
		treeMemoryOverflow();
		return node;
	}
	else if(depth==0)
	{
		return node;
	}
	else if(node->children_L!=0) //node already has children but specified recursive depth has not been reached yet
	{
		for(int i=0;i<node->children_L;i++)
		{
			generatePositionTreeRecursive_computeEvalsNo(node->children[i], depth-1);
		}
	}
	else
	{
		node->children_L = node->position->getTotalMoves();
	}
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
			//node->children[i]->instantEval = resultingPosition->getInstantEval();
			node->children[i]->depth = node->children[i]->parent->depth+1;
			node->children[i] = generatePositionTreeRecursive_computeEvalsNo(node->children[i], depth-1);
		}
		delete[] currentMoveArr;
		return node;
	}
	return node;
}
PositionTree::treenode* PositionTree::generatePositionTreeRecursive_computeEvalsYes(treenode* node, int depth)
{
	move* currentMoveArr;
    if(exceededMemoryLimit)
    {
        return node;
    }
	else if(getMemoryUsage()>PROGRAM_MEMORY_LIMIT)
	{
        exceededMemoryLimit = true;
		treeMemoryOverflow();
		return node;
	}
	else if(depth==0 || !(node->position->positionState==0 && node->drawByRepetition==false))
	{
		node->branchRecursiveAvg = node->instantEval; //if this is a leaf node then it is assumed that the branchAverage is the instant eval
		node->branchBest = node->instantEval; //if this is a leaf node then it is assumed that the branchBest is the instant eval
		return node;
	}
	else if(node->children_L!=0) //node already has children but specified recursive depth has not been reached yet
	{
		for(int i=0;i<node->children_L;i++)
		{
			generatePositionTreeRecursive_computeEvalsYes(node->children[i], depth-1);
		}
		refreshTreeCalculationsRecursiveUpwards(node);
		return node;
	}
	else
	{
		node->children_L = node->position->getTotalMoves();
	}
	if(node->children_L==0) //if this is true than there are no more legal moves or the game has come to a draw for this node.
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
			node->children[i]->instantEval = resultingPosition->getInstantEval();
			if(checkForRepetition(node->children[i]))
			{
				node->children[i]->drawByRepetition = true;
				node->children[i]->instantEval = 0;
			}
			node->children[i]->depth = node->children[i]->parent->depth+1;
			node->children[i] = generatePositionTreeRecursive_computeEvalsYes(node->children[i], depth-1);
			node->branchRecursiveAvg += node->children[i]->branchRecursiveAvg;
		}
		node->branchRecursiveAvg = node->branchRecursiveAvg/node->children_L;
		sortChildrenByBranchBest(node);
		node->branchBest = node->children[0]->branchBest;
		delete[] currentMoveArr;
	}
	return node;
}
//Expand
void PositionTree::expandTree(treenode* startingNode, int depth, bool computeEvals)
{
	if(computeEvals)
	{
		startingNode = generatePositionTreeRecursive_computeEvalsYes(startingNode, depth);
	}
	else
	{
		startingNode = generatePositionTreeRecursive_computeEvalsNo(startingNode, depth);
	}

}
void PositionTree::expandFromRoot(int depth, bool computeEvals)
{
	expandTree(this->root,depth,computeEvals);
}
bool PositionTree::expandNextBestBranch()
{
	treenode* nextNodeToExpand = expandNextBestBranch_findExpansionBranchRecursive(this->root);
	if(nextNodeToExpand==NULL)
	{
		return false;
	}
	expandTree(nextNodeToExpand,1,true);
	refreshTreeCalculationsRecursiveUpwards(nextNodeToExpand->parent);
	return true;
}
bool PositionTree::expandXNextBestBranches(int numberOfBranches)
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
PositionTree::treenode* PositionTree::expandNextBestBranch_findExpansionBranchRecursive(treenode* currentNode)
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
void PositionTree::refreshTreeCalculationsRecursiveUpwards(treenode* node)
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
		refreshTreeCalculationsRecursiveUpwards(node->parent);
	}
}
//Destroy
void PositionTree::destroyEntireTree_recursive(treenode* node)
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
void PositionTree::destroyEntireTree()
{
    if(initNode->children_L==0)
    {
        delete initNode;
        initNode = NULL;
    }
    else
    {
        for(int i=0;i<initNode->children_L;i++)
        {
            destroyEntireTree_recursive(initNode->children[i]);
        }
        delete[] initNode->children;
        delete initNode;
        initNode = NULL;
    }
}
void PositionTree::destroySubTree(treenode* node)
{
	if(node!=NULL && node!=root)
	{
		//1. Disconnect this node from it's parent and reorganize parent's children array (containing the node in question)
		if(node->parent->children_L-1==0) //Case: parent has only 1 child (which is the node in question)
		{
			delete[] node->parent->children;
			node->parent->children = NULL;
		}
		else //Case: parent has more than one child
		{
			treenode** new_children = new treenode*[node->parent->children_L-1];
			int i=0;
			while(i<node->parent->children_L) //copy pointers for all children expect 'node' over to new children
			{
				if(node->parent->children[i]==node)
				{
					i++;
					break;
				}
				else
				{
					new_children[i] = node->parent->children[i];
					i++;
				}
			}
			while(i<node->parent->children_L)
			{
				new_children[i-1] = node->parent->children[i];
				i++;
			}
			delete[] node->parent->children;
			node->parent->children = new_children;
		}
		node->parent->children_L--;
		//2. Run destroyEntireTree_recursive on 'node' (i.e. the child)
		destroyEntireTree_recursive(node);
	}
}

//--Utility--
void PositionTree::sortChildrenByBranchBest(treenode* node)
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
bool PositionTree::positionsIdentical(treenode* node1, treenode* node2) //Compares that two positions have the same pieces on the same squares (as opposed to comparing memory)
{
	Position* position1 = node1->position;
	Position* position2 = node2->position;
	if(position1->whitePieces_L!=position2->whitePieces_L || position1->blackPieces_L!=position2->blackPieces_L)
	{
		return false;
	}
	else
	{
		for(int i=0;i<8;i++)
		{
			for(int j=0;j<8;j++)
			{
				if(position1->theBoard[i][j].piecePtr!=NULL && position2->theBoard[i][j].piecePtr!=NULL)
				{}
				else if(position1->theBoard[i][j].piecePtr==NULL && position2->theBoard[i][j].piecePtr==NULL)
				{}
				else
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
}
bool PositionTree::checkForRepetition(treenode* node)
{
	treenode* currentNode = node;
	int repetitionCount = 0;
	while(true)
	{
		if(node->parent==NULL)
		{
			break;
		}
		if(node->parent->parent==NULL)
		{
			break;
		}
		if(positionsIdentical(node,node->parent->parent))
		{
			repetitionCount++;
			if(repetitionCount>=2)
			{
				return true;
			}
		}
		node = node->parent->parent;
	}
	return false;
}
//--Memory--
long PositionTree::getMemoryUsage()
{
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return usage.ru_maxrss;
}
void PositionTree::treeMemoryOverflow()
{
		fprintf(stderr,"Error: Tree expansion exceeded specified memory limits. Raise limits or reduce expansion.\n");
		struct rusage usage;
		getrusage(RUSAGE_SELF, &usage);
		double peakUsage = usage.ru_maxrss/1000;
		fprintf(stderr,"Peak memory usage: %.1f MB\n", peakUsage);
}

//--Get--
move PositionTree::getBestMove()
{
	if(this->root->children_L==0)
	{
		return move();
	}
	return this->root->children[0]->moveMade; //The best move is assumed to be the first move since expandNextBestBranch() and generatePositionTreeRecursive_computeEvalsYes() will both sort the tree as it generates it.
}
move PositionTree::getBestRandomMove()
{
	if(this->root->children_L==0)
	{
		return move();
	}
	return this->root->children[0]->moveMade; //The best move is assumed to be the first move since expandNextBestBranch() and generatePositionTreeRecursive_computeEvalsYes() will both sort the tree as it generates it.
}
int8_t PositionTree::getGameState()
{
	if(this->root->drawByRepetition==true)
	{
		return 6;
	}
	else
	{
		return this->root->position->positionState;
	}
}
Position* PositionTree::getCurrentPosition()
{
	return this->root->position;
}
bool PositionTree::isValidMove(move moveToBeMade)
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
bool PositionTree::nextMoveExists()
{
	if(this->root->children_L!=0)
	{
		return true;
	}
	return false;
}

//--Actions--
void PositionTree::makeMove_shiftTree(const move moveMade)
{
	//1. Delete all sub trees from root that aren't the 'moveMade'
	int i=0;
	while(this->root->children_L>1)
	{
		if(this->root->children[i]->moveMade.startRank != moveMade.startRank ||
		this->root->children[i]->moveMade.startFile != moveMade.startFile ||
		this->root->children[i]->moveMade.endRank != moveMade.endRank ||
		this->root->children[i]->moveMade.endFile != moveMade.endFile ||
		this->root->children[i]->moveMade.endPieceType != moveMade.endPieceType
		)//if the move doesn't match 'moveMade' then destroy subtree (this should just leave the node for the move that was actually made)
		{
			destroySubTree(this->root->children[i]);
		}
		else
		{
			i++;
		}
	}
	//2. Set root pointer to only remaining child of root (i.e. the node of the moveMade)
	this->root = this->root->children[0];
}
bool PositionTree::makeMove(const move moveMade)
{
	bool validMove = false;
	//1. check that the move passed is not a "null" move and game is in-play
	if(moveMade.endPieceType=='\0')
		return false;
	if(this->root->position->positionState!=Position::positionstate_inplay)
		return false;
	//2. Check "moveMade" against tree to see if it is legal
	validMove = this->isValidMove(moveMade);
	//3. Make move if it is valid
	if(validMove)
	{
		makeMove_shiftTree(moveMade);
	}
	//4. return whether or not the move was made
	return validMove;
}
void PositionTree::debugFunction()
{
	destroySubTree(this->root->children[0]);
}

//--Debug--
void PositionTree::printPositionTree_recursive(PositionTree::treenode* node)
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
			printPositionTree_recursive(node->children[i]);
		}
	}
    delete[] moveNotation;
}
void PositionTree::printPositionTree()
{
	printPositionTree_recursive(this->root);
}
void PositionTree::printPositionTree_recursive(PositionTree::treenode* node, int depth)
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
			printPositionTree_recursive(node->children[i],depth-1);
		}
	}
    delete[] moveNotation;
}
void PositionTree::printPositionTree(int depth)
{
	printPositionTree_recursive(this->root, depth);
}
