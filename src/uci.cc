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
    uciStates.stop = true; //signal thread to stop
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
    else if(command=="debugdump")
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
    if(userInput=="exit"||userInput=="quit")
    {
        uciStates.stop = true;
        return;
    }
    inputTokens.clear(); //reset inputTokens from last command

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
	while(true) //while inputTokens.at(0) is not a recongizable command delete it
    {
        if(inputTokens.size()==0)
        {
            std::cerr << "error180: command not recognized" << std::endl;
            return;
        }
        if(parseCommand_validCommand(inputTokens.at(0)))
        {
            break;
        }
        inputTokens.erase(inputTokens.begin());
    }

    if(inputTokens.at(0)=="debug")
    {
        in_debug();
    }
    else if(inputTokens.at(0)=="isready")
    {
        in_isready();
    }
    else if(inputTokens.at(0)=="setoption")
    {
        in_setoption();
    }
    else if(inputTokens.at(0)=="register")
    {
        in_register();
    }
    else if(inputTokens.at(0)=="ucinewgame")
    {
        in_ucinewgame();
    }
    else if(inputTokens.at(0)=="position")
    {
        in_position();
    }
    else if(inputTokens.at(0)=="go")
    {
        in_go();
    }
    else if(inputTokens.at(0)=="stop")
    {
        in_stop();
    }
    else if(inputTokens.at(0)=="ponderhit")
    {
        in_ponderhit();
    }
    else if(inputTokens.at(0)=="debugdump")
    {
        in_debugdump();
    }
    else
    {
        std::cerr << "error180: command not recognized" << std::endl;
    }
}

