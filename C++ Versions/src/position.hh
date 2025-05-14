#ifndef __POSITION_HH__
#define __POSITION_HH__
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>

class Position
{
private:
	struct square;
	struct piece
	{
		char type = '\0'; //type of piece ('k','q','r','b','n','p')
		char color = '\0'; //'w' or 'b'
		std::uint8_t rank = 0;
		std::uint8_t file = 0;
		struct square* squarePtr = NULL; //pointer to square object for this piece
		struct square* targets[27];
	};
	struct square
	{
		//0,0 is the top left corner 7,7 is the bottom right corner (from white's perspective always)
		std::uint8_t rank = 0;
		std::uint8_t file = 0;
		struct piece* piecePtr = NULL; //pointer to piece object contained on this square
		struct piece* whiteTargeters[12];
		struct piece* blackTargeters[12];
	};
	typedef struct square square;
	typedef struct piece piece;
	piece* whitePieces;
	piece* blackPieces;
	square** theBoard;
	char colorToMove;
	enum CastlingEnum
	{
		kingside_white,
		queenside_white,
		kingside_black,
		queenside_black
	};
	bool castlingFlags[4] = {false,false,false,false};
	short fiftyMoveRuleCounter;
	short moveCounter;
public:
	struct move
	{
		std::uint8_t startFile = 0;
		std::uint8_t startRank = 0;
		std::uint8_t endFile = 0;
		std::uint8_t endRank = 0;
	};
	typedef struct move move;
private:
	move* lastMove;
public:
	Position();
	Position(const char* FENString);
	Position(const Position& lastPosition);
	void printBoard();
	void printStats();
	~Position();
private:
	void placePiece(std::uint8_t rank, std::uint8_t file, char type, char color);
	void addTargets(piece* pieceInQuestion);
	void removeTargets(piece* pieceInQuestion);
};

#endif