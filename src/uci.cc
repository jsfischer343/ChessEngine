#include "uci.hh"


UCI::UCI()
{
	out_id();
	out_sendOptions();
	out_uciok();
    uciStates.isready = true;
}
UCI::~UCI()
{
    if(goThread!=NULL)
    {
        if(goThread->joinable())
        {
            goThread->join();
        }
        delete goThread;
    }
    if(uciPositionTree!=NULL)
    {
        delete uciPositionTree;
    }
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
    commandTokens.clear(); //reset commandTokens from last command

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
    if(commandTokens.size()>=MAX_COMMAND_TOKENS)
    {
        std::cerr << "error138: max allowed commandTokens exceeded" << std::endl;
        return;
    }

    if(command=="debug")
    {
        in_debug();
    }
    else if(command=="isready")
    {
        in_isready();
    }
    else if(command=="setoption")
    {
        in_setoption();
    }
    else if(command=="register")
    {
        in_register();
    }
    else if(command=="ucinewgame")
    {
        in_ucinewgame();
    }
    else if(command=="position")
    {
        in_position();
    }
    else if(command=="go")
    {
        in_go();
    }
    else if(command=="stop")
    {
        in_stop();
    }
    else if(command=="ponderhit")
    {
        in_ponderhit();
    }
    else
    {
        std::cerr << "error180: command not recognized" << std::endl;
    }
}

