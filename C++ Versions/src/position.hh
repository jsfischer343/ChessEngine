#ifndef __POSITION_HH__
#define __POSITION_HH__
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>

#define MAX_PIECES 16
#define MAX_PIECE_TARGETS 27
#define MAX_PIECE_MOVES 27
#define MAX_SQUARE_TARGETS 22
#define MAX_SQUARE_MOVES 22

class Position
{
	private:
		//--Piece and Square Data Structures--
		struct square;
		struct piece
		{
			char type = '\0'; 									//type of piece ('k','q','r','b','n','p')
			char color = '\0'; 									//'w' or 'b'
			int8_t rank = -1; 									//-1 implies that the piece hasn't been initialized
			int8_t file = -1;
			struct square* squarePtr = NULL; 					//pointer to square object for this piece
			short targets_L = 0;
			short moves_L = 0;
			struct square* targets[MAX_PIECE_TARGETS]; 			//squares this piece is attacking
			struct square* moves[MAX_PIECE_MOVES];				//squares this piece can move to next move
			bool pinned[4] = {false,false,false,false};
		};
		enum PinnedEnum
		{
			pin_negativeDiag,
			pin_positiveDiag,
			pin_rank,
			pin_file
		};
		struct square
		{
			//0,0 is the top left corner 7,7 is the bottom right corner (from white's perspective always)
			int8_t rank = -1; 									//-1 implies that the square hasn't been initialized
			int8_t file = -1;
			struct piece* piecePtr = NULL; 						//pointer to piece object contained on this square
			short targeters_L = 0;
			short moves_L = 0;
			struct piece* targeters[MAX_SQUARE_TARGETS]; 		//pieces that are 'attacking' this square (including pinned pieces)
			struct piece* moves[MAX_SQUARE_MOVES]; 				//pieces that can move to this square next turn
		};
		typedef struct square square;
		typedef struct piece piece;
		
		//--Data--
		square** theBoard;	//8x8
		short whitePieces_L = 0;
		short blackPieces_L = 0;
		piece* whitePieces; //16
		piece* blackPieces; //16
		char colorToMove;
		int8_t enPassantRank = -1;
		int8_t enPassantFile = -1;
		bool castlingFlags[4] = {false,false,false,false};
		bool canCastle[4] = {false,false,false,false};
		enum CastlingEnum
		{
			kingside_white,
			queenside_white,
			kingside_black,
			queenside_black
		};
		short fiftyMoveRuleCounter = -1;
		short moveCounter = -1;
	
	public:
		//--Move--
		struct move
		{
			int8_t startRank = -1;
			int8_t startFile = -1;
			int8_t endRank = -1;
			int8_t endFile = -1;
			char endPieceType = '\0'; //for promotion moves
		};
		typedef struct move move;
		
		//--Functions--
		//-Constructors-
		Position();
		Position(const char* FENString);
		Position(const Position& lastPosition, const move moveMade);
		void castlingConstructor(const Position& lastPosition, const int8_t castlingCode);
		Position(const Position* lastPosition, const move moveMade);
		void castlingConstructor(const Position* lastPosition, const int8_t castlingCode);
		~Position();
		
		//-Debug Information-
		void printBoard();
		void printStats();
		void printAllTargets();
		void printAllMoves();
		
		//-Get-
		int getTotalMoves();
		move* getAllMoves();
		char* getNotation(const move& moveMade);
		
	private:
		void setupMemory();
		void cleanupMemory();
		void sanityCheck();		//check that the position is legal
		
		//-Position Calculation-
		void resolve();			//resolve all targets and moves
			void resolve_targets(char color);
				void resolve_targets_whiteKing(piece* kingPtr, int8_t rank, int8_t file);
				void resolve_targets_blackKing(piece* kingPtr, int8_t rank, int8_t file);
			void resolve_pins(char color);
			void resolve_moves(char color);
		void clean();			//remove all targets and moves
		
		//-Piece Setup-
		void initPiece(int8_t rank, int8_t file, char type, char color);
		void removePiece(piece* piecePtr);
		
		//-Updating Targets & Moves-
		bool pushTarget(piece* piecePtr, square* squareTarget);		//Adds a square that the piece is targeting and adds that piece on the squares targeters list
		bool pushMove(piece* piecePtr, square* squareMove);			//Adds a square that the piece is can move to and adds that piece on the squares 'piece can move here' list
		bool popTarget(piece* piecePtr);	
		bool popMove(piece* piecePtr);
		void removeTarget(piece* piecePtr, square* squareTarget);	
		void removeMove(piece* piecePtr, square* squareMove);
		
		//-Castling-
		bool checkCanCastle(int castlingEnumVal);	//used for evaluating if a given castling move is valid (used by constructor to update canCastle flags)
		
		//-Utility-
		bool onBoard(int8_t rank, int8_t file);
		int getTotalTargeters(square* squarePtr, char color);
		char getPieceColor(square* squarePtr);
		piece* getKingPtr(char color);
};

#endif