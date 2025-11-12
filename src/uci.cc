#include "uci.hh"


UCI::UCI()
{
}
UCI::~UCI()
{
}

bool UCI::parseCommand_validCommand(std::string command)
{
    if(command=="debug")
    {
        return true;
    }
    else if(command=="isready")
    {
        return true;
    }
    else if(command=="setoption")
    {
        return true;
    }
    else if(command=="register")
    {
        return true;
    }
    else if(command=="ucinewgame")
    {
        return true;
    }
    else if(command=="position")
    {
        return true;
    }
    else if(command=="go")
    {
        return true;
    }
    else if(command=="stop")
    {
        return true;
    }
    else if(command=="ponderhit")
    {
        return true;
    }
    else if(command=="quit")
    {
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

void UCI::parseCommand(std::string userInput)
{
    tokens.clear(); //reset tokens from last command

    int tokenStart = 0;
    int tokenEnd = 0;
    std::string command;
    while(true)
    {
        //find start of command token
        if(userInput[0]=='\0')
        {
            return;
        }
        tokenStart=0;
        while(userInput[tokenStart]==' '||userInput[tokenStart]=='\t')
        {
            if(userInput[tokenStart]=='\0')
            {
                return;
            }
            tokenStart++;
        }

        //find end of command token
        tokenEnd=tokenStart;
        do {
            tokenEnd++;
        } while(userInput[tokenEnd]!=' '&&userInput[tokenEnd]!='\t'&&userInput[tokenEnd]!='\0'&&tokenEnd<userInput.length());

        //save command
        command = userInput.substr(tokenStart,tokenEnd-tokenStart);
        userInput.erase(0,tokenEnd);
        if(parseCommand_validCommand(command))
        {
            break;
        }
    }

    //save tokens (i.g. arguments)
    bool dontPushToken = false;
    bool moreTokens = true;
    while(moreTokens&&tokens.size()<=MAX_COMMAND_TOKENS)
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
            tokens.push_back(userInput.substr(tokenStart,tokenEnd-tokenStart));
            userInput.erase(0,tokenEnd);
        }
    }

    printf("debug: ");
    std::cout << command;
    for(int i=0;i<tokens.size();i++)
    {
        std::cout << "\t" << tokens.at(i);
    }
    printf("\n");
}

//UCI commands
void UCI::command_GUItoEngine_debug()
{
}
void UCI::command_GUItoEngine_isready()
{
}
void UCI::command_GUItoEngine_setoption()
{
}
void UCI::command_GUItoEngine_register()
{
}
void UCI::command_GUItoEngine_ucinewgame()
{
}
void UCI::command_GUItoEngine_position()
{
}
void UCI::command_GUItoEngine_go()
{
}
void UCI::command_GUItoEngine_stop()
{
}
void UCI::command_GUItoEngine_ponderhit()
{
}
void UCI::command_GUItoEngine_quit()
{
}

void UCI::command_EnginetoGUI_id()
{
}
void UCI::command_EnginetoGUI_uciok()
{
}
void UCI::command_EnginetoGUI_readyok()
{
}
void UCI::command_EnginetoGUI_bestmove()
{
}
void UCI::command_EnginetoGUI_copyprotection()
{
}
void UCI::command_EnginetoGUI_registration()
{
}
void UCI::command_EnginetoGUI_info()
{
}
void UCI::command_EnginetoGUI_option()
{
}