//UCI commands
void UCI::in_debug()
{
    if(inputTokens.size()==2)
    {
        if(inputTokens.at(1)=="on")
        {
            uciStates.debug = true;
        }
        else if(inputTokens.at(1)=="off")
        {
            uciStates.debug = false;
        }
        else
        {
            std::cerr << "error169: invalid debug option" << std::endl;
        }
    }
}
void UCI::in_isready()
{
    while(uciStates.isready==false);
    out_readyok();
}
void UCI::in_setoption()
{
    if(uciStates.isready==true)
    {
        if(inputTokens.size()==1)
        {
            std::cerr << "error214: no arguments provided" << std::endl;
            return;
        }
        if(inputTokens.at(1)!="name")
        {
            std::cerr << "error195: setoption 'name' signifier not found" << std::endl;
            return;
        }

        std::string optionName = "";
        std::string optionValue = "";
        int i=2;
        while(i<inputTokens.size())
        {
            if(inputTokens.at(i)=="value")
            {
                break;
            }
            for(int j=0;j<inputTokens.at(i).size();j++)
            {
                inputTokens.at(i).at(j) = std::tolower(inputTokens.at(i).at(j));
            }
            i++;
        }
        for(int j=2;j<i;j++)
        {
            optionName += inputTokens.at(j);
        }
        if(optionName=="")
        {
            std::cerr << "error243: setoption no option name provided" << std::endl;
            return;
        }
        if(i>=inputTokens.size())
        {
            std::cerr << "error208: setoption 'value' signifier not found" << std::endl;
            return;
        }
        else if((i+1)>inputTokens.size())
        {
            std::cerr << "error212: setoption no value after 'value' was given" << std::endl;
            return;
        }
        else if((i+2)<inputTokens.size())
        {
            std::cerr << "error216: setoption has to many values given after 'value'" << std::endl;
            return;
        }
        else
        {
            optionValue = inputTokens.at(i+1);
        }

        //Options
        if(optionName=="dummy")
        {
            uciOptions.dummy = optionValue;
        }
    }
    else
    {
        std::cerr << "error288: engine is not ready" << std::endl;
    }
}
void UCI::in_register()
{
    //unused
    std::cerr << "error250: this engine does not support registration" << std::endl;
}
void UCI::in_ucinewgame()
{
    if(uciStates.isready==true)
    {
        if(uciPositionTree!=NULL)
        {
            delete uciPositionTree;
            uciPositionTree=NULL;
        }
    }
    previousMoves.clear();
}
bool UCI::in_position_validateMoveVectorAgainstPreviousMoves()
{
    if((inputTokens.size()-3)<previousMoves.size())
    {
        return false;
    }
    for(int i=3;i<previousMoves.size()+3;i++)
    {
        if(inputTokens.at(i)!=previousMoves.at(i-3))
        {
            return false;
        }
    }
    return true;
}
void UCI::in_position_makeMoves(int startingTokenIndex)
{
    if(startingTokenIndex==3)
    {
        previousMoves.clear();
    }
    for(int i=startingTokenIndex;i<inputTokens.size();i++)
    {
        if(!(uciPositionTree->makeMove(uciNotation_TO_move(inputTokens.at(i)))))
        {
            std::cerr << "error287: invalid move was processed" << std::endl;
        }
        else
        {
            previousMoves.push_back(inputTokens.at(i));
        }

    }
}
void UCI::in_position()
{
    if(uciStates.isready==true)
    {
        if(inputTokens.size()==1)
        {
            std::cerr << "error279: no starting position provided -> [startpos | <fenstring>]" << std::endl;
            return;
        }
        if(inputTokens.at(1)=="fen")
        {
            if(inputTokens.size()<8)
            {
                std::cerr << "error313: malformed position command" << std::endl;
                return;
            }
            std::string fenString = "";
            std::regex fenRegex("(([12345678prnbqkPRNBQK]{1,8}/){7}[12345678prnbqkPRNBQK]{1,8}) [wb] ((K{0,1}Q{0,1}k{0,1}q{0,1})|-) (([a-h]{1}[1-8]{1})|-) [0-9]{1,4} [0-9]{1,4}");
            fenString += inputTokens.at(2); //board
            fenString += " ";
            fenString += inputTokens.at(3); //color to move
            fenString += " ";
            fenString += inputTokens.at(4); //castling
            fenString += " ";
            fenString += inputTokens.at(5); //en passant
            fenString += " ";
            fenString += inputTokens.at(6); //50 move counter
            fenString += " ";
            fenString += inputTokens.at(7); //move counter
            if(!(std::regex_match(fenString,fenRegex)))
            {
                std::cerr << "error277: invalid fen string" << std::endl;
                return;
            }
            if(inputTokens.size()<9) //implies that no moves were provided
            {
                setupPositionTree(fenString);
            }
            else
            {
                if(inputTokens.at(8)!="moves")
                {
                    std::cerr << "error279: position command 'moves' signifier missing or malformed" << std::endl;
                    return;
                }
                if(uciPositionTree==NULL || fenString!=lastStartingFEN)
                {
                    setupPositionTree(fenString);
                    in_position_makeMoves(9);
                }
                else
                {
                    if(!in_position_validateMoveVectorAgainstPreviousMoves())
                    {
                        setupPositionTree(fenString);
                        in_position_makeMoves(9);
                    }
                    else
                    {
                        in_position_makeMoves(previousMoves.size()+9);
                    }
                }
            }
            lastStartingFEN = fenString;
        }
        else if(inputTokens.at(1)=="startpos")
        {
            if(inputTokens.size()==2) //implies that no moves were provided
            {
                setupPositionTree();
            }
            else
            {
                if(inputTokens.at(2)!="moves")
                {
                    std::cerr << "error279: position command 'moves' signifier missing or malformed" << std::endl;
                    return;
                }
                if(uciPositionTree==NULL)
                {
                    setupPositionTree();
                    in_position_makeMoves(3);
                }
                else
                {
                    if(!in_position_validateMoveVectorAgainstPreviousMoves())
                    {
                        setupPositionTree();
                        in_position_makeMoves(3);
                    }
                    else
                    {
                        in_position_makeMoves(previousMoves.size()+3);
                    }
                }
            }
        }
        else
        {
            std::cerr << "error387: missing fen or startpos keywords" << std::endl;
        }
    }
    else
    {
        std::cerr << "error288: engine is not ready" << std::endl;
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
bool UCI::in_go_parse()
{
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
    for(int i=1;i<inputTokens.size();i++) //look to see if each of the sub commands are present or not
    {
        if(inputTokens.at(i)=="searchmoves")
        {
            if(searchMovesIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                searchMovesIndex = i;
            }
        }
        else if(inputTokens.at(i)=="ponder")
        {
            if(ponderIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                ponderIndex = i;
            }
        }
        else if(inputTokens.at(i)=="wtime")
        {
            if(wTimeIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                wTimeIndex = i;
            }
        }
        else if(inputTokens.at(i)=="btime")
        {
            if(bTimeIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                bTimeIndex = i;
            }
        }
        else if(inputTokens.at(i)=="winc")
        {
            if(wIncIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                wIncIndex = i;
            }
        }
        else if(inputTokens.at(i)=="binc")
        {
            if(bIncIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                bIncIndex = i;
            }
        }
        else if(inputTokens.at(i)=="movestogo")
        {
            if(movesToGoIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                movesToGoIndex = i;
            }
        }
        else if(inputTokens.at(i)=="depth")
        {
            if(depthIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                depthIndex = i;
            }
        }
        else if(inputTokens.at(i)=="nodes")
        {
            if(nodesIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                nodesIndex = i;
            }
        }
        else if(inputTokens.at(i)=="mate")
        {
            if(mateIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                mateIndex = i;
            }
        }
        else if(inputTokens.at(i)=="movetime")
        {
            if(moveTimeIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
            }
            else
            {
                moveTimeIndex = i;
            }
        }
        else if(inputTokens.at(i)=="infinite")
        {
            if(infiniteIndex!=-1)
            {
                std::cerr << "error354: this command already is mentioned in go command" << std::endl;
                return false;
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
        std::regex uciMoveRegexPattern("[abcdefgh][1-8][abcdefgh][1-8][qrbn]{0,1}");
        while(searchMovesIndex+i<inputTokens.size())
        {
            if(!(in_go_isGoCommand(inputTokens.at(searchMovesIndex+i)))) //terminate loop when next token is a recognized sub command
            {
                break;
            }
            if(!std::regex_match(inputTokens.at(i),uciMoveRegexPattern))
            {
                std::cerr << "error545: invalid move syntax" << std::endl;
                return false;
                break;
            }
            uciGoParams.searchMoves.push_back(uciNotation_TO_move(inputTokens.at(i)));
            i++;
        }
    }
    if(ponderIndex!=-1)
    {
        uciGoParams.ponder = true;
    }
    std::regex integerTypeRegex("[0-9]{1,9}");
    std::regex longTypeRegex("[0-9]{1,17}");
    if(wTimeIndex!=-1 && wTimeIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(wTimeIndex+1))))
        {
            if(std::regex_match(inputTokens.at(wTimeIndex+1),longTypeRegex))
            {
                uciGoParams.wTime = std::stol(inputTokens.at(wTimeIndex+1));
            }
            else
            {
                std::cerr << "error563: invalid time argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error649: not time argument provided" << std::endl;
			return false;
        }
    }
    if(bTimeIndex!=-1 && bTimeIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(bTimeIndex+1))))
        {
            if(std::regex_match(inputTokens.at(bTimeIndex+1),longTypeRegex))
            {
                uciGoParams.bTime = std::stol(inputTokens.at(bTimeIndex+1));
            }
            else
            {
                std::cerr << "error563: invalid time argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error649: not time argument provided" << std::endl;
			return false;
        }
    }
    if(wIncIndex!=-1 && wIncIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(wIncIndex+1))))
        {
            if(std::regex_match(inputTokens.at(wIncIndex+1),longTypeRegex))
            {
                uciGoParams.wInc = std::stol(inputTokens.at(wIncIndex+1));
            }
            else
            {
                std::cerr << "error563: invalid time argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error649: not time argument provided" << std::endl;
			return false;
        }
    }
    if(bIncIndex!=-1 && bIncIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(bIncIndex+1))))
        {
            if(std::regex_match(inputTokens.at(bIncIndex+1),longTypeRegex))
            {
                uciGoParams.bInc = std::stol(inputTokens.at(bIncIndex+1));
            }
            else
            {
                std::cerr << "error563: invalid time argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error649: not time argument provided" << std::endl;
			return false;
        }
    }
    if(movesToGoIndex!=-1 && movesToGoIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(movesToGoIndex+1))))
        {
            if(std::regex_match(inputTokens.at(movesToGoIndex+1),integerTypeRegex))
            {
                uciGoParams.movesToGo = std::stoi(inputTokens.at(movesToGoIndex+1));
            }
            else
            {
                std::cerr << "error722: invalid movestogo argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error721: no movestogo argument provided" << std::endl;
			return false;
        }
    }
    if(depthIndex!=-1 && depthIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(depthIndex+1))))
        {
            if(std::regex_match(inputTokens.at(depthIndex+1),integerTypeRegex))
            {
                uciGoParams.depth = std::stoi(inputTokens.at(depthIndex+1));
            }
            else
            {
                std::cerr << "error633: invalid depth argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error739: no depth argument provided" << std::endl;
			return false;
        }
    }
    if(nodesIndex!=-1 && nodesIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(nodesIndex+1))))
        {
            if(std::regex_match(inputTokens.at(nodesIndex+1),integerTypeRegex))
            {
                uciGoParams.nodes = std::stoi(inputTokens.at(nodesIndex+1));
            }
            else
            {
                std::cerr << "error647: invalid nodes argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error763: no nodes argument provided" << std::endl;
			return false;
        }
    }
    if(mateIndex!=-1 && mateIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(mateIndex+1))))
        {
            if(std::regex_match(inputTokens.at(mateIndex+1),integerTypeRegex))
            {
                uciGoParams.mate = std::stoi(inputTokens.at(mateIndex+1));
            }
            else
            {
                std::cerr << "error661: invalid mate argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error661: no mate argument provided" << std::endl;
			return false;
        }
    }
    if(moveTimeIndex!=-1 && moveTimeIndex+1<inputTokens.size())
    {
        if(!(in_go_isGoCommand(inputTokens.at(moveTimeIndex+1))))
        {
            if(std::regex_match(inputTokens.at(moveTimeIndex+1),longTypeRegex))
            {
                uciGoParams.moveTime = std::stol(inputTokens.at(moveTimeIndex+1));
            }
            else
            {
                std::cerr << "error563: invalid time argument" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "error649: no time argument provided" << std::endl;
			return false;
        }
    }
    if(infiniteIndex!=-1)
    {
        uciGoParams.infinite = true;
    }
    return true;
}
bool UCI::in_go_searchThread_shouldStop()
{
    if(uciStates.stop==true)
    {
        return true;
    }
    if(uciGoParams.infinite == false)
    {
        //TODO: implement time managment algorithm for use of wTime, bTime, increment, movesToGo
        //TODO: add logic to stop when nodes, depth
        //TODO: add logic for finding mate in x moves (if not found it needs to return null move 0000)
        //TODO: research the concept of ponder more so as to understand how to implement it correctly
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
    uciStates.isready = true; //update isready
}
void UCI::in_go()
{
    if(uciStates.isready==false)
    {
        std::cerr << "error288: engine is not ready" << std::endl;
        return;
    }
    if(uciPositionTree==NULL)
    {
        std::cerr << "error424: position hasn't been initialized" << std::endl;
        return;
    }
    if(goThread!=NULL)
    {
        if(goThread->joinable())
        {
            goThread->join();
        }
        delete goThread;
    }
    uciGoParams = ucigoparams();
    if(!in_go_parse())
        return;
    uciStates.isready = false;
    uciStates.stop = false;
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
void UCI::in_debugdump()
{
    uciPositionTree->printPositionTree(1);
    uciPositionTree->printTreeInfo();
    Position* debugPosition = uciPositionTree->getCurrentPosition();
    debugPosition->printBoard();
    debugPosition->printInfo();
    debugPosition->printInstantEvalBreakdown();
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
            tempMove.endPieceType = uciMoveString[4];
        }
        else
        {
            tempMove.endPieceType='\0';
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
        if(engineMove.endPieceType!='\0')
        {
            uciMove.push_back(engineMove.endPieceType);
        }
    }
    return uciMove;
}
void UCI::setupPositionTree()
{
    if(uciPositionTree!=NULL)
    {
        delete uciPositionTree;
        uciPositionTree=NULL;
    }
    uciPositionTree = new PositionTree(2);
}
void UCI::setupPositionTree(std::string fenString)
{
    if(uciPositionTree!=NULL)
    {
        delete uciPositionTree;
    }
    uciPositionTree = new PositionTree(fenString.c_str(),2);
}
