#ifndef __POSITION_HH__
#define __POSITION_HH__
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include <cmath>

#define MAX_PIECES 16
#define MAX_PIECE_TARGETS 27	//Max number of targets a piece can have (this will never be greater than 27 in a normal chess game)
#define MAX_PIECE_MOVES 27		//similar to above
#define MAX_SQUARE_TARGETS 22	//Max number of pieces that can target any 1 square
#define MAX_SQUARE_MOVES 22

#define PIECE_CONTROL_WEIGHT 0.2	//During instant eval the weight given to controlling a square with a piece on it (in proportion to that piece)
#define SQUARE_CONTROL_WEIGHT 0.1	//During instant eval the weight given to controlling an empty square
#define PAWN_WEIGHT_1 0.02 //value of pawn that is 1 out 6 squares from promotion
#define PAWN_WEIGHT_2 0.08 //and so on
#define PAWN_WEIGHT_3 0.15
#define PAWN_WEIGHT_4 0.5
#define PAWN_WEIGHT_PASSED_1 0.5
#define PAWN_WEIGHT_PASSED_2 0.5
#define PAWN_WEIGHT_PASSED_3 1
#define PAWN_WEIGHT_PASSED_4 1.2
#define PAWN_WEIGHT_PASSED_5 2 //value of pawn that is one square from promotion

//--Move--
struct move
{
	int8_t startRank = -1;
	int8_t startFile = -1;
	int8_t endRank = -1;
	int8_t endFile = -1;
	char endPieceType = '\0'; //needed for promotion moves
};
typedef struct move move;

class Position
{
	public:
		//STRUCTS
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
		
		//DATA
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
		int8_t positionState = 0;
		enum PositionState
		{
			positionstate_inplay,
			positionstate_draw50,
			positionstate_drawStalemate,
			positionstate_drawInsufficientMaterial,
			positionstate_whiteWin,
			positionstate_blackWin
		};
		
		//FUNCTIONS
		//-Constructors-
		Position();
		Position(const char* FENString);
		Position(const Position& lastPosition, const move moveMade);
	private:
		void castlingConstructor(const Position& lastPosition, const int8_t castlingCode);
	public:
		Position(const Position* lastPosition, const move moveMade);
	private:
		void castlingConstructor(const Position* lastPosition, const int8_t castlingCode);
	public:
		~Position();

		//-Get-
		int getTotalMoves();
		move* getAllMoves();
		char* getNotation(const move& moveMade);
		move getMoveFromNotation(const char* notation);
		int getTotalTargeters(square* squarePtr, char color);
		int getTotalMovers(square* squarePtr, char color);
		char getPieceColor(square* squarePtr);
		piece* getKingPtr(char color);
		double instantEval();
	private:
			bool instantEval_passedPawn(int8_t rank, char color, int j);

	public:
		//-Debug Information-
		void printBoard();
		void printStats();
		void printAllTargets();
		void printAllMoves();

		//-Utility-
		bool onBoard(int8_t rank, int8_t file);
		
	private:
		void setupMemory();
		void cleanupMemory();
		void sanityCheck();		//check that the position is legal
		
		//-Position Calculation-
		void resolve();			//resolve all targets and moves
			void resolve_targets(char color);
				void resolve_targets_scan(piece* currentPiecePtr, int8_t startingRank, int8_t startingFile, int rankDirection, int fileDirection); // rank/fileDirection = -1, 0, or 1
				void resolve_targets_king(piece* kingPtr, int8_t rank, int8_t file);
			void resolve_pins(char color);
				void resolve_pins_scan(piece* kingPtr, int rankDirection, int fileDirection);
			void resolve_moves(char color);
				void resolve_moves_whiteKing(piece* kingPtr, int8_t rank, int8_t file);
				void resolve_moves_blackKing(piece* kingPtr, int8_t rank, int8_t file);
				void resolve_moves_knight(piece* currentPiecePtr, int rankOffset, int fileOffset);
				void resolve_moves_scan(piece* currentPiecePtr, int8_t startingRank, int8_t startingFile, int rankDirection, int fileDirection);
			void resolve_movesInCheck(char color);
				bool resolve_movesInCheck_blocksCheck(const piece* kingPtr, const piece* checkingPiecePtr, int8_t endRank, int8_t endFile);
				void resolve_movesInCheck_knight_capturesOnly(piece* currentPiecePtr, piece* checkingPiece, int rankOffset, int fileOffset);
				void resolve_movesInCheck_knight_blocksAndCaptures(piece* kingPtr, piece* currentPiecePtr, piece* checkingPiece, int rankOffset, int fileOffset);
				void resolve_movesInCheck_scan_blocksAndCaptures(piece* kingPtr, piece* currentPiecePtr, piece* checkingPiece, int rankDirection, int fileDirection);
				void resolve_movesInCheck_scan_capturesOnly(piece* currentPiecePtr, piece* checkingPiece, int rankDirection, int fileDirection);
			void resolve_positionState();
				bool resolve_positionState_insufficientMaterial();
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
};

#endif
