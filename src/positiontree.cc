/*
	Bluespiral, a simple open source chess engine
	Copyright (C) 2025 John Fischer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
PositionTree::treenode* PositionTree::generatePositionTreeRecursive(treenode* node, int depth)
{
	move* currentMoveArr;
	bool currentNodePositionObjIsEphemeral = false;

	if(node->position==NULL) //check to make sure that position object exists for this node
	{
		currentNodePositionObjIsEphemeral = true;
		reinstantiateTreeNodePositionObjsRecursiveUpwards(node);
	}

	if(depth==0 || node->position->positionState!=0 || node->drawByRepetition!=false) //if the node is not in play (i.e. no legal moves), draw by repetition, or depth is met then return this node and stop generating
	{
		node->colorToMove = node->position->colorToMove;
		node->positionState = node->position->positionState;
		node->branchRecursiveAvg = node->instantEval;
		node->branchBest = node->instantEval;
		node->longestBranchDepth = node->depth;
		if(currentNodePositionObjIsEphemeral)
		{
			destroyTreeNodePositionObjsRecursiveUpwards(node);
		}
		return node;
	}
	else if(node->children_L!=0) //node already has children but specified recursive depth has not been reached yet (uncommon case)
	{
		for(int i=0;i<node->children_L;i++)
		{
			generatePositionTreeRecursive(node->children[i], depth-1); //call re
		}
		node->colorToMove = node->position->colorToMove;
		node->positionState = node->position->positionState;
		node->branchRecursiveAvg = node->instantEval;
		node->branchBest = node->instantEval;
		node->longestBranchDepth = node->depth;
		refreshTreeCalculationsRecursiveUpwards(node);
		if(currentNodePositionObjIsEphemeral)
		{
			destroyTreeNodePositionObjsRecursiveUpwards(node);
		}
		return node;
	}
	else //otherwise the total moves will need to be known so we get that from the position object
	{
		node->children_L = node->position->getTotalMoves();
	}

	if(node->children_L==0) //if there are no legal moves then this will be a leaf node
	{
		node->colorToMove = node->position->colorToMove;
		node->positionState = node->position->positionState;
		node->branchRecursiveAvg = node->instantEval;
		node->branchBest = node->instantEval;
		node->longestBranchDepth = node->depth;
		if(currentNodePositionObjIsEphemeral)
		{
			destroyTreeNodePositionObjsRecursiveUpwards(node);
		}
		return node;
	}
	else //otherwise expand the children
	{
		if((node->depth-root->depth)+1>treeInfo.depth)
		{
			treeInfo.depth = (node->depth-root->depth)+1;
		}
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
			node->children[i]->positionState = resultingPosition->positionState; //save position state for branch expansion algorithm
			node->children[i]->moveMade = currentMoveArr[i];
			if(checkForRepetition(node->children[i])) //if there is a repetition in the tree then the eval must be set to 0
			{
				node->children[i]->drawByRepetition = true;
				node->children[i]->instantEval = 0;
			}
			else
			{
				node->children[i]->instantEval = resultingPosition->getInstantEval();
			}
			node->children[i]->depth = node->children[i]->parent->depth+1; //set depth of child
			node->children[i]->longestBranchDepth = node->children[i]->depth;
			node->children[i] = generatePositionTreeRecursive(node->children[i], depth-1); //call tree generation recursively
			if(node->children[i]->depth>=(POSITION_OBJ_EPHEMERAL_DEPTH+root->depth)) //if the tree layer is past a certain pre-defined depth then delete the position object to save memory
			{
				delete node->children[i]->position;
				node->children[i]->position = NULL;
			}
			node->branchRecursiveAvg += node->children[i]->branchRecursiveAvg;
		}
		node->branchRecursiveAvg = node->branchRecursiveAvg/node->children_L;
		sortChildrenByBranchBest(node);
		node->branchBest = node->children[0]->branchBest;
		refreshLongestBranchDepthRecursiveUpwards(node);
		delete[] currentMoveArr;
	}
	if(currentNodePositionObjIsEphemeral)
	{
		destroyTreeNodePositionObjsRecursiveUpwards(node);
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
bool PositionTree::expandNextDecisionMatrix()
{
	bool returnValue = false;
	switch(decisionMatrixStateIterator)
	{
		case 0:
			returnValue = this->expandNextBestBranchDeep();
			break;
		case 1:
			returnValue = this->expandNextBestBranchWide();
			break;
		case 2:
			returnValue = this->expandNextBestBranchRecursiveAvgDeep();
			break;
		case 3:
			if(!this->expandNextCheckDeep())
			{
				returnValue = this->expandNextBestBranchDeep();
			}
			else
			{
				returnValue = true;
			}
			break;
		case 4:
			if(!this->expandNextCaptureDeep())
			{
				returnValue = this->expandNextBestBranchDeep();
			}
			else
			{
				returnValue = true;
			}
			break;
		default:
			throw;
			break;
	}
	if(decisionMatrixStateIterator>=1) //TEMP: set to 2 (default should be 4) //note: current check and capture expansion algorithms are extremely slow
	{
		decisionMatrixStateIterator=0;
	}
	else
	{
		decisionMatrixStateIterator++;
	}
	return returnValue;
}
bool PositionTree::expandNextBestBranchDeep()
{
	treenode* nextNodeToExpand = expandNextBestBranchDeep_findExpansionBranchRecursive(root);
	if(nextNodeToExpand==NULL)
	{
		return false;
	}
	expandTree(nextNodeToExpand,1);
	refreshTreeCalculationsRecursiveUpwards(nextNodeToExpand->parent);
	return true;
}
PositionTree::treenode* PositionTree::expandNextBestBranchDeep_findExpansionBranchRecursive(treenode* node)
{
	if(node->children_L==0)
	{
		if(node->depth<(MAX_DEPTH+root->depth) && node->positionState==0)
		{
			return node;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		treenode* nextNodeToExpand;
		for(int i=0;i<node->children_L;i++)
		{
			nextNodeToExpand = expandNextBestBranchDeep_findExpansionBranchRecursive(node->children[i]);
			if(nextNodeToExpand!=NULL)
			{
				return nextNodeToExpand;
			}
		}
	}
	return NULL;
}
bool PositionTree::expandNextBestBranchWide()
{
	treenode* nextNodeToExpand = expandNextBestBranchWide_findExpansionBranch(root);
	if(nextNodeToExpand==NULL)
	{
		return false;
	}
	expandTree(nextNodeToExpand,1);
	refreshTreeCalculationsRecursiveUpwards(nextNodeToExpand->parent);
	return true;
}
PositionTree::treenode* PositionTree::expandNextBestBranchWide_findExpansionBranch(treenode* node)
{
	if(node->children_L==0)
	{
		if(node->depth<(MAX_DEPTH+root->depth) && node->positionState==0)
		{
			return node;
		}
		return NULL;
	}
	int currentLevel=node->depth+1;
	while(currentLevel<=node->longestBranchDepth)
	{
		treenode* nextNodeToExpand;
		for(int i=0;i<node->children_L;i++)
		{
			nextNodeToExpand = expandNextBestBranchWide_findExpansionBranch_limitDepthRecursive(node->children[i], currentLevel);
			if(nextNodeToExpand!=NULL)
			{
				return nextNodeToExpand;
			}
		}
		currentLevel++;
	}
	return NULL;
}
PositionTree::treenode* PositionTree::expandNextBestBranchWide_findExpansionBranch_limitDepthRecursive(treenode* node, int depthLimit)
{
	if(node->depth>depthLimit)
	{
		return NULL;
	}
	if(node->children_L==0)
	{
		if(node->depth<(MAX_DEPTH+root->depth) && node->positionState==0)
		{
			return node;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		treenode* nextNodeToExpand;
		for(int i=0;i<node->children_L;i++)
		{
			nextNodeToExpand = expandNextBestBranchWide_findExpansionBranch_limitDepthRecursive(node->children[i], depthLimit);
			if(nextNodeToExpand!=NULL)
			{
				return nextNodeToExpand;
			}
		}
	}
	return NULL;
}
bool PositionTree::expandNextBestBranchRecursiveAvgDeep()
{
	treenode* nextNodeToExpand = expandNextBestBranchRecursiveAvgDeep_findExpansionBranchRecursive(root);
	if(nextNodeToExpand==NULL)
	{
		return false;
	}
	expandTree(nextNodeToExpand,1);
	refreshTreeCalculationsRecursiveUpwards(nextNodeToExpand->parent);
	return true;
}
PositionTree::treenode* PositionTree::expandNextBestBranchRecursiveAvgDeep_findExpansionBranchRecursive(treenode* node)
{
	if(node->children_L==0)
	{
		if(node->depth<(MAX_DEPTH+root->depth) && node->positionState==0)
		{
			return node;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		//sort all nodes in this branch by best branch average for the current color-
		treenode** branchAvgSorted = new treenode*[node->children_L];
		for(int i=0;i<node->children_L;i++)
		{
			branchAvgSorted[i] = node->children[i];
		}
		treenode* temp;
		if(node->colorToMove=='w')
		{
			for(int i=0;i<node->children_L;i++)
			{
				for(int j=i+1;j<node->children_L;j++)
				{
					if(branchAvgSorted[i]->branchBest < branchAvgSorted[j]->branchBest)
					{
						temp = branchAvgSorted[i];
						branchAvgSorted[i] = branchAvgSorted[j];
						branchAvgSorted[j] = temp;
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
					if(branchAvgSorted[i]->branchBest > branchAvgSorted[j]->branchBest)
					{
						temp = branchAvgSorted[i];
						branchAvgSorted[i] = branchAvgSorted[j];
						branchAvgSorted[j] = temp;
					}
				}
			}
		}
		//-

		treenode* nextNodeToExpand;
		for(int i=0;i<node->children_L;i++)
		{
			nextNodeToExpand = expandNextBestBranchRecursiveAvgDeep_findExpansionBranchRecursive(branchAvgSorted[i]);
			if(nextNodeToExpand!=NULL)
			{
				return nextNodeToExpand;
			}
		}
		delete[] branchAvgSorted;
	}
	return NULL;
}
bool PositionTree::expandNextCheckDeep()
{
	treenode* nextNodeToExpand = expandNextCheckDeep_findExpansionBranchRecursive(root);
	if(nextNodeToExpand==NULL)
	{
		return false;
	}
	expandTree(nextNodeToExpand,1);
	refreshTreeCalculationsRecursiveUpwards(nextNodeToExpand->parent);
	return true;
}
PositionTree::treenode* PositionTree::expandNextCheckDeep_findExpansionBranchRecursive(treenode* node)
{
	if(node->children_L==0)
	{
		reinstantiateTreeNodePositionObjsRecursiveUpwards(node); //if the node's position object is missing this will refresh it
		if(node->depth<(MAX_DEPTH+root->depth) && node->positionState==0 && node->position->isInCheck())
		{
			destroyCurrentTreeNodePositionObject(node); //only destroys if it is at or below the ephemeral depth
			return node;
		}
		else
		{
			destroyCurrentTreeNodePositionObject(node); //only destroys if it is at or below the ephemeral depth
			return NULL;
		}
	}
	else
	{
		treenode* nextNodeToExpand;
		for(int i=0;i<node->children_L;i++)
		{
			nextNodeToExpand = expandNextCheckDeep_findExpansionBranchRecursive(node->children[i]);
			if(nextNodeToExpand!=NULL)
			{
				destroyTreeNodePositionObjsRecursiveUpwards(node); //if this node is at or below the ephemeral depth this will re-destroy all the nodes that were reinstantiated
				return nextNodeToExpand;
			}
		}
		destroyTreeNodePositionObjsRecursiveUpwards(node); //if this node is at or below the ephemeral depth this will re-destroy all the nodes that were reinstantiated
	}
	return NULL;
}
// bool PositionTree::expandNextCheckWide()
// {}
// PositionTree::treenode* PositionTree::expandNextCheckWide_findExpansionBranch(treenode* node)
// {}
// PositionTree::treenode* PositionTree::expandNextCheckWide_findExpansionBranch_limitDepthRecursive(treenode* node, int depthLimit)
// {}
bool PositionTree::expandNextCaptureDeep()
{
	treenode* nextNodeToExpand = expandNextCaptureDeep_findExpansionBranchRecursive(root);
	if(nextNodeToExpand==NULL)
	{
		return false;
	}
	expandTree(nextNodeToExpand,1);
	refreshTreeCalculationsRecursiveUpwards(nextNodeToExpand->parent);
	return true;
}
PositionTree::treenode* PositionTree::expandNextCaptureDeep_findExpansionBranchRecursive(treenode* node)
{
	if(node->children_L==0)
	{
		reinstantiateTreeNodePositionObjsRecursiveUpwards(node); //if the node's position object is missing this will refresh it
		if(node->depth<(MAX_DEPTH+root->depth) && node->positionState==0 && node->parent->position->isCapture(node->moveMade))
		{
			destroyCurrentTreeNodePositionObject(node); //only destroys if it is at or below the ephemeral depth
			return node;
		}
		else
		{
			destroyCurrentTreeNodePositionObject(node); //only destroys if it is at or below the ephemeral depth
			return NULL;
		}
	}
	else
	{
		treenode* nextNodeToExpand;
		for(int i=0;i<node->children_L;i++)
		{
			nextNodeToExpand = expandNextCaptureDeep_findExpansionBranchRecursive(node->children[i]);
			if(nextNodeToExpand!=NULL)
			{
				return nextNodeToExpand;
			}
		}
		destroyTreeNodePositionObjsRecursiveUpwards(node); //if this node is at or below the ephemeral depth this will re-destroy all the nodes that were reinstantiated
	}
	return NULL;
}
// bool PositionTree::expandNextCaptureWide()
// {}
// PositionTree::treenode* PositionTree::expandNextCaptureWide_findExpansionBranch(treenode* node)
// {}
// PositionTree::treenode* PositionTree::expandNextCaptureWide_findExpansionBranch_limitDepthRecursive(treenode* node, int depthLimit)
// {}
//Refresh
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
void PositionTree::refreshLongestBranchDepthRecursiveUpwards(treenode* node)
{
	if(node==NULL)
	{
		treeInfo.depth = root->longestBranchDepth-root->depth;
		return;
	}
	else
	{
		if(node->children_L==0)
		{
			node->longestBranchDepth = node->depth;
		}
		else
		{
			int maxLongestBranchDepth = 0;
			for(int i=0;i<node->children_L;i++)
			{
				if(node->children[i]->longestBranchDepth>maxLongestBranchDepth)
				{
					maxLongestBranchDepth = node->children[i]->longestBranchDepth;
				}
			}
			node->longestBranchDepth = maxLongestBranchDepth;
		}
		refreshLongestBranchDepthRecursiveUpwards(node->parent);
	}
}
//Destroy
void PositionTree::destroyEntireTree_recursive(treenode* node)
{
	if(node->children_L==0)
	{
		treeInfo.totalNodes--;
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
		treeInfo.totalNodes--;
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
		treeInfo.totalNodes--;
		refreshLongestBranchDepthRecursiveUpwards(node->parent);
		//2. Run destroyEntireTree_recursive on 'node' (i.e. the child)
		destroyEntireTree_recursive(node);
	}
}
//Tree Utilities
void PositionTree::reinstantiateTreeNodePositionObjsRecursiveUpwards(treenode* node)
{
	if(node->position!=NULL)
	{
		return;
	}
	else
	{
		reinstantiateTreeNodePositionObjsRecursiveUpwards(node->parent);
		node->position = new Position(node->parent->position,node->moveMade);
	}
}
void PositionTree::destroyTreeNodePositionObjsRecursiveUpwards(treenode* node)
{
	if(node->depth<(POSITION_OBJ_EPHEMERAL_DEPTH+root->depth))
	{
		return;
	}
	else
	{
		delete node->position;
		node->position = NULL;
		destroyTreeNodePositionObjsRecursiveUpwards(node->parent);
	}
}
void PositionTree::destroyCurrentTreeNodePositionObject(treenode* node)
{
	if(node->depth<(POSITION_OBJ_EPHEMERAL_DEPTH+root->depth))
	{
		return;
	}
	if(node!=NULL&&node->position!=NULL)
	{
		delete node->position;
		node->position = NULL;
	}
	else
	{
		throw;
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
    printf("[%d]: %s  IE: %.3f\tBRA: %.3f\tBB: %.3f\n",node->depth,moveNotation,node->instantEval,node->branchRecursiveAvg,node->branchBest);
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
    printf("[%d]: %s  IE: %.3f\tBRA: %.3f\tBB: %.3f\tBMD: %d\n",node->depth,moveNotation,node->instantEval,node->branchRecursiveAvg,node->branchBest,node->longestBranchDepth);
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
void PositionTree::printExtraDebug()
{
	printf("--Extra Debug Info--\n");
	printf("Nodes (by recursive function count): %ld\n",countNodes(root));
	printf("Total Initialized Position Objects: %ld\n",getTotalInitializedPositionObjects(root));
}
long PositionTree::countNodes(treenode* node)
{
	long sum = 0;
	if(node->children_L==0)
	{
		return 1;
	}
	else
	{
		sum += 1; //for current node
		for(int i=0;i<node->children_L;i++)
		{
			sum += countNodes(node->children[i]);
		}
		return sum;
	}
	return sum;
}
long PositionTree::countNodes(treenode* node, int depth)
{
	long sum = 0;
	if(node->children_L==0||depth==0)
	{
		return 1;
	}
	else
	{
		sum += 1; //for current node
		for(int i=0;i<node->children_L;i++)
		{
			sum += countNodes(node->children[i],depth-1);
		}
		return sum;
	}
	return sum;
}
long PositionTree::getTotalInitializedPositionObjects(treenode* node)
{
	long sum = 0;
	if(node->children_L==0)
	{
		if(node->position!=NULL)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if(node->position!=NULL)
		{
			sum += 1;
		}
		for(int i=0;i<node->children_L;i++)
		{
			sum += getTotalInitializedPositionObjects(node->children[i]);
		}
		return sum;
	}
	return sum;
}
