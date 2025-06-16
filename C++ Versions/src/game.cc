#include "utilities.hh"
#include "position.hh"
#include "game.hh"

//-Constructors-
Game::Game()
{
	previousPositions_L = 0;
	previousPositions = new Position*[TOTAL_PREVIOUS_POSITIONS];
	currentPosition = new Position();
	generateMoveTreeFromRoot(currentPosition, 1);
	calculateBestMove();
}
Game::Game(const char* FENString)
{
	previousPositions_L = 0;
	previousPositions = new Position*[TOTAL_PREVIOUS_POSITIONS];
	currentPosition = new Position(FENString);
	generateMoveTreeFromRoot(currentPosition, 1);
	calculateBestMove();
}
Game::~Game()
{
	if(root!=NULL)
	{
		destroyEntireTree();
	}
	delete currentPosition;
	for(int i=0;i<previousPositions_L;i++)
	{
		delete previousPositions[i];
	}
	delete[] previousPositions;
}

//-Actions-
bool Game::makeMove(const move moveMade)
{
	bool validMove = false;
	if(moveMade.endPieceType=='\0')
		return false;
	if(gameState!=gamestate_inplay)
		return false;
	//2. Check move made against these moves if not found it is not a valid move
	if(root==NULL)
		generateMoveTreeFromRoot(currentPosition, 1);
	for(int i=0;i<root->children_L;i++)
	{
		if(root->children[i]->moveMade.startRank == moveMade.startRank &&
		root->children[i]->moveMade.startFile == moveMade.startFile &&
		root->children[i]->moveMade.endRank == moveMade.endRank &&
		root->children[i]->moveMade.endFile == moveMade.endFile &&
		root->children[i]->moveMade.endPieceType == moveMade.endPieceType
		)
		{
			validMove = true;
			break;
		}
	}
	//3. Make move if it is valid
	if(validMove)
	{
		queuePreviousPosition(currentPosition);
		currentPosition = new Position(currentPosition, moveMade);
	}
	//4. Calculate best move
	calculateBestMove();
	resolveGameState();
	//5. return whether or not the move was made
	return validMove;
}

//-Debug Information-
void Game::printBestMove()
{
	if(currentPosition!=NULL)
	{
		char* moveNotation = currentPosition->getNotation(bestMove);
		printf("%s\n",moveNotation);
		delete moveNotation;
	}
}
void Game::printBoard()
{
	if(currentPosition!=NULL)
	{
		currentPosition->printBoard();
	}
	else
	{
		printf("There is no initialized current position.");
	}
}
void Game::printAllLegalMoves()
{
	printMoveTree(1);
}
void Game::printMoveTree_recursive(tree* node, int depth)
{
	char* moveNotation;
	if(node->children_L==0||depth==0)
	{
		moveNotation = node->parent->position->getNotation(node->moveMade);
		for(int i=0;i<depth;i++)
		{
			printf("\t");
		}
		printf("[%d]: %s  Instant Eval: %.3f  Branch Best: %.3f\n",depth,moveNotation,node->instantEval,node->branchBest);
		delete[] moveNotation;
	}
	else
	{
		moveNotation = node->parent->position->getNotation(node->moveMade);
		for(int i=0;i<depth;i++)
		{
			printf("\t");
		}
		printf("[%d]: %s  Instant Eval: %.3f  Branch Best: %.3f\n",depth,moveNotation,node->instantEval,node->branchBest);
		for(int i=0;i<node->children_L;i++)
		{
			printMoveTree_recursive(node->children[i],depth-1);
		}
		delete[] moveNotation;
	}
}
void Game::printMoveTree(int depth)
{
	if(root==NULL)
	{
		printf("Move tree hasn't been generated\n");
		return;
	}
	if(root->children_L>0 && depth>0)
	{
		for(int i=0;i<root->children_L;i++)
		{
			printMoveTree_recursive(root->children[i],depth-1);
		}
	}
}

//-Tree- (public)
void Game::generateMoveTree(int depth)
{
	if(root!=NULL)
	{
		destroyEntireTree();
	}
	generateMoveTreeFromRoot(currentPosition, depth);
}


