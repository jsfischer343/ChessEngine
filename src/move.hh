#ifndef __MOVE_HH__
#define __MOVE_HH__

//--Move--
struct move
{
	int8_t startRank = -1;
	int8_t startFile = -1;
	int8_t endRank = -1;
	int8_t endFile = -1;
	char endPieceType = '\0'; //needed exclusively for promotion moves
	static bool movesEqual(move move1, move move2)
	{
		if(move1.startRank==move2.startRank &&
		move1.startFile==move2.startFile &&
		move1.endRank==move2.endRank &&
		move1.endFile==move2.endFile &&
		move1.endPieceType==move2.endPieceType)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
typedef struct move move;

#endif
