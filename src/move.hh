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
