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
    //uci command stream: send to uci
    if(usingUCI)
    {
        bluespiralUCI->parseCommand(userInput);
        return;
    }
    if(userInput=="exit"||userInput=="quit")
    {
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
    printf("--Bluespiral Help--\n");
    printf("uci:\tTells the engine to switch to uci (universal chess interface) mode.\n");
    printf("help:\tPrints this menu.\n");
}
