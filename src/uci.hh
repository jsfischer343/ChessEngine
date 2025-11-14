#ifndef __UCI_HH__
#define __UCI_HH__
#include <iostream>
#include <cstring>
#include <vector>
#include <regex>
#include <thread>
#include "global.hh"
#include "position.hh"
#include "positiontree.hh"

#define MAX_COMMAND_TOKENS 200
#define SEARCH_TIME_MARGIN 10 //margin in milliseconds that is used to ensure that the engine ends on time

class UCI
{
    private:
        struct ucistates
        {
            bool debug = false;
            bool isready = false;
            bool stop = false;
        };
        struct ucioptions
        {
            std::string dummy = "";
        };
        struct ucigoparams
        {
            std::vector<move> searchMoves;
            bool ponder = false;
            long wTime = -1;
            long bTime = -1;
            long wInc = -1;
            long bInc = -1;
            int movesToGo = -1;
            int depth = -1;
            int nodes = -1;
            int mate = -1;
            long moveTime = -1;
            bool infinite = false;
        };
        typedef struct ucistates ucistates;
        typedef struct ucioptions ucioptions;
        typedef struct ucigoparams ucigoparams;

        //DATA
        std::vector<std::string> commandTokens;
        ucistates uciStates;
        ucioptions uciOptions;
        ucigoparams uciGoParams;
        PositionTree* uciPositionTree = NULL;

        std::thread* goThread = NULL;
        std::chrono::time_point<std::chrono::steady_clock> goThread_startTime;

    public:
        UCI();
        ~UCI();
        void parseCommand(std::string userInput);
            bool parseCommand_validCommand(std::string command);

    private:
        //UCI commands
        //void in_uci(); //caught and managed by CLI class
        void in_debug();
        void in_isready();
        void in_setoption();
        void in_register();
        void in_ucinewgame();
        void in_position();
            void in_position_makeMoves(int startingCommandTokenIndex);
        void in_go();
            void in_go_parse();
            void in_go_searchThread();
                bool in_go_searchThread_shouldStop();
            bool in_go_isGoCommand(std::string command);
        void in_stop();
        void in_ponderhit();
        void in_quit();

        void out_id();
        void out_uciok();
        void out_readyok();
        void out_bestmove(move bestMove);
        // void out_copyprotection();
        // void out_registration();
        void out_info();
        void out_sendOptions();

        //Utility
        move uciNotation_TO_move(std::string uciMoveString);
        std::string move_TO_uciNotation(move engineMove);
        void setupPositionTree();
        void setupPositionTree(std::string fenString);
};

#endif