//-Comparision
bool Game::positionObjCompare(Position* position1, Position* position2)
{
	//Compares that two positions have pieces on same squares (as opposed to identical in memory)
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

//-Game State-
void Game::queuePreviousPosition(Position* previousPosition)
{
	if(previousPositions_L!=TOTAL_PREVIOUS_POSITIONS)
	{
		for(int i=previousPositions_L;i>0;i--)
		{
			previousPositions[i] = previousPositions[i-1];
		}
		previousPositions[0] = previousPosition;
		previousPositions_L++;
	}
	else
	{
		delete previousPositions[TOTAL_PREVIOUS_POSITIONS-1];
		previousPositions_L--;
		for(int i=previousPositions_L;i>0;i--)
		{
			previousPositions[i] = previousPositions[i-1];
		}
		previousPositions[0] = previousPosition;
		previousPositions_L++;
	}
}
bool Game::resolveGameState_repetition()
{
	short repetitionCount = 0;
	for(int i=0;i<previousPositions_L;i++)
	{
		if(positionObjCompare(currentPosition,previousPositions[i]))
		{
			repetitionCount++;
			if(repetitionCount==2)
			{
				return true;
			}
		}
	}
	return false;
}
void Game::resolveGameState()
{
	if(currentPosition->fiftyMoveRuleCounter!=100)
	{
		if(root->children_L!=0)
		{
			if(resolveGameState_repetition())
			{
				gameState = gamestate_drawRepetition;
				return;
			}
			else
			{
				gameState = gamestate_inplay;
				return;
			}
		}
		else
		{
			Position::piece* tempKingPtr;
			if(currentPosition->colorToMove=='w')
			{
				tempKingPtr = currentPosition->getKingPtr('w');
				if(currentPosition->getTotalTargeters(tempKingPtr->squarePtr,'b')!=0)
				{
					gameState = gamestate_blackWin;
					return;
				}
				else
				{
					gameState = gamestate_drawStalemate;
					return;
				}
			}
			else
			{
				tempKingPtr = currentPosition->getKingPtr('b');
				if(currentPosition->getTotalTargeters(tempKingPtr->squarePtr,'w')!=0)
				{
					gameState = gamestate_whiteWin;
					return;
				}
				else
				{
					gameState = gamestate_drawStalemate;
					return;
				}
			}
		}
	}
	else
	{
		gameState = gamestate_draw50;
		return;
	}
}

//-Trees-
//Create
Game::tree* Game::generateMoveTreeFromRoot_recursive(tree* parent, tree* node, Position* position, move moveMade, int depth)
{
	move* currentMoveArr;
	if(getMemoryUsage()>TREE_MEMORY_LIMIT)
	{
		treeMemoryOverflow();
	}
	else if(depth==0)
	{
		node = new tree();
		node->parent = parent;
		node->position = position;
		node->moveMade = moveMade;
		node->instantEval = position->instantEval();
		return node;
	}
	else
	{
		node = new tree();
		node->parent = parent;
		node->position = position;
		node->moveMade = moveMade;
		node->instantEval = position->instantEval();
		node->children_L = position->getTotalMoves();
		if(node->children_L==0)
		{
			return node;
		}
		else
		{
			node->children = new tree*[node->children_L];
			currentMoveArr = position->getAllMoves();
			for(int i=0;i<node->children_L;i++)
			{
				Position* resultingPosition = new Position(position, currentMoveArr[i]);
				node->children[i] = generateMoveTreeFromRoot_recursive(node, node->children[i], resultingPosition, currentMoveArr[i], depth-1);
			}
			delete[] currentMoveArr;
		}
		return node;
	}
	return node;
}
void Game::generateMoveTreeFromRoot(Position* startingPosition, int depth)
{
	root = generateMoveTreeFromRoot_recursive(NULL, root, startingPosition, move(), depth);
}
Game::tree* Game::expandMoveTreeFromNode_recursive(tree* parent, tree* node, Position* position, move moveMade, int depth)
{
	move* currentMoveArr;
	if(getMemoryUsage()>TREE_MEMORY_LIMIT)
	{
		treeMemoryOverflow();
	}
	else if(depth==0)
	{
		node = new tree();
		node->parent = parent;
		node->position = position;
		node->moveMade = moveMade;
		node->instantEval = position->instantEval();
		return node;
	}
	else
	{
		node = new tree();
		node->parent = parent;
		node->position = position;
		node->moveMade = moveMade;
		node->instantEval = position->instantEval();
		node->children_L = position->getTotalMoves();
		if(node->children_L==0)
		{
			return node;
		}
		else
		{
			node->children = new tree*[node->children_L];
			currentMoveArr = position->getAllMoves();
			for(int i=0;i<node->children_L;i++)
			{
				Position* resultingPosition = new Position(position, currentMoveArr[i]);
				node->children[i] = expandMoveTreeFromNode_recursive(node, node->children[i], resultingPosition, currentMoveArr[i], depth-1);
			}
			delete[] currentMoveArr;
		}
		return node;
	}
	return node;
}
void Game::expandMoveTreeFromNode(tree* node, int depth)
{
	move* currentMoveArr;
	if(getMemoryUsage()>TREE_MEMORY_LIMIT)
	{
		treeMemoryOverflow();
	}
	else if(depth==0)
	{
		return;
	}
	else
	{
		node->children_L = node->position->getTotalMoves();
		if(node->children_L==0)
		{
			return;
		}
		else
		{
			node->children = new tree*[node->children_L];
			currentMoveArr = node->position->getAllMoves();
			for(int i=0;i<node->children_L;i++)
			{
				Position* resultingPosition = new Position(node->position, currentMoveArr[i]);
				node->children[i] = expandMoveTreeFromNode_recursive(node,node->children[i],resultingPosition,currentMoveArr[i],depth-1);
			}
			delete[] currentMoveArr;
		}
	}
}
void Game::expandBestBranches_sort(tree** bestBranches, const int bestBranches_L)
{
	tree* temp;
	for(int i=0;i<bestBranches_L;i++)
	{
		for(int j=i+1;j<bestBranches_L;j++)
		{
			if(bestBranches[i]->instantEval > bestBranches[j]->instantEval)
			{
				temp = bestBranches[i];
				bestBranches[i] = bestBranches[j];
				bestBranches[j] = temp;
			}
		}
	}
}
void Game::expandBestBranches(tree* baseNode, const int numberOfBranches)
{
	expandMoveTreeFromNode(baseNode, 1);
	if(baseNode->children_L<=numberOfBranches) //if the number of branches (i.e. moves) available to expand to is less than the number that the expand request is being made for then no branches need to be thrown out, therefore just return.
		return;

	//Setup
	tree** bestBranches = new tree*[numberOfBranches];

	if(baseNode->position->colorToMove=='w')
	{
		for(int i=0;i<numberOfBranches;i++)
		{
			bestBranches[i] = baseNode->children[i];
		}
		expandBestBranches_sort(bestBranches, numberOfBranches);
		for(int i=numberOfBranches;i<baseNode->children_L;i++)
		{
			if(baseNode->children[i]->instantEval>bestBranches[0]->instantEval)
			{
				bestBranches[0] = baseNode->children[i];
				expandBestBranches_sort(bestBranches, numberOfBranches);
			}
		}
	}
	else
	{
		for(int i=0;i<numberOfBranches;i++)
		{
			bestBranches[i] = baseNode->children[i];
		}
		expandBestBranches_sort(bestBranches, numberOfBranches);
		for(int i=numberOfBranches;i<baseNode->children_L;i++)
		{
			if(baseNode->children[i]->instantEval<bestBranches[numberOfBranches-1]->instantEval)
			{
				bestBranches[numberOfBranches-1] = baseNode->children[i];
				expandBestBranches_sort(bestBranches, numberOfBranches);
			}
		}
	}
	bool tempBool;
	for(int i=baseNode->children_L-1;i>=0;i--)
	{
		tempBool = false;
		for(int j=0;j<numberOfBranches;j++)
		{
			if(bestBranches[j]==NULL)
			{
				break;
			}
			else
			{
				if(baseNode->children[i]==bestBranches[j])
				{
					tempBool = true;
				}
			}
		}
		if(!tempBool)
		{
			destroySubTree(baseNode->children[i]);
		}
	}
	delete[] bestBranches;
}
void Game::reducedRecursiveExpansion_recursive(tree* node, const int numberOfBranches, int depth)
{
	if(depth==0)
	{
		return;
	}
	expandBestBranches(node,numberOfBranches);
	for(int i=0;i<node->children_L;i++)
	{
		reducedRecursiveExpansion_recursive(node->children[i],numberOfBranches,depth-1);
	}
}
void Game::reducedRecursiveExpansion(const int numberOfBranches, int depth)
{
	generateMoveTree(2);
	for(int i=0;i<root->children_L;i++)
	{
		for(int j=0;j<root->children[i]->children_L;j++)
		{
			reducedRecursiveExpansion_recursive(root->children[i]->children[j],numberOfBranches,depth-2);
		}
	}
}
//Destroy
void Game::destroyEntireTree_recursive(tree* node)
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
void Game::destroyEntireTree()
{
	if(root!=NULL)
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
}
void Game::destroySubTree_popChildFromParent(tree* parent, tree* child)
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
void Game::destroySubTree_recursiveLayer1(tree* node)
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
void Game::destroySubTree_recursiveLayer2(tree* node)
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
void Game::destroySubTree(tree* node)
{
	if(node!=NULL && node!=root)
	{
		destroySubTree_recursiveLayer1(node);
	}
}
//Misc
void Game::refreshBranchBest(tree* node, int depth)
{
	tree* bestNode = NULL;
	if(depth==0||node->children_L==0)
	{
		node->branchBest = node->instantEval;
		return;
	}
	else
	{
		for(int i=0;i<node->children_L;i++)
		{
			refreshBranchBest(node->children[i],depth-1);
			if(node->position->colorToMove=='w')
			{
				if(bestNode==NULL)
				{
					bestNode = node->children[i];
				}
				else
				{
					if(node->children[i]->branchBest>bestNode->branchBest)
					{
						bestNode = node->children[i];
					}
				}
			}
			else
			{
				if(bestNode==NULL)
				{
					bestNode = node->children[i];
				}
				else
				{
					if(node->children[i]->branchBest<bestNode->branchBest)
					{
						bestNode = node->children[i];
					}
				}
			}
		}
		node->branchBest = bestNode->branchBest;
	}
}

//-Calculation-
/* ~Deprecated~
void Game::calculateBestMove_recursiveHalfExpansion(int depth)
{
	double average = 0;
	tree* currentNode;
	for(int i=0;i<root->children_L;i++)
	{
		for(int j=0;j<root->children[i]->children_L;j++)
		{
			average = 0;
			currentNode = root->children[i]->children[j];
			for(int k=0;k<currentNode->children_L;k++)
			{
				average = average+currentNode->children[k]->instantEval;
			}
			average = average/currentNode->children_L;
			if(currentNode->position->colorToMove=='w')
			{
				for(int k=0;k<currentNode->children_L;k++)
				{
					if(currentNode->children[k]->instantEval<average)
					{
						destroySubTree(currentNode->children[k]);
					}
					else
					{
						calculateBestMove_recursiveHalfExpansion_recursive(currentNode->children[k], depth-3);
					}
				}
			}
			else
			{
				for(int k=0;k<currentNode->children_L;k++)
				{
					if(currentNode->children[k]->instantEval>average)
					{
						destroySubTree(currentNode->children[k]);
					}
					else
					{
						calculateBestMove_recursiveHalfExpansion_recursive(currentNode->children[k], depth-3);
					}
				}
			}
		}
	}
}
void Game::calculateBestMove_recursiveHalfExpansion_recursive(tree* node, int depth)
{
	if(depth==0)
	{
		return;
	}
	expandMoveTreeFromNode(node,1);
	double average = 0;
	for(int i=0;i<node->children_L;i++)
	{
		average = average+node->children[i]->instantEval;
	}
	average = average/node->children_L;
	if(node->position->colorToMove=='w')
	{
		for(int i=0;i<node->children_L;i++)
		{
			if(node->children[i]->instantEval<average)
			{
				destroySubTree(node->children[i]);
			}
			else
			{
				calculateBestMove_recursiveHalfExpansion_recursive(node->children[i], depth-1);
			}
		}
	}
	else
	{
		for(int i=0;i<node->children_L;i++)
		{
			if(node->children[i]->instantEval>average)
			{
				destroySubTree(node->children[i]);
			}
			else
			{
				calculateBestMove_recursiveHalfExpansion_recursive(node->children[i], depth-1);
			}
		}
	}
}
*/

void Game::calculateBestMove()
{
	destroyEntireTree();
	reducedRecursiveExpansion(TREE_CHILDREN_MAX,TREE_DEPTH);
	refreshBranchBest(root,TREE_DEPTH);

	tree* bestNode;
	if(root->children_L!=0)
		bestNode = root->children[0];
	if(root->position->colorToMove=='w')
	{
		for(int i=0;i<root->children_L;i++)
		{
			if(root->children[i]->branchBest>bestNode->branchBest)
			{
				bestNode = root->children[i];
			}
		}
	}
	else
	{
		for(int i=0;i<root->children_L;i++)
		{
			if(root->children[i]->branchBest<bestNode->branchBest)
			{
				bestNode = root->children[i];
			}
		}
	}
	bestMove = bestNode->moveMade;

	destroyEntireTree();
	generateMoveTree(1);
}

//-Memory-
long Game::getMemoryUsage()
{
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return usage.ru_maxrss;
}
void Game::treeMemoryOverflow()
{
		fprintf(stderr,"Error: Tree expansion exceeded specified memory limits. Raise limits or reduce expansion.\n");
		struct rusage usage;
		getrusage(RUSAGE_SELF, &usage);
		double peakUsage = usage.ru_maxrss/1000;
		fprintf(stderr,"Peak memory usage: %.1f MB\n", peakUsage);
		exit(1);
}
