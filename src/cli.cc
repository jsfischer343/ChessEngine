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

#include "cli.hh"

CLI::CLI()
{
	bluespiralUCI = NULL;
}
CLI::~CLI()
{
	if(bluespiralUCI!=NULL)
	{
		delete bluespiralUCI;
	}
}

void CLI::parseCommand(std::string userInput)
{
    if(userInput=="exit"||userInput=="quit")
    {
        return;
    }
    //uci command stream: send to uci
    if(usingUCI)
    {
        bluespiralUCI->parseCommand(userInput);
        return;
    }

    inputTokens.clear(); //reset inputTokens from last command
    if(userInput.size()==0)
    {
        printHelp();
        return;
    }

    bool moreTokens = true;
    while(moreTokens)
	{
		int tokenStart=0;
		while(userInput[tokenStart]==' '||userInput[tokenStart]=='\t')
		{
			tokenStart++;
		}
		int tokenEnd=tokenStart;
		do {
			tokenEnd++;
			if(userInput[tokenEnd]=='\0')
			{
				moreTokens = false;
			}
		} while(userInput[tokenEnd]!=' '&&userInput[tokenEnd]!='\t'&&userInput[tokenEnd]!='\0');
		inputTokens.push_back(userInput.substr(tokenStart,tokenEnd-tokenStart));
		userInput.erase(0,tokenEnd);
	}

    //normal command stream: identify command
	if(inputTokens.at(0)=="uci")
	{
		command_uci();
	}
	else if(inputTokens.at(0)=="help")
	{
		command_help();
	}
	else
    {
        printHelp();
    }
}

void CLI::command_uci()
{
    if(inputTokens.size()!=1)
    {
        printHelp();
        return;
    }
    bluespiralUCI = new UCI();
    usingUCI = true;
}
void CLI::command_help()
{
    printHelp();
}

void CLI::printHelp()
{
    std::cout << "--Bluespiral Help--\n";
    std::cout << "uci:\tTells the engine to switch to uci (universal chess interface) mode.\n";
    std::cout << "help:\tPrints this menu.\n";
}
