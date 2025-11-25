#include "positiontree.hh"

PositionTree::PositionTree(int depth)
{
    root = new treenode();
	this->initNode = root;
    root->parent = NULL;
	Position* startingPosition = new Position();
    root->position = startingPosition;
	root->colorToMove = startingPosition->colorToMove;
    root->moveMade = move();
    root->depth = 0;
	root = generatePositionTreeRecursive(root, depth);
}
PositionTree::PositionTree(const char* startingPositionFEN, int depth)
{
    root = new treenode();
	this->initNode = root;
    root->parent = NULL;
	Position* startingPosition = new Position(startingPositionFEN);
    root->position = startingPosition;
	root->colorToMove = startingPosition->colorToMove;
    root->moveMade = move();
    root->depth = 0;
	root = generatePositionTreeRecursive(root, depth);
}
PositionTree::PositionTree(Position* startingPosition, int depth)
{
    root = new treenode();
	this->initNode = root;
    root->parent = NULL;
    root->position = startingPosition;
    root->moveMade = move();
    root->depth = 0;
	root = generatePositionTreeRecursive(root, depth);
}
PositionTree::~PositionTree()
{
    destroyEntireTree();
}

//--Tree Managment--
//Create
void PositionTree::generatePositionTreeRecursive_reinstantiatePositionObjsRecursiveUpwards(treenode* node)
{
	if(node->position!=NULL)
	{
		return;
	}
	else
	{
		generatePositionTreeRecursive_reinstantiatePositionObjsRecursiveUpwards(node->parent);
		node->position = new Position(node->parent->position,node->moveMade);
	}
}
void PositionTree::generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(treenode* node)
{
	if(node->depth<(POSITION_OBJ_EPHEMERAL_DEPTH+root->depth))
	{
		return;
	}
	else
	{
		delete node->position;
		node->position = NULL;
		generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(node->parent);
	}
}
PositionTree::treenode* PositionTree::generatePositionTreeRecursive(treenode* node, int depth)
{
	move* currentMoveArr;
	bool currentNodePositionObjIsEphemeral = false;

	if(node->position==NULL)
	{
		currentNodePositionObjIsEphemeral = true;
		generatePositionTreeRecursive_reinstantiatePositionObjsRecursiveUpwards(node);
	}
    if(exceededMemoryLimit)
    {
		node->colorToMove = node->position->colorToMove;
		node->branchRecursiveAvg = node->instantEval; //if this is a leaf node then it is assumed that the branchAverage is the instant eval
		node->branchBest = node->instantEval; //if this is a leaf node then it is assumed that the branchBest is the instant eval
		if(currentNodePositionObjIsEphemeral)
		{
			generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(node);
		}
        return node;
    }
	else if(getMemoryUsage()>POSITIONTREE_MEMORY_LIMIT)
	{
        exceededMemoryLimit = true;
		warnTreeMemoryOverflow();
		node->colorToMove = node->position->colorToMove;
		node->branchRecursiveAvg = node->instantEval;
		node->branchBest = node->instantEval;
		if(currentNodePositionObjIsEphemeral)
		{
			generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(node);
		}
		return node;
	}
	else if(depth==0 || !(node->position->positionState==0 && node->drawByRepetition==false))
	{
		node->colorToMove = node->position->colorToMove;
		node->branchRecursiveAvg = node->instantEval;
		node->branchBest = node->instantEval;
		if(currentNodePositionObjIsEphemeral)
		{
			generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(node);
		}
		return node;
	}
	else if(node->children_L!=0) //node already has children but specified recursive depth has not been reached yet
	{
		for(int i=0;i<node->children_L;i++)
		{
			generatePositionTreeRecursive(node->children[i], depth-1);
		}
		node->colorToMove = node->position->colorToMove;
		node->branchRecursiveAvg = node->instantEval;
		node->branchBest = node->instantEval;
		refreshTreeCalculationsRecursiveUpwards(node);
		if(currentNodePositionObjIsEphemeral)
		{
			generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(node);
		}
		return node;
	}
	else
	{
		node->children_L = node->position->getTotalMoves();
	}
	if(node->children_L==0) //if this is true than there are no more legal moves or the game has come to a draw for this node.
	{
		node->colorToMove = node->position->colorToMove;
		node->branchRecursiveAvg = node->instantEval;
		node->branchBest = node->instantEval;
		if(currentNodePositionObjIsEphemeral)
		{
			generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(node);
		}
		return node;
	}
	else
	{
		node->children = new treenode*[node->children_L];
		currentMoveArr = node->position->getAllMoves();
		for(int i=0;i<node->children_L;i++)
		{
			treeInfo.totalNodes++;
			node->children[i] = new treenode();
			Position* resultingPosition = new Position(node->position, currentMoveArr[i]);
			node->children[i]->parent = node;
			node->children[i]->position = resultingPosition;
			node->children[i]->colorToMove = resultingPosition->colorToMove;
			node->children[i]->moveMade = currentMoveArr[i];
			if(checkForRepetition(node->children[i]))
			{
				node->children[i]->drawByRepetition = true;
				node->children[i]->instantEval = 0;
			}
			else
			{
				node->children[i]->instantEval = resultingPosition->getInstantEval();
			}
			node->children[i]->depth = node->children[i]->parent->depth+1;
			node->children[i] = generatePositionTreeRecursive(node->children[i], depth-1);
			if(node->children[i]->depth>=(POSITION_OBJ_EPHEMERAL_DEPTH+root->depth))
			{
				delete node->children[i]->position;
				node->children[i]->position = NULL;
			}
			node->branchRecursiveAvg += node->children[i]->branchRecursiveAvg;
		}
		node->branchRecursiveAvg = node->branchRecursiveAvg/node->children_L;
		sortChildrenByBranchBest(node);
		node->branchBest = node->children[0]->branchBest;
		delete[] currentMoveArr;
	}
	if(currentNodePositionObjIsEphemeral)
	{
		generatePositionTreeRecursive_destroyPositionObjsRecursiveUpwards(node);
	}
	return node;
}
//Expand
void PositionTree::expandTree(treenode* startingNode, int depth)
{
	generatePositionTreeRecursive(startingNode, depth);
}
void PositionTree::expandFromRoot(int depth)
{
	expandTree(root,depth);
}
bool PositionTree::expandNextBestBranch()
{
	treenode* nextNodeToExpand = expandNextBestBranch_findExpansionBranchRecursive(root);
	if(nextNodeToExpand==NULL)
	{
		return false;
	}
	expandTree(nextNodeToExpand,1);
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
		if(currentNode->depth<(MAX_DEPTH+root->depth))
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
		node = NULL;
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
		node = NULL;
	}
}
void PositionTree::destroyEntireTree()
{
    if(initNode->children_L==0)
    {
		delete initNode->position;
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
		delete initNode->position;
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
	if(node->colorToMove=='w')
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
	else if(node->colorToMove=='b')
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
	else
	{
		throw;
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
	treenode* currentNode = node->parent;
	int repetitionCount = 0;
	while(true)
	{
		if(currentNode==NULL)
		{
			break;
		}
		if(positionsIdentical(node,currentNode))
		{
			repetitionCount++;
			if(repetitionCount>=2)
			{
				return true;
			}
		}
		currentNode = currentNode->parent;
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
void PositionTree::warnTreeMemoryOverflow()
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
	if(root->children_L==0)
	{
		return move();
	}
	return root->children[0]->moveMade; //The best move is assumed to be the first move since expandNextBestBranch() and generatePositionTreeRecursive_computeEvalsYes() will both sort the tree as it generates it.
}
move PositionTree::getBestRandomMove()
{
	if(root->children_L==0)
	{
		return move();
	}
	int i=1;
	//The best move is assumed to be the first move since expandNextBestBranch() and generatePositionTreeRecursive_computeEvalsYes() will both sort the tree as it generates it.
	float topMoveBranchBest = root->children[0]->branchBest;
	int root_children_L = root->children_L;
	if(root->colorToMove=='w')
	{
		topMoveBranchBest-=EVALUATION_EQUIVALENCY_THRESHOLD;
		while(i<root_children_L)
		{
			if(root->children[i]->branchBest<=topMoveBranchBest)
				break;
			i++; //increment while the next move is similar enough in evaluation to the best move
		}
	}
	else
	{
		topMoveBranchBest+=EVALUATION_EQUIVALENCY_THRESHOLD;
		while(i<root_children_L)
		{
			if(root->children[i]->branchBest>=topMoveBranchBest)
				break;
			i++;
		}
	}
	srand(time(0));
	int randomIndex = rand()%i; //selects a random index from the range given by i
	return root->children[randomIndex]->moveMade;
}
int8_t PositionTree::getGameState()
{
	if(root->drawByRepetition==true)
	{
		return 6;
	}
	else
	{
		return root->position->positionState;
	}
}
Position* PositionTree::getCurrentPosition()
{
	return root->position;
}
bool PositionTree::isValidMove(move moveToBeMade)
{
	for(int i=0;i<root->children_L;i++)
	{
		if(move::movesEqual(root->children[i]->moveMade,moveToBeMade))
		{
			return true;
		}
	}
	return false;
}
bool PositionTree::nextMoveExists()
{
	if(root->children_L!=0)
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
	while(root->children_L>1)
	{
		if(!move::movesEqual(root->children[i]->moveMade,moveMade)) //if the move doesn't match 'moveMade' then destroy subtree (this should just leave the node for the move that was actually made)
		{
			destroySubTree(root->children[i]);
		}
		else
		{
			i++;
		}
	}
	//2. Set root pointer to only remaining child of root (i.e. the node of the moveMade)
	root = root->children[0];
	//3. Expand the tree so it at least has the next layer after root
	expandFromRoot(1);
	//4. If the position objects in the new root's children has been NULL'd then reinstantiate them
	for(int i=0;i<root->children_L;i++)
	{
		if(root->children[i]->position==NULL)
		{
			root->children[i]->position = new Position(root->position,root->children[i]->moveMade);
		}
	}
}
bool PositionTree::makeMove(const move moveMade)
{
	bool validMove = false;
	//1. check that the move passed is not a "null" move and game is in-play
	if(moveMade.startRank==-1)
		return false;
	if(root->position->positionState!=Position::positionstate_inplay)
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

//--Debug--
void PositionTree::printTreeInfo()
{
	printf("--Position Tree Info--\n");
	printf("Nodes: %ld\n",treeInfo.totalNodes);
}
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
    for(int i=root->depth;i<node->depth;i++)
    {
        printf("\t");
    }
    printf("[%d]: %s  IE: %.3f  BRA: %.3f  BB: %.3f\n",node->depth,moveNotation,node->instantEval,node->branchRecursiveAvg,node->branchBest);
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
	printPositionTree_recursive(root);
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
    for(int i=root->depth;i<node->depth;i++)
    {
        printf("\t");
    }
    printf("[%d]: %s  IE: %.3f  BRA: %.3f  BB: %.3f\n",node->depth,moveNotation,node->instantEval,node->branchRecursiveAvg,node->branchBest);
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
	printPositionTree_recursive(root, depth);
}
