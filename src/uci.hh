#ifndef __UCI_HH__
#define __UCI_HH__
#include <iostream>
#include <cstring>
#include <vector>
#include <regex>
#include "global.hh"
#include "position.hh"
#include "positiontree.hh"

#define MAX_COMMAND_TOKENS 100

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
            int depth = 1000;
            int nodes = 50000000;
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

    public:
        UCI();
        ~UCI();
        void parseCommand(std::string userInput);
            bool parseCommand_validCommand(std::string command);

    private:
        //UCI commands
        //void command_GUItoEngine_uci(); //caught and managed by CLI class
        void command_GUItoEngine_debug();
        void command_GUItoEngine_isready();
        void command_GUItoEngine_setoption();
        void command_GUItoEngine_register();
        void command_GUItoEngine_ucinewgame();
        void command_GUItoEngine_position();
            void command_GUItoEngine_position_makeMoves(int startingIndex);
        void command_GUItoEngine_go();
            bool command_GUItoEngine_go_isGoCommand(std::string command);
        void command_GUItoEngine_stop();
        void command_GUItoEngine_ponderhit();
        void command_GUItoEngine_quit();

        void command_EnginetoGUI_id();
        void command_EnginetoGUI_uciok();
        void command_EnginetoGUI_readyok();
        void command_EnginetoGUI_bestmove();
        void command_EnginetoGUI_copyprotection();
        void command_EnginetoGUI_registration();
        void command_EnginetoGUI_info();
        void command_EnginetoGUI_option();

        //Utility
        move uciNotation_TO_move(std::string uciMoveString);
        std::string move_TO_uciNotation(move engineMove);
        void setupPositionTree();
        void setupPositionTree(std::string fenString);
};

#endif