//UCI commands
void UCI::in_debug()
{
    if(commandTokens.size()==1)
    {
        if(commandTokens.at(0)=="on")
        {
            uciStates.debug = true;
        }
        else if(commandTokens.at(0)=="off")
        {
            uciStates.debug = false;
        }
    }
}
void UCI::in_isready()
{
    if(uciStates.isready)
    {
        std::cout << "readyok" << std::endl;
    }
}
void UCI::in_setoption()
{
    if(commandTokens.size()==0)
    {
        std::cerr << "error214: no arguments provided" << std::endl;
        return;
    }
    std::string optionName = "";
    std::string optionValue = "";
    if(commandTokens.at(0)!="name")
    {
        std::cerr << "error195: setoption 'name' signifier not found" << std::endl;
        return;
    }
    int i=1;
    while(i<commandTokens.size())
    {
        if(commandTokens.at(i)=="value")
        {
            break;
        }
        for(int j=0;j<commandTokens.at(i).size();j++)
        {
            commandTokens.at(i).at(j) = std::tolower(commandTokens.at(i).at(j));
        }
        i++;
    }
    for(int j=1;j<i;j++)
    {
        optionName += commandTokens.at(1);
    }
    if(optionName=="")
    {
        std::cerr << "error243: setoption no option name provided" << std::endl;
        return;
    }
    if(i>=commandTokens.size())
    {
        std::cerr << "error208: setoption 'value' signifier not found" << std::endl;
        return;
    }
    else if((i+1)==commandTokens.size())
    {
        std::cerr << "error212: setoption no value after 'value' was given" << std::endl;
        return;
    }
    else if((i+2)<commandTokens.size())
    {
        std::cerr << "error216: setoption has to many values given after 'value'" << std::endl;
        return;
    }
    else
    {
        optionValue = commandTokens.at(i+1);
    }

    //Options
    if(optionName=="dummy")
    {
        uciOptions.dummy = optionValue;
    }
}
void UCI::in_register()
{
    //unused
    std::cerr << "error250: this engine does not support registration" << std::endl;
}
void UCI::in_ucinewgame()
{
    uciGoParams = ucigoparams();
    if(uciPositionTree!=NULL)
    {
        delete uciPositionTree;
    }
}
void UCI::in_position_makeMoves(int startingCommandTokenIndex)
{
    if(commandTokens.at(startingCommandTokenIndex)!="moves")
    {
        std::cerr << "error279: position command 'moves' signifier missing" << std::endl;
        return;
    }
    else
    {
        for(int i=startingCommandTokenIndex+1;i<commandTokens.size();i++)
        {
            if(!(uciPositionTree->makeMove(uciNotation_TO_move(commandTokens.at(i)))))
            {
                std::cerr << "error287: invalid move was processed" << std::endl;
            }
            else
            {
                uciPositionTree->expandFromRoot(2);
            }
        }
    }
}
void UCI::in_position()
{
    if(commandTokens.size()==0)
    {
        return;
    }
    if(commandTokens.at(0)=="fen")
    {
        if(commandTokens.size()<7)
        {
            std::cerr << "error313: insufficient arguments to create fen string" << std::endl;
            return;
        }
        std::string fenString = "";
        std::regex fenRegex("(([12345678prnbqkPRNBQK]{1,8}/){7}[12345678prnbqkPRNBQK]{1,8}) [wb] ((K{0,1}Q{0,1}k{0,1}q{0,1})|-) (([a-h]{1}[1-8]{1})|-) [0-9]{1,4} [0-9]{1,4}");
        fenString += commandTokens.at(1); //board
        fenString += " ";
        fenString += commandTokens.at(2); //color to move
        fenString += " ";
        fenString += commandTokens.at(3); //castling
        fenString += " ";
        fenString += commandTokens.at(4); //en passant
        fenString += " ";
        fenString += commandTokens.at(5); //50 move counter
        fenString += " ";
        fenString += commandTokens.at(6); //move counter
        if(!(std::regex_match(fenString,fenRegex)))
        {
            std::cerr << "error277: invalid fen string" << std::endl;
            return;
        }
        else
        {
            setupPositionTree(fenString);
        }
        if(commandTokens.size()<8)
        {
            return; //implies that no moves were provided
        }
        else
        {
            in_position_makeMoves(7);
        }
    }
    else if(commandTokens.at(0)=="startpos")
    {
        setupPositionTree();
        if(commandTokens.size()==1)
        {
            return; //implies that no moves were provided
        }
        else
        {
            in_position_makeMoves(1);
        }
    }
    else
    {
        in_position_makeMoves(0);
    }
}
bool UCI::in_go_isGoCommand(std::string command)
{
    if(command=="searchmoves")
    {
        return true;
    }
    else if(command=="ponder")
    {
        return true;
    }
    else if(command=="wtime")
    {
        return true;
    }
    else if(command=="btime")
    {
        return true;
    }
    else if(command=="winc")
    {
        return true;
    }
    else if(command=="binc")
    {
        return true;
    }
    else if(command=="movestogo")
    {
        return true;
    }
    else if(command=="depth")
    {
        return true;
    }
    else if(command=="nodes")
    {
        return true;
    }
    else if(command=="mate")
    {
        return true;
    }
    else if(command=="movetime")
    {
        return true;
    }
    else if(command=="infinite")
    {
        return true;
    }
    return false;
}
void UCI::in_go_parse()
{
    if(uciPositionTree==NULL)
    {
        std::cerr << "error424: position hasn't been initialized" << std::endl;
    }
    int searchMovesIndex = -1;
    int ponderIndex = -1;
    int wTimeIndex = -1;
    int bTimeIndex = -1;
    int wIncIndex = -1;
    int bIncIndex = -1;
    int movesToGoIndex = -1;
    int depthIndex = -1;
    int nodesIndex = -1;
    int mateIndex = -1;
    int moveTimeIndex = -1;
    int infiniteIndex = -1;
    for(int i=0;i<commandTokens.size();i++) //look to see if each of the sub commands are present or not
    {
        if(commandTokens.at(i)=="searchmoves")
        {
            if(searchMovesIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                searchMovesIndex = i;
            }
        }
        else if(commandTokens.at(i)=="ponder")
        {
            if(ponderIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                ponderIndex = i;
            }
        }
        else if(commandTokens.at(i)=="wtime")
        {
            if(wTimeIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                wTimeIndex = i;
            }
        }
        else if(commandTokens.at(i)=="btime")
        {
            if(bTimeIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                bTimeIndex = i;
            }
        }
        else if(commandTokens.at(i)=="winc")
        {
            if(wIncIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                wIncIndex = i;
            }
        }
        else if(commandTokens.at(i)=="binc")
        {
            if(bIncIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                bIncIndex = i;
            }
        }
        else if(commandTokens.at(i)=="movestogo")
        {
            if(movesToGoIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                movesToGoIndex = i;
            }
        }
        else if(commandTokens.at(i)=="depth")
        {
            if(depthIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                depthIndex = i;
            }
        }
        else if(commandTokens.at(i)=="nodes")
        {
            if(nodesIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                nodesIndex = i;
            }
        }
        else if(commandTokens.at(i)=="mate")
        {
            if(mateIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                mateIndex = i;
            }
        }
        else if(commandTokens.at(i)=="movetime")
        {
            if(moveTimeIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                moveTimeIndex = i;
            }
        }
        else if(commandTokens.at(i)=="infinite")
        {
            if(infiniteIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
            }
            else
            {
                infiniteIndex = i;
            }
        }
    }

    //if the command is present and syntax is correct then update uciGoParams with associated value
    if(searchMovesIndex!=-1)
    {
        int i=1;
        while(searchMovesIndex+i<commandTokens.size())
        {
            if(in_go_isGoCommand(commandTokens.at(searchMovesIndex+i))) //terminate loop when next token is a recognized sub command
            {
                break;
            }
            i++;
        }
        std::regex uciMoveRegexPattern("[abcdefgh][1-8][abcdefgh][1-8][qrbn]{0,1}");
        for(int j=searchMovesIndex;j<searchMovesIndex+i;j++)
        {
            if(!(std::regex_match(commandTokens.at(j),uciMoveRegexPattern)))
            {
                std::cerr << "error545: invalid move syntax" << std::endl;
                break;
            }
            uciGoParams.searchMoves.push_back(uciNotation_TO_move(commandTokens.at(j)));
        }
    }
    if(ponderIndex!=-1)
    {
        uciGoParams.ponder = true;
    }
    std::regex integerTypeRegex("[0-9]{1,9}");
    std::regex longTypeRegex("[0-9]{1,17}");
    if(wTimeIndex!=-1 && wTimeIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(wTimeIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(wTimeIndex+1),longTypeRegex)))
            {
                std::cerr << "error563: invalid time argument" << std::endl;
            }
            else
            {
                uciGoParams.wTime = std::stol(commandTokens.at(wTimeIndex+1));
            }
        }
    }
    if(bTimeIndex!=-1 && bTimeIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(bTimeIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(bTimeIndex+1),longTypeRegex)))
            {
                std::cerr << "error563: invalid time argument" << std::endl;
            }
            else
            {
                uciGoParams.bTime = std::stol(commandTokens.at(bTimeIndex+1));
            }
        }
    }
    if(wIncIndex!=-1 && wIncIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(wIncIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(wIncIndex+1),longTypeRegex)))
            {
                std::cerr << "error563: invalid time argument" << std::endl;
            }
            else
            {
                uciGoParams.wInc = std::stol(commandTokens.at(wIncIndex+1));
            }
        }
    }
    if(bIncIndex!=-1 && bIncIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(bIncIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(bIncIndex+1),longTypeRegex)))
            {
                std::cerr << "error563: invalid time argument" << std::endl;
            }
            else
            {
                uciGoParams.bInc = std::stol(commandTokens.at(bIncIndex+1));
            }
        }
    }
    if(movesToGoIndex!=-1 && movesToGoIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(movesToGoIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(movesToGoIndex+1),integerTypeRegex)))
            {
                std::cerr << "error661: invalid moves argument" << std::endl;
            }
            else
            {
                uciGoParams.movesToGo = std::stoi(commandTokens.at(movesToGoIndex+1));
            }
        }
    }
    if(depthIndex!=-1 && depthIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(depthIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(depthIndex+1),integerTypeRegex)))
            {
                std::cerr << "error633: invalid depth argument" << std::endl;
            }
            else
            {
                uciGoParams.depth = std::stoi(commandTokens.at(depthIndex+1));
            }
        }
    }
    if(nodesIndex!=-1 && nodesIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(nodesIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(nodesIndex+1),integerTypeRegex)))
            {
                std::cerr << "error647: invalid nodes argument" << std::endl;
            }
            else
            {
                uciGoParams.nodes = std::stoi(commandTokens.at(nodesIndex+1));
            }
        }
    }
    if(mateIndex!=-1 && mateIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(mateIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(mateIndex+1),integerTypeRegex)))
            {
                std::cerr << "error661: invalid move argument" << std::endl;
            }
            else
            {
                uciGoParams.mate = std::stoi(commandTokens.at(mateIndex+1));
            }
        }
    }
    if(moveTimeIndex!=-1 && moveTimeIndex+1<commandTokens.size())
    {
        if(!(in_go_isGoCommand(commandTokens.at(moveTimeIndex+1))))
        {
            if(!(std::regex_match(commandTokens.at(moveTimeIndex+1),longTypeRegex)))
            {
                std::cerr << "error563: invalid time argument" << std::endl;
            }
            else
            {
                uciGoParams.moveTime = std::stol(commandTokens.at(moveTimeIndex+1));
            }
        }
    }
    if(infiniteIndex!=-1)
    {
        uciGoParams.infinite = true;
    }
}
bool UCI::in_go_searchThread_shouldStop()
{
    if(uciStates.stop==true)
    {
        return true;
    }
    if(uciGoParams.infinite == false)
    {
        if(uciGoParams.moveTime!=-1)
        {
            std::chrono::duration duration = std::chrono::steady_clock::now() - goThread_startTime;
            std::chrono::duration duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
            if((uciGoParams.moveTime-SEARCH_TIME_MARGIN)<duration_milliseconds.count())
            {
                return true;
            }
        }
    }
    return false;
}
void UCI::in_go_searchThread()
{
    while(!in_go_searchThread_shouldStop())
    {
        uciPositionTree->expandNextBestBranch();
    }
    out_bestmove(uciPositionTree->getBestMove());
    uciStates.isready = true;
}
void UCI::in_go()
{
    in_go_parse();
    uciStates.stop = false;
    if(goThread!=NULL)
    {
        if(goThread->joinable())
        {
            goThread->join();
        }
        delete goThread;
    }
    uciStates.isready = false;
    goThread_startTime = std::chrono::steady_clock::now();
    goThread = new std::thread(&UCI::in_go_searchThread, this);
}
void UCI::in_stop()
{
    uciStates.stop = true;
}
void UCI::in_ponderhit()
{
    //???
    std::cerr << "error716: command not implemented" << std::endl;
}

