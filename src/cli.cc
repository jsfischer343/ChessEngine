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

    commandTokens.clear(); //reset commandTokens from last command
    if(userInput.size()==0)
    {
        printHelp();
        return;
    }

	int tokenStart=0;
	int tokenEnd=tokenStart;
	do {
		tokenEnd++;
	} while(userInput[tokenEnd]!=' '&&userInput[tokenEnd]!='\t'&&userInput[tokenEnd]!='\0'&&tokenEnd<userInput.length());

    //save command
	std::string command = userInput.substr(tokenStart,tokenEnd-tokenStart);
    userInput.erase(0,tokenEnd);

    //save commandTokens (i.g. arguments)
    bool dontPushToken = false;
    bool moreTokens = true;
    while(moreTokens&&commandTokens.size()<=MAX_COMMAND_TOKENS)
    {
        if(userInput[0]=='\0')
        {
            break;
        }
        tokenStart=0;
        while(userInput[tokenStart]==' '||userInput[tokenStart]=='\t')
        {
            if(userInput[tokenStart]=='\0')
            {
                moreTokens = false;
                dontPushToken = true;
                break;
            }
            tokenStart++;
        }

        //find end of command token
        tokenEnd=tokenStart;
        do {
            if(userInput[tokenEnd]=='\0')
            {
                moreTokens = false;
                break;
            }
            tokenEnd++;
        } while(userInput[tokenEnd]!=' '&&userInput[tokenEnd]!='\t');
        if(!dontPushToken)
        {
            commandTokens.push_back(userInput.substr(tokenStart,tokenEnd-tokenStart));
            userInput.erase(0,tokenEnd);
        }
    }

    //normal command stream: identify command
	if(command=="uci")
	{
		command_uci();
	}
	else if(command=="botvsbot")
	{
		command_botvsbot();
	}
	else if(command=="humanvsbot")
	{
		command_humanvsbot();
	}
	else if(command=="humanvshuman")
	{
		command_humanvshuman();
	}
	else if(command=="help")
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
    if(commandTokens.size()!=0)
    {
        printHelp("uci");
        return;
    }
    bluespiralUCI = new UCI();
    usingUCI = true;
}
void CLI::command_botvsbot()
{
    if(commandTokens.size()!=0)
    {
        printHelp("botvsbot");
        return;
    }
	Game botVsBotGame = Game();
	while(botVsBotGame.getGameState()==0)
	{
		botVsBotGame.updateBestMoveRandom();
		botVsBotGame.printBestMove();
		botVsBotGame.makeMove(botVsBotGame.bestMove);
	}

	int8_t gameState = botVsBotGame.getGameState();
	switch(gameState)
	{
		case 0:
			printf("Error 22\n");
			break;
		case 1:
			printf("Draw by 50 move rule\n");
			break;
		case 2:
			printf("Draw by stalemate.\n");
			break;
		case 3:
			printf("Draw by insufficient material.\n");
			break;
		case 4:
			printf("White win.\n");
			break;
		case 5:
			printf("Black win.\n");
			break;
		case 6:
			printf("Draw by threefold repetition.\n");
			break;
		default:
			printf("Error 36\n");
			break;
	}
}
void CLI::command_humanvsbot()
{
    if(commandTokens.size()>1)
    {
        printHelp("humanvsbot");
        return;
    }
    char humanColor;
    if(commandTokens.size()==1)
    {
        if(commandTokens.at(0)=="w")
        {
            humanColor='w';
        }
        else if(commandTokens.at(0)=="b")
        {
            humanColor='b';
        }
        else
        {
            printHelp("humanvsbot");
            return;
        }
    }
    else
    {
        srand(time(0));
        int randomIndex = rand()%2;
        if(randomIndex==0)
        {
            humanColor='w';
        }
        else
        {
            humanColor='b';
        }
    }
	bool validMove;
	char* buffer = new char[20];
	Game botVsHumanGame = Game();
	if(humanColor=='w')
	{
		while(botVsHumanGame.getGameState()==0)
		{
			botVsHumanGame.printBoard();
			do
			{
				fgets(buffer, sizeof(buffer), stdin);
				if(buffer[0]=='q' && buffer[1]=='\n')
				{
					delete[] buffer;
					return;
				}
				validMove = botVsHumanGame.makeMove(botVsHumanGame.getCurrentPosition()->getMoveFromNotation(buffer));
				if(validMove==false)
					std::cout << "Invalid move. Type 'q' to quit. Type 'q' to quit or enter valid move.\n";
			} while(validMove==false);
			if(botVsHumanGame.getGameState()!=0)
			{
				botVsHumanGame.printBoard();
				break;
			}
			do
			{
				botVsHumanGame.updateBestMoveRandom();
				botVsHumanGame.printBestMove();
				validMove = botVsHumanGame.makeMove(botVsHumanGame.bestMove);
				if(validMove==false)
					throw;
			} while(validMove==false);
		}
	}
	else
	{
		while(botVsHumanGame.getGameState()==0)
		{
			do
			{
				botVsHumanGame.updateBestMoveRandom();
				botVsHumanGame.printBestMove();
				validMove = botVsHumanGame.makeMove(botVsHumanGame.bestMove);
				if(validMove==false)
					throw;
			} while(validMove==false);
			botVsHumanGame.printBoard();
			if(botVsHumanGame.getGameState()!=0)
				break;
			do
			{
				fgets(buffer, sizeof(buffer), stdin);
				if(buffer[0]=='q' && buffer[1]=='\n')
				{
					delete[] buffer;
					return;
				}
				validMove = botVsHumanGame.makeMove(botVsHumanGame.getCurrentPosition()->getMoveFromNotation(buffer));
				if(validMove==false)
					std::cout << "Invalid move. Type 'q' to quit or enter valid move.\n";
			} while(validMove==false);
		}
	}

	int8_t gameState = botVsHumanGame.getGameState();
	switch(gameState)
	{
		case 0:
			printf("Error 22\n");
			break;
		case 1:
			printf("Draw by 50 move rule\n");
			break;
		case 2:
			printf("Draw by stalemate.\n");
			break;
		case 3:
			printf("Draw by insufficient material.\n");
			break;
		case 4:
			printf("White win.\n");
			break;
		case 5:
			printf("Black win.\n");
			break;
		case 6:
			printf("Draw by threefold repetition.\n");
			break;
		default:
			printf("Error 36\n");
			break;
	}
	delete[] buffer;
}
void CLI::command_humanvshuman()
{
    if(commandTokens.size()!=0)
    {
        printHelp("humanvshuman");
        return;
    }
	bool validMove;
	char* buffer = new char[20];
	Game humanVsHumanGame = Game();
    while(humanVsHumanGame.getGameState()==0)
    {
        humanVsHumanGame.printBoard();
        do
        {
            fgets(buffer, sizeof(buffer), stdin);
            if(buffer[0]=='q' && buffer[1]=='\n')
            {
                delete[] buffer;
                return;
            }
            validMove = humanVsHumanGame.makeMove(humanVsHumanGame.getCurrentPosition()->getMoveFromNotation(buffer));
            if(validMove==false)
                std::cout << "Invalid move. Type 'q' to quit. Type 'q' to quit or enter valid move.\n";
        } while(validMove==false);
    }

	int8_t gameState = humanVsHumanGame.getGameState();
	switch(gameState)
	{
		case 0:
			printf("Error 22\n");
			break;
		case 1:
			printf("Draw by 50 move rule\n");
			break;
		case 2:
			printf("Draw by stalemate.\n");
			break;
		case 3:
			printf("Draw by insufficient material.\n");
			break;
		case 4:
			printf("White win.\n");
			break;
		case 5:
			printf("Black win.\n");
			break;
		case 6:
			printf("Draw by threefold repetition.\n");
			break;
		default:
			printf("Error 36\n");
			break;
	}
	delete[] buffer;
}
void CLI::command_help()
{
    printHelp();
}

void CLI::printHelp()
{
    printf("--Bluespiral Help--\n");
    printf("uci:\tTells the engine to switch to uci (universal chess interface) mode.\n");
    printf("botvsbot:\tRuns a bot vs bot game printing each move in standard chess notation.\n");
    printf("humanvsbot [<human color>]:\tRuns a human vs bot game printing the board after every bot move. If no color is provided a random one is selected. Valid Colors: w, b\n");
    printf("humanvshuman:\tRuns a human vs human game printing the board after every move.\n");
    printf("help [command]:\tProvides detailed information about each command.\n");
}
void CLI::printHelp(std::string command)
{
    printf("WIP\n");
}
