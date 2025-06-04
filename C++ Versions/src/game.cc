#include "utilities.hh"
#include "position.hh"
#include "game.hh"

//-Constructors-
Game::Game()
{
	currentPosition = new Position();
}
Game::Game(const char* FENString)
{
	currentPosition = new Position(FENString);
}
Game::~Game()
{
	destroyTree(root);
}

//-Actions-
void Game::makeMove(int8_t startRank, int8_t startFile, int8_t endFile, int8_t endRank, char endPieceType)
{
	
}
//-Information-
void Game::printBestMove()
{
	char* moveNotation = currentPosition->getNotation(bestMove);
	printf("%s\n",moveNotation);
}
//-Debug-
void Game::printBoard()
{
	currentPosition->printBoard();
}
void Game::printAllLegalMoves()
{
	root = generateMoveTree(root, currentPosition, 1);
	printMoveTree(1);
}
void Game::generateMoveTree(int depth)
{
	root = generateMoveTree(root, currentPosition, depth);
}

//-Trees-
Game::tree* Game::generateMoveTree(tree* rootNode, Position* startingPosition, int depth)
{
	return generateMoveTree_recursive(NULL, rootNode, startingPosition, Position::move(), depth);
}
Game::tree* Game::generateMoveTree_recursive(tree* parent, tree* node, Position* position, Position::move moveMade, int depth)
{
	Position::move* currentMoveArr; //move to line 36
	if(depth==0)
	{
		node = new tree();
		node->parent = parent;
		node->position = position;
		node->moveMade = moveMade;
		return node;
	}
	else
	{
		node = new tree();
		node->parent = parent;
		node->position = position;
		node->moveMade = moveMade;
		node->children_L = position->getTotalMoves();
		node->children = new tree*[node->children_L];
		currentMoveArr = position->getAllMoves();
		for(int i=0;i<node->children_L;i++)
		{
			Position* resultingPosition = new Position(position, currentMoveArr[i]);
			node->children[i] = generateMoveTree_recursive(node, node->children[i], resultingPosition, currentMoveArr[i], depth-1);
		}
		delete[] currentMoveArr;
		return node;
	}
}
void Game::printMoveTree(int depth)
{
	if(root->children_L>0 && depth>0)
	{
		for(int i=0;i<root->children_L;i++)
		{
			printMoveTree_recursive(root->children[i],depth-1);
		}
	}
}
void Game::printMoveTree_recursive(tree* node, int depth)
{
	char* moveNotation;
	if(node->children_L==0||depth==0)
	{
		moveNotation = node->parent->position->getNotation(node->moveMade);
		printf("[%d]: %s\n",depth,moveNotation);
		delete[] moveNotation;
	}
	else
	{
		moveNotation = node->parent->position->getNotation(node->moveMade);
		for(int i=0;i<depth;i++)
		{
			printf("\t");
		}
		printf("[%d]: %s\n",depth,moveNotation);
		for(int i=0;i<node->children_L;i++)
		{
			printMoveTree_recursive(node->children[i],depth-1);
		}
		delete[] moveNotation;
	}
}
void Game::destroyTree(tree* node)
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
			destroyTree(node->children[i]);
		}
		delete[] node->children;
		delete node->position;
		delete node;
	}
}

Position::move Game::calculateBestMove()
{
	return Position::move();
}