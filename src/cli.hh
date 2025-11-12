#ifndef __CLI_HH__
#define __CLI_HH__
#include <iostream>
#include <cstring>
#include <vector>
#include <ctime>
#include "global.hh"
#include "uci.hh"
#include "game.hh"

#define MAX_COMMAND_TOKENS 100

class CLI
{
    private:
        bool usingUCI = false;
        UCI bluespiralUCI;
        std::vector<std::string> tokens;

    public:
        CLI();
        ~CLI();
        void parseCommand(std::string userInput);

    private:
        void command_uci();
        void command_botvsbot();
        void command_humanvsbot();
        void command_humanvshuman();
        void command_help();

        void printHelp();
        void printHelp(std::string command);
};

#endif