void UCI::out_id()
{
    std::cout << "id name BlueSpiral " << GLOBAL_BLUESPIRAL_VERSION << std::endl;
	std::cout << "id author " << GLOBAL_AUTHOR << std::endl;
}
void UCI::out_uciok()
{
    std::cout << "uciok" << std::endl;
}
void UCI::out_readyok()
{
    std::cout << "readyok" << std::endl;
}
void UCI::out_bestmove(move bestMove)
{
    std::string uciNotationBestMove = move_TO_uciNotation(bestMove);
    std::cout << "bestmove " << uciNotationBestMove << std::endl;
}
// void UCI::out_copyprotection()
// {
//     //not used
// }
// void UCI::out_registration()
// {
//     //not used
// }
void UCI::out_info()
{
}
void UCI::out_sendOptions()
{
    std::cout << "option name dummy type string default test123" << std::endl;
}

//Utility
move UCI::uciNotation_TO_move(std::string uciMoveString)
{
    move tempMove = move();
    if(uciMoveString=="0000")
    {
        return tempMove;
    }
    else
    {
        tempMove.startFile = Position::notationFile_TO_engineFile(uciMoveString[0]);
        tempMove.startRank = Position::notationRank_TO_engineRank(uciMoveString[1]);
        tempMove.endFile = Position::notationFile_TO_engineFile(uciMoveString[2]);
        tempMove.endRank = Position::notationRank_TO_engineRank(uciMoveString[3]);
        if(uciMoveString.size()==5)
        {
            tempMove.endPieceType = uciMoveString.at(4);
        }
        else
        {
            if(uciPositionTree->getCurrentPosition()->theBoard[tempMove.startRank][tempMove.startFile].piecePtr==NULL)
            {
                tempMove.endPieceType='\0';
            }
            else
            {
                tempMove.endPieceType=uciPositionTree->getCurrentPosition()->theBoard[tempMove.startRank][tempMove.startFile].piecePtr->type;
            }
        }
        return tempMove;
    }
}
std::string UCI::move_TO_uciNotation(move engineMove)
{
    std::string uciMove;
    if(engineMove.startRank==-1 &&
    engineMove.startFile==-1 &&
    engineMove.endRank==-1 &&
    engineMove.endFile==-1 &&
    engineMove.endPieceType=='\0')
    {
        uciMove = "0000";
    }
    else
    {
		uciMove.push_back(Position::engineFile_TO_notationFile(engineMove.startFile));
        uciMove.push_back(Position::engineRank_TO_notationRank(engineMove.startRank));
        uciMove.push_back(Position::engineFile_TO_notationFile(engineMove.endFile));
        uciMove.push_back(Position::engineRank_TO_notationRank(engineMove.endRank));
        if(uciPositionTree->getCurrentPosition()->theBoard[engineMove.startRank][engineMove.startFile].piecePtr!=NULL)
        {
            if(uciPositionTree->getCurrentPosition()->theBoard[engineMove.startRank][engineMove.startFile].piecePtr->type!=engineMove.endPieceType) //if start piece type and end piece type are different then this is probably a promotion
            {
                uciMove.push_back(engineMove.endPieceType);
            }
        }
    }
    return uciMove;
}
void UCI::setupPositionTree()
{
    if(uciPositionTree!=NULL)
    {
        delete uciPositionTree;
    }
    uciPositionTree = new PositionTree(new Position(),2);
}
void UCI::setupPositionTree(std::string fenString)
{
    if(uciPositionTree!=NULL)
    {
        delete uciPositionTree;
    }
    uciPositionTree = new PositionTree(new Position(fenString.c_str()),2);
}
