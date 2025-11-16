#ifndef __POSITION_HH__
#define __POSITION_HH__
#include <string>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cctype>
#include <cmath>
#include "global.hh"
#include "move.hh"

#define MAX_PIECES 16
#define MAX_PIECE_TARGETS 27	//Max number of targets a piece can have (this will never be greater than 27 in a normal chess game)
#define MAX_PIECE_MOVES 27		//similar to above
#define MAX_SQUARE_TARGETS 22	//Max number of pieces that can target any 1 square
#define MAX_SQUARE_MOVES 22

//Instant Evaluation Tuning Parameters
//Symmetric Parameters
#define PIECE_CONTROL_WEIGHT 0.005	//weight given to controlling a square with a piece on it (in proportion to that piece)
#define SQUARE_CONTROL_WEIGHT 0.01	//weight given to controlling an empty square
#define KING_ADJACENT_SQUARE_CONTROL_WEIGHT_DEFENSIVE 0
#define KING_ADJACENT_SQUARE_CONTROL_WEIGHT_OFFENSIVE 0
#define PAWN_WEIGHT_1 0 //value of pawn that is 1 out 6 squares from promotion
#define PAWN_WEIGHT_2 0 //and so on
#define PAWN_WEIGHT_3 0
#define PAWN_WEIGHT_4 0
#define PAWN_WEIGHT_PASSED_1 0
#define PAWN_WEIGHT_PASSED_2 0
#define PAWN_WEIGHT_PASSED_3 0.1
#define PAWN_WEIGHT_PASSED_4 0.2
#define PAWN_WEIGHT_PASSED_5 0.3 //value of pawn that is one square from promotion
//Asymmetric Parameters
#define TURN_BASED_CONTROL_BONUS 1 //multiplicative factor for controlling a piece on your turn
// // //Symmetric Parameters
// // #define PIECE_CONTROL_WEIGHT 0.1	//weight given to controlling a square with a piece on it (in proportion to that piece)
// // #define SQUARE_CONTROL_WEIGHT 0.05	//weight given to controlling an empty square
// // #define KING_ADJACENT_SQUARE_CONTROL_WEIGHT_DEFENSIVE 0.02
// // #define KING_ADJACENT_SQUARE_CONTROL_WEIGHT_OFFENSIVE 0.02
// // #define PAWN_WEIGHT_1 0.05 //value of pawn that is 1 out 6 squares from promotion
// // #define PAWN_WEIGHT_2 0.1 //and so on
// // #define PAWN_WEIGHT_3 0.15
// // #define PAWN_WEIGHT_4 0.3
// // #define PAWN_WEIGHT_PASSED_1 0.25
// // #define PAWN_WEIGHT_PASSED_2 0.4
// // #define PAWN_WEIGHT_PASSED_3 0.75
// // #define PAWN_WEIGHT_PASSED_4 1.2
// // #define PAWN_WEIGHT_PASSED_5 1.8 //value of pawn that is one square from promotion
// // //Asymmetric Parameters
// // #define TURN_BASED_CONTROL_BONUS 1 //multiplicative factor for controlling a piece on your turn

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
		struct instantEval
		{
			//Material
			float material_pawns = 0;
			float material_knights = 0;
			float material_bishops = 0;
			float material_rooks = 0;
			float material_queens = 0;
			//Positional
			float positional_squarecontrol_base = 0;
			float positional_squarecontrol_king_defensive = 0;
			float positional_squarecontrol_king_offensive = 0;
			float positional_squarecontrol_pawns = 0;
			float positional_squarecontrol_knights = 0;
			float positional_squarecontrol_bishops = 0;
			float positional_squarecontrol_rooks = 0;
			float positional_squarecontrol_queens = 0;
			float positional_pawns_notpassed = 0;
			float positional_pawns_passed = 0;
			float total = 0;
		};
		typedef struct square square;
		typedef struct piece piece;
		typedef struct instantEval instantEval;

		enum PinnedEnum
		{
			pin_negativeDiag,
			pin_positiveDiag,
			pin_rank,
			pin_file
		};
		enum CastlingEnum
		{
			kingside_white,
			queenside_white,
			kingside_black,
			queenside_black
		};
		enum PositionState
		{
			positionstate_inplay,
			positionstate_draw50,
			positionstate_drawStalemate,
			positionstate_drawInsufficientMaterial,
			positionstate_whiteWin,
			positionstate_blackWin
		};
		
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
		short fiftyMoveRuleCounter = -1;
		short moveCounter = -1;
		int8_t positionState = 0;
		instantEval positionEvaluation;
		
		//FUNCTIONS
		//-Constructors-
		Position();
		Position(const char* FENString);
		Position(const Position* lastPosition, const move moveMade);
	private:
		void castlingConstructor(const Position* lastPosition, const int8_t castlingCode);//used to finish up construction when the move made was castling
	public:
		~Position();

		//-Get-
		int getTotalMoves() const;
		move* getAllMoves() const;
		char* getNotation(const move& moveMade) const;
		move getMoveFromNotation(const char* notation) const;
		int getTotalTargeters(square* squarePtr, char color) const;
		int getTotalMovers(square* squarePtr, char color) const;
		char getPieceColor(square* squarePtr) const;
		char getPieceColor(int8_t rank, int8_t file) const;
		char getPieceType(square* squarePtr) const;
		char getPieceType(int8_t rank, int8_t file) const;
		piece* getKingPtr(char color) const;
		float getInstantEval() const;

		//-Debug Information-
		void printBoard();
		void printInfo();
		void printInstantEvalBreakdown();
		void printAllTargets();
		void printAllMoves();

		//-Utility-
		static bool onBoard(int8_t rank, int8_t file);
		bool adjacentToKing(int8_t rank, int8_t file, piece* kingPtr);
		static char engineRank_TO_notationRank(int8_t engineRank);
		static char engineFile_TO_notationFile(int8_t engineFile);
		static int8_t notationRank_TO_engineRank(char notationRank);
		static int8_t notationFile_TO_engineFile(char notationFile);
		static char enginePieceType_TO_notationPieceType(char enginePieceType, char enginePieceColor);
		static bool isFileOrRank(char charInQuestion);
		static bool isRank(char charInQuestion);
		static bool isFile(char charInQuestion);
		
	private:
		//-Memory-
		void setupMemory();
		void cleanupMemory();
		void sanityCheck();		//check that the position is legal
		
		//-Position Calculation-
		void resolve();			//resolve all targets, moves, and compute instant evaluation
			void resolve_targets(char color); //adds targets for pieces in whitePieces and blackPieces array and simultaneously adds targeters to the squares it runs into to the squares on theBoard
				void resolve_targets_scan(piece* currentPiecePtr, int8_t startingRank, int8_t startingFile, int rankDirection, int fileDirection); // rank/fileDirection = -1, 0, or 1
				void resolve_targets_king(piece* kingPtr, int8_t rank, int8_t file);
			void resolve_pins(char color);
				void resolve_pins_scan(piece* kingPtr, int rankDirection, int fileDirection);
			void resolve_moves(char color); //same thing as targets but it determines if the piece can actually move there based on pins
				void resolve_moves_whiteKing(piece* kingPtr, int8_t rank, int8_t file);
				void resolve_moves_blackKing(piece* kingPtr, int8_t rank, int8_t file);
				void resolve_moves_knight(piece* currentPiecePtr, int rankOffset, int fileOffset);
				void resolve_moves_scan(piece* currentPiecePtr, int8_t startingRank, int8_t startingFile, int rankDirection, int fileDirection);
			void resolve_movesInCheck(char color); //similar to resolve_moves() but skips moves that do not resolve check
				bool resolve_movesInCheck_blocksCheck(const piece* kingPtr, const piece* checkingPiecePtr, int8_t endRank, int8_t endFile);
				void resolve_movesInCheck_knight_capturesOnly(piece* currentPiecePtr, piece* checkingPiece, int rankOffset, int fileOffset);
				void resolve_movesInCheck_knight_blocksAndCaptures(piece* kingPtr, piece* currentPiecePtr, piece* checkingPiece, int rankOffset, int fileOffset);
				void resolve_movesInCheck_scan_blocksAndCaptures(piece* kingPtr, piece* currentPiecePtr, piece* checkingPiece, int rankDirection, int fileDirection);
				void resolve_movesInCheck_scan_capturesOnly(piece* currentPiecePtr, piece* checkingPiece, int rankDirection, int fileDirection);
			void resolve_positionState();
				bool resolve_positionState_insufficientMaterial();
			void resolve_instantEval();
				bool resolve_instantEval_passedPawn(int8_t rank, char color, int j);
		void clean();			//remove all targets and moves
		
		//-Piece Setup-
		void initPiece(int8_t rank, int8_t file, char type, char color); //adds a piece but doesn't resolve targets, moves, or validity of piece
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
