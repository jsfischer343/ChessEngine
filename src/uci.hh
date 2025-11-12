#ifndef __UCI_HH__
#define __UCI_HH__
#include <iostream>
#include <cstring>
#include <vector>
#include "global.hh"

#define MAX_COMMAND_TOKENS 100

class UCI
{
    private:
        //STATES
        bool state_debug = false;
        bool state_isready = false;
        std::vector<std::string> tokens;

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
        void command_GUItoEngine_go();
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
};

#endif
