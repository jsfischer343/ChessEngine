#Author: John Fischer
#Program: BlueSpiral Chess Engine
#
#Notes: - top left square from white's perspective is 0,0 file increase left to right rank increase top to bottem
#       - 

from dataclasses import dataclass, astuple
import random
import re
import numpy as np
import math
import itertools
import time
import copy
import sys

#TODO:
#1. Work promotion into move capability and other methods
#2. Reassess eval calculation (need to add promotion metrics)
#3. Reduce computational cost of position object
#4. Need a better way to get move depth so that it is no O(30^n) big O of an exponential

##OBJECTS##
@dataclass
class Square:
    file: int
    rank: int

class Piece:
    def __init__(self,type,square):
        #piece position and type validation
        if type not in ['R','N','B','Q','P','r','n','b','q','p','K','k']:
            raise TypeError('A invalid piece type "'+type+'" was specifed during piece intialization.')
        if square.file not in range(0,8):
            raise TypeError('A invalid file "'+str(square.file)+'" was specifed during piece intialization.')
        if square.rank not in range(0,8):
            raise TypeError('A invalid rank "'+str(square.rank)+'" was specifed during piece intialization.')
        self.type=type
        self.square=square

class Move:
    def __init__(self,pieceStart,pieceEnd):
        self.pieceStart=pieceStart #piece in question
        self.pieceEnd=pieceEnd #square it is moving to

##GAME OBJECT##
class Game:
    moveHistory = []
    class TreeNode:
        def __init__(self,move,position,depth,parent):
            self.move=move
            self.position=position
            self.eval=round(position.instantEval(),2)
            self.depth=depth
            self.parent=parent
            self.children=[]
            
        def calculateBranchAverage(self):
            return (self.calculateBranchAverageRecursive(self)+self.eval)/2

        def calculateBranchAverageRecursive(self,node):
            if len(node.children)==0:
                return 0
            else:
                sum = 0
                for x in range(len(node.children)):
                    sum = sum+self.calculateBranchAverageRecursive(node.children[x])+node.children[x].eval
                return sum/len(node.children)
            
            
    def __init__(self,startingPosition):
        self.startingPosition=startingPosition
        self.currentPosition=startingPosition
        self.GS='in-play' #assume the game is in play
        #1. verify that the game hasn't reached a draw or checkmate
        self.verifyGameState()
        #2. create first evaluation tree and update stats
        self.moveTreeHead = self.TreeNode(None,self.currentPosition,0,None)
        self.bestMoveNode = None
        self.bestMove = None
        

    def verifyGameState(self):
        if self.currentPosition.GS == 'whiteWin':
            self.GS='whiteWin'
        elif self.currentPosition.GS == 'blackWin':
            self.GS='blackWin'
        elif self.currentPosition.GS == 'stalemate':
            self.GS='stalemate'
        elif self.currentPosition.GS == 'insufficient':
            self.GS='insufficient'
        elif self.currentPosition.GS == 'draw50':
            self.GS='draw50'
        else:
            self.GS='in-play'

    def getBestMove(self):
        #0. Do sanity checks: game state
        if self.currentPosition.CTM=='w':
            max = self.moveTreeHead.children[0]
            for x in range(len(self.moveTreeHead.children)):
                if self.moveTreeHead.children[x].calculateBranchAverage()>max.calculateBranchAverage():
                    max = self.moveTreeHead.children[x]
            return max.move
        if self.currentPosition.CTM=='b':
            minx = self.moveTreeHead.children[0]
            for x in range(len(self.moveTreeHead.children)):
                if self.moveTreeHead.children[x].calculateBranchAverage()>min.calculateBranchAverage():
                    min = self.moveTreeHead.children[x]
            return min.move
    
    def calculateBestMoveAndEval(self,depth):
        self.createMoveTree(self.moveTreeHead,depth)
        self.bestMoveNode = self.getBestMoveNode()
        if self.bestMoveNode is not None:
            self.bestMove = self.bestMoveNode.move
        else:
            self.bestMove = None
    
    def getBestMoveNode(self):
        #0. Do sanity checks: game state
        if self.currentPosition.CTM=='w' and len(self.moveTreeHead.children)!=0:
            max = self.moveTreeHead.children[0]
            for x in range(len(self.moveTreeHead.children)):
                if self.moveTreeHead.children[x].calculateBranchAverage()>max.calculateBranchAverage():
                    max = self.moveTreeHead.children[x]
            return max
        elif self.currentPosition.CTM=='b' and len(self.moveTreeHead.children)!=0:
            min = self.moveTreeHead.children[0]
            for x in range(len(self.moveTreeHead.children)):
                if self.moveTreeHead.children[x].calculateBranchAverage()>min.calculateBranchAverage():
                    min = self.moveTreeHead.children[x]
            return min
        else:
            return None

    def createMoveTree(self,node,depth):
        ##ITERATIVE PART##
        #list all legal moves in move format
        movesInThisPosition = []
        #Castling Moves
        if (node.position.inCheck('w')==False
        and len(node.position.dynamicBoard[5][7].blackTargeters)==0 and len(node.position.dynamicBoard[5][7].blackZombieTargeters)==0
        and len(node.position.dynamicBoard[6][7].blackTargeters)==0 and len(node.position.dynamicBoard[6][7].blackZombieTargeters)==0
        and node.position.board[5][7]=='.' and node.position.board[6][7]=='.'
        and node.position.WCK
        and node.position.CTM=='w'
        ):
            movesInThisPosition.append(Move(Piece('K',Square(4,7)),Piece('K',Square(6,7))))
        if (node.position.inCheck('w')==False
        and len(node.position.dynamicBoard[3][7].blackTargeters)==0 and len(node.position.dynamicBoard[3][7].blackZombieTargeters)==0
        and len(node.position.dynamicBoard[2][7].blackTargeters)==0 and len(node.position.dynamicBoard[2][7].blackZombieTargeters)==0
        and node.position.board[3][7]=='.' and node.position.board[2][7]=='.' and node.position.board[1][7]=='.'
        and node.position.WCQ
        and node.position.CTM=='w'
        ):
            movesInThisPosition.append(Move(Piece('K',Square(4,7)),Piece('K',Square(2,7))))
        if (node.position.inCheck('b')==False
        and len(node.position.dynamicBoard[5][0].whiteTargeters)==0 and len(node.position.dynamicBoard[5][0].whiteZombieTargeters)==0
        and len(node.position.dynamicBoard[6][0].whiteTargeters)==0 and len(node.position.dynamicBoard[6][0].whiteZombieTargeters)==0
        and node.position.board[5][0]=='.' and node.position.board[6][0]=='.'
        and node.position.BCK
        and node.position.CTM=='b'
        ):
            movesInThisPosition.append(Move(Piece('k',Square(4,0)),Piece('k',Square(6,0))))
        if (node.position.inCheck('b')==False
        and len(node.position.dynamicBoard[3][0].whiteTargeters)==0 and len(node.position.dynamicBoard[3][0].whiteZombieTargeters)==0
        and len(node.position.dynamicBoard[2][0].whiteTargeters)==0 and len(node.position.dynamicBoard[2][0].whiteZombieTargeters)==0
        and node.position.board[3][0]=='.' and node.position.board[2][0]=='.' and node.position.board[1][0]=='.'
        and node.position.BCQ
        and node.position.CTM=='b'
        ):
            movesInThisPosition.append(Move(Piece('k',Square(4,0)),Piece('k',Square(2,0))))
        #Moves by Square
        for x in range(0,8):
            for y in range(0,8):
                for h in range(len(node.position.dynamicBoard[x][y].nextMovePieces)):
                    pieceStartType = node.position.dynamicBoard[x][y].nextMovePieces[h].type
                    pieceStartFile = node.position.dynamicBoard[x][y].nextMovePieces[h].square.file
                    pieceStartRank = node.position.dynamicBoard[x][y].nextMovePieces[h].square.rank
                    pieceEndType = node.position.dynamicBoard[x][y].nextMovePieces[h].type
                    pieceEndFile = x
                    pieceEndRank = y
                    #Promotion Move? (if true then 4 'moves' need to be appended)
                    if pieceStartType=='P' and pieceStartRank==1:
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece('N',Square(pieceEndFile,pieceEndRank))))
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece('B',Square(pieceEndFile,pieceEndRank))))
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece('R',Square(pieceEndFile,pieceEndRank))))
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece('Q',Square(pieceEndFile,pieceEndRank))))
                    elif pieceStartType=='p' and pieceStartRank==6:
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece('n',Square(pieceEndFile,pieceEndRank))))
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece('b',Square(pieceEndFile,pieceEndRank))))
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece('r',Square(pieceEndFile,pieceEndRank))))
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece('q',Square(pieceEndFile,pieceEndRank))))
                    #Otherwise just append
                    else:
                        movesInThisPosition.append(Move(Piece(pieceStartType,Square(pieceStartFile,pieceStartRank)),Piece(pieceEndType,Square(pieceEndFile,pieceEndRank))))
        ##RECURSIVE PART##
        if node.depth==depth:
            #discontinue branching
            return
        else:
            for x in range(len(movesInThisPosition)):
                node.children.append(self.TreeNode(movesInThisPosition[x],node.position.move(movesInThisPosition[x]),node.depth+1,node)) #append all the possible moves as node children
            ##FILTER (TYPE 1.0)##
            #go through 'children moves' and filter out moves based on evaluation criteria (this will reduce computational load but may cause error)
            #numberOfFilterPasses=0
            #if node.depth+1<5:
            #    numberOfFilterPasses = node.depth+1
            #else:
            #    numberOfFilterPasses = 5
            #if node.position.CTM=='w' and len(node.children)!=0:
            #    for i in range(0,numberOfFilterPasses):
            #        sum = 0
            #        max = node.children[0]
            #        for x in range(len(node.children)):
            #            sum = sum+node.children[x].eval
            #            if node.children[x].eval>max.eval:
            #                max = node.children[x]
            #        average = sum/len(node.children)
            #        for x in range(len(node.children)-1,-1,-1):
            #            if node.children[x].eval<average and node.children[x]!=max:
            #                del node.children[x] #trim children moves that are less than the average (i.e. optimizing for white)
            #if node.position.CTM=='b' and len(node.children)!=0:
            #    for i in range(0,numberOfFilterPasses):
            #        sum = 0
            #        min = node.children[0]
            #        for x in range(len(node.children)):
            #            sum = sum+node.children[x].eval
            #            if node.children[x].eval<min.eval:
            #                min = node.children[x]
            #        average = sum/len(node.children)
            #        for x in range(len(node.children)-1,-1,-1):
            #            if node.children[x].eval>average and node.children[x]!=min:
            #                del node.children[x] #trim children moves that are less than the average (i.e. optimizing for black)
            ##FILTER (TYPE 2.0)##
            #go through 'children moves' and filter out moves based on evaluation criteria (this will reduce computational load but may cause error)
            if node.position.CTM=='w' and len(node.children)!=0:
                while len(node.children)>8:
                    sum = 0
                    max = node.children[0]
                    for x in range(len(node.children)):
                        sum = sum+node.children[x].eval
                        if node.children[x].eval>max.eval:
                            max = node.children[x]
                    average = sum/len(node.children)
                    for x in range(len(node.children)-1,-1,-1):
                        if node.children[x].eval<average and node.children[x]!=max:
                            del node.children[x] #trim children moves that are less than the average (i.e. optimizing for white)
            if node.position.CTM=='b' and len(node.children)!=0:
                while len(node.children)>8:
                    sum = 0
                    min = node.children[0]
                    for x in range(len(node.children)):
                        sum = sum+node.children[x].eval
                        if node.children[x].eval<min.eval:
                            min = node.children[x]
                    average = sum/len(node.children)
                    for x in range(len(node.children)-1,-1,-1):
                        if node.children[x].eval>average and node.children[x]!=min:
                            del node.children[x] #trim children moves that are less than the average (i.e. optimizing for black)
            ###
            for x in range(len(node.children)):
                self.createMoveTree(node.children[x],depth) #call createMoveTree on each child node to create it's children

    def printMoveTree(self):
        self.printMoveTreeRecursive(self.moveTreeHead)
        
    def printMoveTreeRecursive(self,node):
        if node is not None and node.parent is not None:
            for x in range(node.depth):
                print("\t",end='')
            print("Move: "+moveObjToMoveNotation(node.move,node.parent.position,node.position)+"\tDepth: "+str(node.depth)+"\tEval: "+str(node.eval))
            for x in range(len(node.children)):
                self.printMoveTreeRecursive(node.children[x])
        elif node is not None:
            for x in range(node.depth):
                print("\t",end='')
            print("Move: "+moveObjToMoveNotation(node.move,None,node.position)+"\tDepth: "+str(node.depth)+"\tEval: "+str(node.eval))
            for x in range(len(node.children)):
                self.printMoveTreeRecursive(node.children[x])
    
    def move(self,move):
        #0. Do sanity checks: game state, legal move
        if self.currentPosition.GS!='in-play':
            raise Exception("Game is over move() can not be called.")
        else:
            self.currentPosition=self.currentPosition.move(move)
            self.GS='in-play' #assume the game is in play
            #1. verify that the game hasn't reached a draw or checkmate
            self.verifyGameState()
            #2. create first evaluation tree and update stats
            self.moveTreeHead = self.TreeNode(None,self.currentPosition,0,None)
            self.bestMoveNode = None
            self.bestMove = None
            
    def printBestMove(self,flag):
        if self.bestMove==None:
            print("There are no moves in this position.")
        else:
            if flag==0:
                bestMove = self.bestMove
                print(moveObjToMoveNotation(bestMove,self.currentPosition,self.currentPosition.move(bestMove)))
            if flag==1:
                bestMoveNode = self.bestMoveNode
                print("BestMove: "+moveObjToMoveNotation(bestMoveNode.move,self.currentPosition,self.currentPosition.move(bestMoveNode.move))+"\t Branch Eval Average: "+str(round(bestMoveNode.calculateBranchAverage(),2)))
    
    def printEval(self):
        #0. Do sanity checks: game state
        print("WIP")
        
    def displayBoard(self):
        self.currentPosition.displayBoard()
        

##POSITION OBJECT##
class Position:
    class DynamicSquare:
        def __init__(self,contents,whiteTargeters,whiteZombieTargeters,blackTargeters,blackZombieTargeters,nextMovePieces):
            self.contents = contents #('.' implies empty, 'Q','k',...)
            self.whiteTargeters = whiteTargeters
            self.whiteZombieTargeters = whiteZombieTargeters
            self.blackTargeters = blackTargeters
            self.blackZombieTargeters = blackZombieTargeters
            self.nextMovePieces = nextMovePieces
    
    ##Position Object Initalization##
    def __init__(self,board,CTM,WCK,WCQ,BCK,BCQ,EPS,FRC,MC):
        ##Positional Information##
        #CTM: color to move
        #WC: white castling (boolean, Q=queenside, K=kingside)
        #BC: black castling
        #EPS: en passant target (there can only ever be one)
        #FRC: fifty rule counter # of moves ("move" consisting of one white and one black move) since last pawn push or capture
        #MC: move counter (where a move is one move of white and one move of black)
        self.board=board
        self.CTM=CTM
        self.WCK=WCK
        self.WCQ=WCQ
        self.BCK=BCK
        self.BCQ=BCQ
        self.EPS=EPS
        self.FRC=FRC
        self.MC=MC
        self.kingPosW = Square(-1,-1)
        self.kingPosW = Square(-1,-1)
        self.GS = 'in-play'
        
        #Check correct board size
        if len(board) != 8:
            raise ValueError ("Incorrect size of board")
        for x in range(0,8):
            if len(board[x]) != 8:
                raise ValueError ("Incorrect size of board")
                
        ##Position Validation##
        #1. no pieces share same square
        #done! (due to inherint data properties)
        #2. exactly one white and one black king
        whiteKingCount=0
        blackKingCount=0
        for x in range(0,8):
            for y in range(0,8):
                if board[x][y]=='k':
                    blackKingCount=blackKingCount+1
                    self.kingPosB = Square(x,y)
                if board[x][y]=='K':
                    whiteKingCount=whiteKingCount+1
                    self.kingPosW = Square(x,y)
        if whiteKingCount != 1 or blackKingCount != 1:
            raise ValueError("Not exactly 1 king of each color on the board")
        
        ##Dynamic Squares Creation##
        #This is to find all moves and targets for each square
        self.dynamicBoard = [['' for h in range(0,8)] for g in range (0,8)]
        if self.CTM == 'w':
            for x in range(0,8):
                for y in range(0,8):
                    temp_TargetersW,temp_zombieTargetersW,temp_TargetersB,temp_zombieTargetersB = self.makeSquareTargeterList(x,y)
                    temp_moveHereW = self.canMoveHereWhitesTurn(x,y)
                    self.dynamicBoard[x][y] = self.DynamicSquare(self.board[x][y],temp_TargetersW,temp_zombieTargetersW,temp_TargetersB,temp_zombieTargetersB,temp_moveHereW)
            self.cleanIllegalMoves('w')
        if self.CTM == 'b':
            for x in range(0,8):
                for y in range(0,8):
                    temp_TargetersW,temp_zombieTargetersW,temp_TargetersB,temp_zombieTargetersB = self.makeSquareTargeterList(x,y)
                    temp_MoveHereB = self.canMoveHereBlacksTurn(x,y)
                    self.dynamicBoard[x][y] = self.DynamicSquare(self.board[x][y],temp_TargetersW,temp_zombieTargetersW,temp_TargetersB,temp_zombieTargetersB,temp_MoveHereB)
            self.cleanIllegalMoves('b')
        
        ##Position Validation Resumed##
        #3. king must not be in check with opposition to move
        if self.inCheck('w') and self.CTM == 'b':
            raise Exception("King is in check with opposition to move")
        if self.inCheck('b') and self.CTM == 'w':
            raise Exception("King is in check with opposition to move")
        #4. not a stalemate, checkmate or otherwise drawn position
        #a. stalemate
        tempBool=True
        if self.CTM == 'w':
            for x in range(0,8):
                for y in range(0,8):
                    if len(self.dynamicBoard[x][y].nextMovePieces)>0:
                        tempBool=False
            if tempBool and self.inCheck('w')==False:
                self.GS='stalemate'
        if self.CTM == 'b':
            for x in range(0,8):
                for y in range(0,8):
                    if len(self.dynamicBoard[x][y].nextMovePieces)>0:
                        tempBool=False
            if tempBool and self.inCheck('b')==False:
                self.GS='stalemate'
        #b. 50 move rule
        if FRC==50:
            self.GS='draw50'
        #c. white checkmate
        if self.CTM == 'b':
            for x in range(0,8):
                for y in range(0,8):
                    if len(self.dynamicBoard[x][y].nextMovePieces)>0:
                        tempBool=False
            if tempBool and self.inCheck('b'):
                self.GS='whiteWin'
        #c. black checkmate
        if self.CTM == 'w':
            for x in range(0,8):
                for y in range(0,8):
                    if len(self.dynamicBoard[x][y].nextMovePieces)>0:
                        tempBool=False
            if tempBool and self.inCheck('w'):
                self.GS='blackWin'
        #d. insufficient material
        if self.checkInsufficientMaterial():
            self.GS='insufficient'
                        
            
        #5. castling rights compared against king and rook current squares
        if self.kingPosW != Square(4,7):
            self.WCK = False
            self.WCQ = False
        if self.kingPosB != Square(4,0):
            self.BCK = False
            self.BCQ = False
        if self.board[0][7] != 'R':
            self.WCQ=False
        if self.board[7][7] != 'R':
            self.WCK=False
        if self.board[0][0] != 'r':
            self.BCQ=False
        if self.board[7][0] != 'r':
            self.BCK=False
            
        self.evalMap = self.createInstantEvalMap()
        

    ##Position Functions##
    def pieceColor(self,pieceString):
        whiteColoredPieces = ['K','Q','B','N','R','P']
        blackColoredPieces = ['k','q','b','n','r','p']
        if pieceString in whiteColoredPieces:
            return 'w'
        if pieceString in blackColoredPieces:
            return 'b'
        else:
            return ''

    def scanDirectionSimple(self,file,rank,color,fileIncrement,rankIncrement): #scan a given direction from a square and find first piece targeting it (in that direction)
        pieceList = []
        onBoard=range(0,8)
        ##White##
        if color=='w':
            #1. diagonals
            if fileIncrement!=0 and rankIncrement!=0:
                #a. scan northeast (file(x) larger, rank(y) smaller)
                #b. scan northwest (file(x) smaller, rank(y) smaller)
                if (fileIncrement==1 and rankIncrement==-1) or (fileIncrement==-1 and rankIncrement==-1):
                    boolTemp = True
                    x=file          #placeholder for file
                    y=rank          #and rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='K':
                            pieceList.append(Piece('K',Square(x,y)))
                            #boolTemp=False
                            #Note: white pawns face north so if you scan north and there is a pawn infront then it is not targeting this square
                        if self.board[x][y]!='.' and self.board[x][y]!='k':
                            boolTemp=False #stop checking this diagonal because there is a piece blocking defense
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp):
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                            break
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            break
                        if self.board[x][y]!='.' and self.board[x][y]!='k':
                            break
                             #stop checking this diagonal because there is a piece blocking defense
                        x=x+fileIncrement
                        y=y+rankIncrement
                #c. scan southeast (file(x) larger, rank(y) larger)
                #d. scan southwest (file(x) smaller, rank(y) larger)
                if (fileIncrement==1 and rankIncrement==1) or (fileIncrement==-1 and rankIncrement==1):
                    boolTemp=True
                    x=file
                    y=rank  
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='K':
                            pieceList.append(Piece('K',Square(x,y)))
                            #boolTemp=False
                        if self.board[x][y]=='P':
                            pieceList.append(Piece('P',Square(x,y)))
                            boolTemp=False
                        #Note: white pawns face north so if you scan south and there is a pawn behind (diagonally) then it is targeting this square
                        if self.board[x][y]!='.' and self.board[x][y]!='k':
                            boolTemp=False
                            
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp):
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                            break
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            break
                        if self.board[x][y]!='.' and self.board[x][y]!='k':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
            #2. ranks and files
            if fileIncrement==0 or rankIncrement==0:
            #a. scan file right
            #b. scan file left
            #c. scan rank down (increasing)
            #d. scan rank up (decreasing)
                boolTemp=True
                x=file
                y=rank
                x=x+fileIncrement
                y=y+rankIncrement
                if x in onBoard and y in onBoard:
                #1. first iteration includes king
                    if self.board[x][y]=='R':
                        pieceList.append(Piece('R',Square(x,y)))
                        boolTemp=False
                    if self.board[x][y]=='Q':
                        pieceList.append(Piece('Q',Square(x,y)))
                        boolTemp=False
                    if self.board[x][y]=='K':
                        pieceList.append(Piece('K',Square(x,y)))
                        #boolTemp=False
                    if self.board[x][y]!='.' and self.board[x][y]!='k':
                        boolTemp=False
                #2. additional iterations exclude pawns and king
                x=x+fileIncrement
                y=y+rankIncrement
                while(x in onBoard and y in onBoard and boolTemp):
                    if self.board[x][y]=='R':
                        pieceList.append(Piece('R',Square(x,y)))
                        break
                    if self.board[x][y]=='Q':
                        pieceList.append(Piece('Q',Square(x,y)))
                        break
                    if self.board[x][y]!='.' and self.board[x][y]!='k':
                        break
                    x=x+fileIncrement
                    y=y+rankIncrement
            
        ##Black##
        if color=='b':
             #1. diagonals
            if fileIncrement!=0 and rankIncrement!=0:
                #a. scan northeast (file(x) larger, rank(y) smaller)
                #b. scan northwest (file(x) smaller, rank(y) smaller)
                if (fileIncrement==1 and rankIncrement==-1) or (fileIncrement==-1 and rankIncrement==-1):
                    boolTemp=True
                    x=file
                    y=rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='k':
                            pieceList.append(Piece('k',Square(x,y)))
                            #boolTemp=False
                        if self.board[x][y]=='p':
                            pieceList.append(Piece('p',Square(x,y)))
                            boolTemp=False
                        #Note: black pawns face south so if you scan north (behind) and there is a pawn there then it is targeting this square
                        if self.board[x][y]!='.' and self.board[x][y]!='K':
                            boolTemp=False
                    x=x+fileIncrement
                    y=y+rankIncrement
                    #2. additional iterations exclude pawns and king
                    while(x in onBoard and y in onBoard and boolTemp):
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                            break
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            break
                        if self.board[x][y]!='.' and self.board[x][y]!='K':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
                #c. scan southeast (file(x) larger, rank(y) larger)
                #d. scan southwest (file(x) smaller, rank(y) larger)
                if (fileIncrement==1 and rankIncrement==1) or (fileIncrement==-1 and rankIncrement==1):
                    boolTemp=True
                    x=file
                    y=rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='k':
                            pieceList.append(Piece('k',Square(x,y)))
                            #boolTemp=False
                        #Note: black pawns face south so if you scan south (infront) then you are behind the pawn and it is not defending you
                        if self.board[x][y]!='.' and self.board[x][y]!='K':
                            boolTemp=False
                    #2. additional iterations exclude pawns and kingx=x+fileIncrement
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp):
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                            break
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            break
                        if self.board[x][y]!='.' and self.board[x][y]!='K':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
            #2. ranks and files
            if fileIncrement==0 or rankIncrement==0:
            #a. scan file right
            #b. scan file left
            #c. scan rank down (increasing)
            #d. scan rank up (decreasing)
                boolTemp=True
                x=file
                y=rank
                x=x+fileIncrement
                y=y+rankIncrement
                if x in onBoard and y in onBoard:
                #1. first iteration includes king
                    if self.board[x][y]=='r':
                        pieceList.append(Piece('r',Square(x,y)))
                        boolTemp=False
                    if self.board[x][y]=='q':
                        pieceList.append(Piece('q',Square(x,y)))
                        boolTemp=False
                    if self.board[x][y]=='k':
                        pieceList.append(Piece('k',Square(x,y)))
                        #boolTemp=False
                    if self.board[x][y]!='.' and self.board[x][y]!='K':
                        boolTemp=False
                #2. additional iterations exclude pawns and king
                x=x+fileIncrement
                y=y+rankIncrement
                while(x in onBoard and y in onBoard and boolTemp):
                    if self.board[x][y]=='r':
                        pieceList.append(Piece('r',Square(x,y)))
                        break
                    if self.board[x][y]=='q':
                        pieceList.append(Piece('q',Square(x,y)))
                        break
                    if self.board[x][y]!='.' and self.board[x][y]!='K':
                        break
                    x=x+fileIncrement
                    y=y+rankIncrement
        return pieceList

    def pinnedStatus(self,board,file,rank):
        onBoard = range(0,8)
        #1. Does the piece share a same diag, file, or rank with king and if yes then identify diag, file, or rank (+direction)
        negDiagShared = False
        posDiagShared = False
        fileShared = False
        rankShared = False
        #2. Starting at piece identify if there are any pieces inbetween the piece and the king
        #3. going the opposite direction identify if there is the respective enemy piece that is opposite of the king in question
        pinnedStatus = 'none'
        
        #White#
        if self.pieceColor(board[file][rank]) == 'w':
            if self.kingPosW.file==file:
                fileShared = True
                for x in range(rank+1,self.kingPosW.rank): #there is a piece inbetween piece in question and same colored king
                    if board[file][x]!='.':
                        return 'none'
            elif self.kingPosW.rank==rank:
                rankShared = True
                for x in range(file+1,self.kingPosW.file): #there is a piece inbetween piece in question and same colored king
                    if board[x][rank]!='.':
                        return 'none'
            else:
                for x in range(-7,8): #positive axis diag
                    if ((file+x) in onBoard and (rank+x) in onBoard) and self.kingPosW.file==(file+x) and self.kingPosW.rank==(rank+x):
                        posDiagShared = True
                        break
                if posDiagShared:
                    #check for pieces inbetween king and pinned? piece
                    #a. piece is higher on neg diag than king
                    if rank<self.kingPosW.rank:
                        for x in range(1,7):
                            if (rank+x)==self.kingPosW.rank:
                                break
                            if board[file+x][rank+x]!='.':
                                return 'none'
                    #b. piece is lower on pos diag than king
                    else:
                        for x in range(1,7):
                            if (rank+x)==self.kingPosW.rank:
                                break
                            if board[file-x][rank-x]!='.':
                                return 'none'
                else:
                    for x in range(-7,8): #negative axis diag
                        if ((file+x) in onBoard and (rank-x) in onBoard) and self.kingPosW.file==(file+x) and self.kingPosW.rank==(rank-x):
                            negDiagShared = True
                            break
                    if negDiagShared:
                        #check for pieces inbetween king and pinned? piece
                        #a. piece is higher on neg diag than king
                        if rank<self.kingPosW.rank:
                            for x in range(1,7):
                                if (rank+x)==self.kingPosW.rank:
                                    break
                                if board[file-x][rank+x]!='.':
                                    return 'none'
                        #b. piece is lower on pos diag than king
                        else:
                            for x in range(1,7):
                                if (rank+x)==self.kingPosW.rank:
                                    break
                                if board[file+x][rank-x]!='.':
                                    return 'none'
            #The below checks after the piece to see if there is a piece pinning it to the king (i.e. r,b,q)
            #Case1 - File shared, king above piece
            if fileShared and (self.kingPosW.rank-rank)<0:
                for x in onBoard:
                    if (rank+x+1) not in onBoard:
                        break
                    if board[file][rank+x+1] not in ['r','q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file][rank+x+1] in ['r','q']:
                        pinnedStatus = 'file'
                        break
            #Case2 - File shared, king below piece
            if fileShared and (self.kingPosW.rank-rank)>0:
                for x in onBoard:
                    if (rank-x-1) not in onBoard:
                        break
                    if board[file][rank-x-1] not in ['r','q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file][rank-x-1] in ['r','q']:
                        pinnedStatus = 'file'
                        break
            #Case3 - Rank shared, king to right of piece
            if rankShared and (self.kingPosW.file-file)>0:
                for x in onBoard:
                    if (file-x-1) not in onBoard:
                        break
                    if board[file-x-1][rank] not in ['r','q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file-x-1][rank] in ['r','q']:
                        pinnedStatus = 'rank'
                        break
            #Case4 - Rank shared, king to left of piece
            if rankShared and (self.kingPosW.file-file)<0:
                for x in onBoard:
                    if (file+x+1) not in onBoard:
                        break
                    if board[file+x+1][rank] not in ['r','q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file+x+1][rank] in ['r','q']:
                        pinnedStatus = 'rank'
                        break
            #Case5 - Positive diagonal shared, king below piece
            if posDiagShared and (self.kingPosW.file-file)>0:
                for x in range(1,self.kingPosW.file-file+1):
                    if board[file+x][rank+x]!='.':
                        pinnedStatus = 'none'
                for x in range(1,8):
                    if (file-x) not in onBoard or (rank-x) not in onBoard:
                        break
                    if board[file-x][rank-x] not in ['b','q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file-x][rank-x] in ['b','q']:
                        pinnedStatus = 'posDiag'
                        break
            #Case6 - Positive diagonal shared, king above piece
            if posDiagShared and (self.kingPosW.file-file)<0:
                for x in range(1,self.kingPosW.file-file+1):
                    if board[file-x][rank-x]!='.':
                        pinnedStatus = 'none'
                for x in range(1,8):
                    if (file+x) not in onBoard or (rank+x) not in onBoard:
                        break
                    if board[file+x][rank+x] not in ['b','q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file+x][rank+x] in ['b','q']:
                        pinnedStatus = 'posDiag'
                        break
            #Case7 - Negative diagonal shared, king below piece
            if negDiagShared and (self.kingPosW.file-file)<0:
                for x in range(1,self.kingPosW.file-file+1):
                    if board[file-x][rank+x]!='.':
                        pinnedStatus = 'none'
                for x in range(1,8):
                    if (file+x) not in onBoard or (rank-x) not in onBoard:
                        break
                    if board[file+x][rank-x] not in ['b','q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file+x][rank-x] in ['b','q']:
                        pinnedStatus = 'negDiag'
                        break
            #Case8 - Negative diagonal shared, king above piece
            if negDiagShared and (self.kingPosW.file-file)>0:
                for x in range(1,self.kingPosW.file-file+1):
                    if board[file+x][rank-x]!='.':
                        pinnedStatus = 'none'
                for x in range(1,8):
                    if (file-x) not in onBoard or (rank+x) not in onBoard:
                        break
                    if board[file-x][rank+x] not in ['b','q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file-x][rank+x] in ['b','q']:
                        pinnedStatus = 'negDiag'
                        break
        
        #Black# (symmetric)
        if self.pieceColor(board[file][rank]) == 'b':
            if self.kingPosB.file==file:
                fileShared = True
                for x in range(rank+1,self.kingPosB.rank): #there is a piece inbetween piece in question and same colored king
                    if board[file][x]!='.':
                        return 'none'
            elif self.kingPosB.rank==rank:
                rankShared = True
                for x in range(file+1,self.kingPosB.file): #there is a piece inbetween piece in question and same colored king
                    if board[x][rank]!='.':
                        return 'none'
            else:
                for x in range(-7,8): #positive axis diag
                    if ((file+x) in onBoard and (rank+x) in onBoard) and self.kingPosB.file==(file+x) and self.kingPosB.rank==(rank+x):
                        posDiagShared = True
                        break
                if posDiagShared:
                    #check for pieces inbetween king and pinned? piece
                    #a. piece is higher on pos diag than king
                    if rank<self.kingPosB.rank:
                        for x in range(1,7):
                            if (rank+x)==self.kingPosB.rank:
                                break
                            if board[file+x][rank+x]!='.':
                                return 'none'
                    #b. piece is lower on pos diag than king
                    else:
                        for x in range(1,7):
                            if (rank+x)==self.kingPosB.rank:
                                break
                            if board[file-x][rank-x]!='.':
                                return 'none'
                else:
                    for x in range(-7,8): #negative axis diag
                        if ((file+x) in onBoard and (rank-x) in onBoard) and self.kingPosB.file==(file+x) and self.kingPosB.rank==(rank-x):
                            negDiagShared = True
                            break
                    if negDiagShared:
                        #check for pieces inbetween king and pinned? piece
                        #a. piece is higher on neg diag than king
                        if rank<self.kingPosB.rank:
                            for x in range(1,7):
                                if (rank+x)==self.kingPosB.rank:
                                    break
                                if board[file-x][rank+x]!='.':
                                    return 'none'
                        #b. piece is lower on pos diag than king
                        else:
                            for x in range(1,7):
                                if (rank+x)==self.kingPosB.rank:
                                    break
                                if board[file+x][rank-x]!='.':
                                    return 'none'
            #Case1 - File shared, king above piece
            if fileShared and (self.kingPosB.rank-rank)<0:
                for x in onBoard:
                    if (rank+x+1) not in onBoard:
                        break
                    if board[file][rank+x+1] not in ['R','Q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file][rank+x+1] in ['R','Q']:
                        pinnedStatus = 'file'
                        break
            #Case2 - File shared, king below piece
            if fileShared and (self.kingPosB.rank-rank)>0:
                for x in onBoard:
                    if (rank-x-1) not in onBoard:
                        break
                    if board[file][rank-x-1] not in ['R','Q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file][rank-x-1] in ['R','Q']:
                        pinnedStatus = 'file'
                        break
            #Case3 - Rank shared, king to right of piece
            if rankShared and (self.kingPosB.file-file)>0:
                for x in onBoard:
                    if (file-x-1) not in onBoard:
                        break
                    if board[file-x-1][rank] not in ['R','Q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file-x-1][rank] in ['R','Q']:
                        pinnedStatus = 'rank'
                        break
            #Case4 - Rank shared, king to left of piece
            if rankShared and (self.kingPosB.file-file)<0:
                for x in onBoard:
                    if (file+x+1) not in onBoard:
                        break
                    if board[file+x+1][rank] not in ['R','Q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file+x+1][rank] in ['R','Q']:
                        pinnedStatus = 'rank'
                        break
            #Case5 - Positive diagonal shared, king below piece
            if posDiagShared and (self.kingPosB.file-file)>0:
                for x in range(1,self.kingPosB.file-file+1):
                    if board[file+x][rank+x]!='.':
                        pinnedStatus = 'none'
                for x in range(1,8):
                    if (file-x) not in onBoard or (rank-x) not in onBoard:
                        break
                    if board[file-x][rank-x] not in ['B','Q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file-x][rank-x] in ['B','Q']:
                        pinnedStatus = 'posDiag'
                        break
            #Case6 - Positive diagonal shared, king above piece
            if posDiagShared and (self.kingPosB.file-file)<0:
                for x in range(1,self.kingPosB.file-file+1):
                    if board[file-x][rank-x]!='.':
                        pinnedStatus = 'none'
                for x in range(1,8):
                    if (file+x) not in onBoard or (rank+x) not in onBoard:
                        break
                    if board[file+x][rank+x] not in ['B','Q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file+x][rank+x] in ['B','Q']:
                        pinnedStatus = 'posDiag'
                        break
            #Case7 - Negative diagonal shared, king below piece
            if negDiagShared and (self.kingPosB.file-file)<0:
                for x in range(1,self.kingPosB.file-file+1):
                    if board[file-x][rank+x]!='.':
                        pinnedStatus = 'none'
                for x in range(1,8):
                    if (file+x) not in onBoard or (rank-x) not in onBoard:
                        break
                    if board[file+x][rank-x] not in ['B','Q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file+x][rank-x] in ['B','Q']:
                        pinnedStatus = 'negDiag'
                        break
            #Case8 - Negative diagonal shared, king above piece
            if negDiagShared and (self.kingPosB.file-file)>0:
                for x in range(1,self.kingPosB.file-file+1):
                    if board[file+x][rank-x]!='.':
                        pinnedStatus = 'none'
                for x in range(1,8):
                    if (file-x) not in onBoard or (rank+x) not in onBoard:
                        break
                    if board[file-x][rank+x] not in ['B','Q','.']:
                        pinnedStatus = 'none'
                        break
                    if board[file-x][rank+x] in ['B','Q']:
                        pinnedStatus = 'negDiag'
                        break
        return pinnedStatus

    def squareOutsideOfPin(self,squareFile,squareRank,pinFile,pinRank): #square - the square we would like to move to, pin - the square the piece in question is on
        onBoard = range(0,8)
        if self.pinnedStatus(self.board,pinFile,pinRank)=='file':
            if squareFile!=pinFile:
                return True
        elif self.pinnedStatus(self.board,pinFile,pinRank)=='rank':
            if squareRank!=pinRank:
                return True
        elif self.pinnedStatus(self.board,pinFile,pinRank)=='posDiag':
            for x in range(-7,8):
                if squareFile+x==pinFile and squareRank+x==pinRank:
                    return False
            return True
        elif self.pinnedStatus(self.board,pinFile,pinRank)=='negDiag':
            for x in range(-7,8):
                if squareFile+x==pinFile and squareRank-x==pinRank:
                    return False
            return True
        else:
            return False
        return False

    def scanToMove(self,file,rank,color,fileIncrement,rankIncrement): #this verison of the scan looks at squares that pieces can move to (this is unique because of the nature of pawns)
        pieceList=[]    #collection of pieces in this scanned direction
        onBoard=range(0,8)
        ##White##
        if color=='w':
            #1. diagonals
            if fileIncrement!=0 and rankIncrement!=0:
                #a. scan northeast (file(x) larger, rank(y) smaller)
                #b. scan northwest (file(x) smaller, rank(y) smaller)
                if (fileIncrement==1 and rankIncrement==-1) or (fileIncrement==-1 and rankIncrement==-1):
                    boolTemp=True
                    x=file          #placeholder for file
                    y=rank          #and rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard and self.squareOutsideOfPin(file,rank,x,y)==False:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                        if self.board[x][y]=='K':
                            pieceList.append(Piece('K',Square(x,y)))
                        if self.board[x][y]!='.':
                            boolTemp=False #stop checking this diagonal because there is a piece blocking defense
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp and self.squareOutsideOfPin(file,rank,x,y)==False):
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                            break
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            break
                        if self.board[x][y]!='.':
                            break #stop checking this diagonal because there is a piece blocking defense
                        x=x+fileIncrement
                        y=y+rankIncrement
                #c. scan southeast (file(x) larger, rank(y) larger)
                #d. scan southwest (file(x) smaller, rank(y) larger)
                if (fileIncrement==1 and rankIncrement==1) or (fileIncrement==-1 and rankIncrement==1):
                    boolTemp=True
                    x=file
                    y=rank  
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard and self.squareOutsideOfPin(file,rank,x,y)==False:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='K':
                            pieceList.append(Piece('K',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='P':
                            if self.board[file][rank]!='.' or (self.EPS.file==file and self.EPS.rank==rank): #The pawn in question can only move to the square if there is a piece there or it is an en passant target
                                pieceList.append(Piece('P',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp and self.squareOutsideOfPin(file,rank,x,y)==False):
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                            break
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            break
                        if self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
            #2. ranks and files
            if fileIncrement==0 or rankIncrement==0:
                #d. scan rank up (decreasing)
                if fileIncrement==0 and rankIncrement==1: #Note: In this special case we need to consider white pawn moves
                    boolTemp=True
                    x=file
                    y=rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard and self.squareOutsideOfPin(file,rank,x,y)==False:
                    #1. first iteration includes king
                        if self.board[x][y]=='R':
                            pieceList.append(Piece('R',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='K':
                            pieceList.append(Piece('K',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='P' and self.board[x][y-1]=='.':
                            pieceList.append(Piece('P',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp and self.squareOutsideOfPin(file,rank,x,y)==False):
                        if self.board[x][y]=='R':
                            pieceList.append(Piece('R',Square(x,y)))
                            break
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            break
                        if self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
                #a. scan file right
                #b. scan file left
                #c. scan rank down (increasing)
                else: #we don't need to consider pawn moves
                    boolTemp=True
                    x=file
                    y=rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard and self.squareOutsideOfPin(file,rank,x,y)==False:
                    #1. first iteration includes king
                        if self.board[x][y]=='R':
                            pieceList.append(Piece('R',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='K':
                            pieceList.append(Piece('K',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp and self.squareOutsideOfPin(file,rank,x,y)==False):
                        if self.board[x][y]=='R':
                            pieceList.append(Piece('R',Square(x,y)))
                            break
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                            break
                        if self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
                    
            
        ##Black##
        if color=='b':
            #1. diagonals
            if fileIncrement!=0 and rankIncrement!=0:
                #a. scan northeast (file(x) larger, rank(y) smaller)
                #b. scan northwest (file(x) smaller, rank(y) smaller)
                if (fileIncrement==1 and rankIncrement==-1) or (fileIncrement==-1 and rankIncrement==-1):
                    boolTemp=True
                    x=file          #placeholder for file
                    y=rank          #and rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard and self.squareOutsideOfPin(file,rank,x,y)==False:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='k':
                            pieceList.append(Piece('k',Square(x,y)))
                        if self.board[x][y]=='p':
                            if self.board[file][rank]!='.' or (self.EPS.file==file and self.EPS.rank==rank): #The pawn in question can only move to the square if there is a piece there
                                pieceList.append(Piece('p',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]!='.':
                            boolTemp=False #stop checking this diagonal because there is a piece blocking defense
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp and self.squareOutsideOfPin(file,rank,x,y)==False):
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                            break
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            break
                        if self.board[x][y]!='.':
                            break #stop checking this diagonal because there is a piece blocking defense
                        x=x+fileIncrement
                        y=y+rankIncrement
                #c. scan southeast (file(x) larger, rank(y) larger)
                #d. scan southwest (file(x) smaller, rank(y) larger)
                if (fileIncrement==1 and rankIncrement==1) or (fileIncrement==-1 and rankIncrement==1):
                    boolTemp=True
                    x=file
                    y=rank  
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard and self.squareOutsideOfPin(file,rank,x,y)==False:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='k':
                            pieceList.append(Piece('k',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp and self.squareOutsideOfPin(file,rank,x,y)==False):
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                            break
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            break
                        if self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
            #2. ranks and files
            if fileIncrement==0 or rankIncrement==0:
                #c. scan rank down (increasing)
                if fileIncrement==0 and rankIncrement==-1: #Note: In this special case we need to consider black pawn moves
                    boolTemp=True
                    x=file
                    y=rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard and self.squareOutsideOfPin(file,rank,x,y)==False:
                    #1. first iteration includes king
                        if self.board[x][y]=='r':
                            pieceList.append(Piece('r',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='k':
                            pieceList.append(Piece('k',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='p' and self.board[x][y+1]=='.':
                            pieceList.append(Piece('p',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp and self.squareOutsideOfPin(file,rank,x,y)==False):
                        if self.board[x][y]=='r':
                            pieceList.append(Piece('r',Square(x,y)))
                            break
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            break
                        if self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
                #a. scan file right
                #b. scan file left
                #d. scan rank up (decreasing)
                else: #we don't need to consider pawn moves
                    boolTemp=True
                    x=file          #placeholder for file
                    y=rank          #and rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard and self.squareOutsideOfPin(file,rank,x,y)==False:
                    #1. first iteration includes king
                        if self.board[x][y]=='r':
                            pieceList.append(Piece('r',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]=='k':
                            pieceList.append(Piece('k',Square(x,y)))
                            boolTemp=False
                        if self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp and self.squareOutsideOfPin(file,rank,x,y)==False):
                        if self.board[x][y]=='r':
                            pieceList.append(Piece('r',Square(x,y)))
                            break
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                            break
                        if self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
        return pieceList

    def knightScan(self,file,rank,color):
        pieceList=[]
        onBoard = range(0,8)
        ##White##
        if color=='w':
            #a. left 2 down 1
            x=file-2
            y=rank+1
            if x in onBoard and y in onBoard and self.board[x][y]=='N':
                pieceList.append(Piece('N',Square(x,y)))
            #b. left 2 up 1
            x=file-2
            y=rank-1
            if x in onBoard and y in onBoard and self.board[x][y]=='N':
                pieceList.append(Piece('N',Square(x,y)))
            #c. left 1 down 2
            x=file-1
            y=rank+2
            if x in onBoard and y in onBoard and self.board[x][y]=='N':
                pieceList.append(Piece('N',Square(x,y)))
            #d. left 1 up 2
            x=file-1
            y=rank-2
            if x in onBoard and y in onBoard and self.board[x][y]=='N':
                pieceList.append(Piece('N',Square(x,y)))
            #e. right 2 down 1
            x=file+2
            y=rank+1
            if x in onBoard and y in onBoard and self.board[x][y]=='N':
                pieceList.append(Piece('N',Square(x,y)))
            #f. right 2 up 1
            x=file+2
            y=rank-1
            if x in onBoard and y in onBoard and self.board[x][y]=='N':
                pieceList.append(Piece('N',Square(x,y)))
            #g. right 1 down 2
            x=file+1
            y=rank+2
            if x in onBoard and y in onBoard and self.board[x][y]=='N':
                pieceList.append(Piece('N',Square(x,y)))
            #h. right 1 up 2
            x=file+1
            y=rank-2
            if x in onBoard and y in onBoard and self.board[x][y]=='N':
                pieceList.append(Piece('N',Square(x,y)))
        ##Black##
        if color=='b':
            #a. left 2 down 1
            x=file-2
            y=rank+1
            if x in onBoard and y in onBoard and self.board[x][y]=='n':
                pieceList.append(Piece('n',Square(x,y)))
            #b. left 2 up 1
            x=file-2
            y=rank-1
            if x in onBoard and y in onBoard and self.board[x][y]=='n':
                pieceList.append(Piece('n',Square(x,y)))
            #c. left 1 down 2
            x=file-1
            y=rank+2
            if x in onBoard and y in onBoard and self.board[x][y]=='n':
                pieceList.append(Piece('n',Square(x,y)))
            #d. left 1 up 2
            x=file-1
            y=rank-2
            if x in onBoard and y in onBoard and self.board[x][y]=='n':
                pieceList.append(Piece('n',Square(x,y)))
            #e. right 2 down 1
            x=file+2
            y=rank+1
            if x in onBoard and y in onBoard and self.board[x][y]=='n':
                pieceList.append(Piece('n',Square(x,y)))
            #f. right 2 up 1
            x=file+2
            y=rank-1
            if x in onBoard and y in onBoard and self.board[x][y]=='n':
                pieceList.append(Piece('n',Square(x,y)))
            #g. right 1 down 2
            x=file+1
            y=rank+2
            if x in onBoard and y in onBoard and self.board[x][y]=='n':
                pieceList.append(Piece('n',Square(x,y)))
            #h. right 1 up 2
            x=file+1
            y=rank-2
            if x in onBoard and y in onBoard and self.board[x][y]=='n':
                pieceList.append(Piece('n',Square(x,y)))
        return pieceList

    def knightScanToMove(self,file,rank,color):
        pieceList=[]
        onBoard = range(0,8)
        ##White##
        if color=='w':
            #a. left 2 down 1
            x=file-2
            y=rank+1
            if x in onBoard and y in onBoard and self.board[x][y]=='N' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('N',Square(x,y)))
            #b. left 2 up 1
            x=file-2
            y=rank-1
            if x in onBoard and y in onBoard and self.board[x][y]=='N' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('N',Square(x,y)))
            #c. left 1 down 2
            x=file-1
            y=rank+2
            if x in onBoard and y in onBoard and self.board[x][y]=='N' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('N',Square(x,y)))
            #d. left 1 up 2
            x=file-1
            y=rank-2
            if x in onBoard and y in onBoard and self.board[x][y]=='N' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('N',Square(x,y)))
            #e. right 2 down 1
            x=file+2
            y=rank+1
            if x in onBoard and y in onBoard and self.board[x][y]=='N' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('N',Square(x,y)))
            #f. right 2 up 1
            x=file+2
            y=rank-1
            if x in onBoard and y in onBoard and self.board[x][y]=='N' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('N',Square(x,y)))
            #g. right 1 down 2
            x=file+1
            y=rank+2
            if x in onBoard and y in onBoard and self.board[x][y]=='N' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('N',Square(x,y)))
            #h. right 1 up 2
            x=file+1
            y=rank-2
            if x in onBoard and y in onBoard and self.board[x][y]=='N' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('N',Square(x,y)))
        ##Black##
        if color=='b':
            #a. left 2 down 1
            x=file-2
            y=rank+1
            if x in onBoard and y in onBoard and self.board[x][y]=='n' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('n',Square(x,y)))
            #b. left 2 up 1
            x=file-2
            y=rank-1
            if x in onBoard and y in onBoard and self.board[x][y]=='n' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('n',Square(x,y)))
            #c. left 1 down 2
            x=file-1
            y=rank+2
            if x in onBoard and y in onBoard and self.board[x][y]=='n' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('n',Square(x,y)))
            #d. left 1 up 2
            x=file-1
            y=rank-2
            if x in onBoard and y in onBoard and self.board[x][y]=='n' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('n',Square(x,y)))
            #e. right 2 down 1
            x=file+2
            y=rank+1
            if x in onBoard and y in onBoard and self.board[x][y]=='n' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('n',Square(x,y)))
            #f. right 2 up 1
            x=file+2
            y=rank-1
            if x in onBoard and y in onBoard and self.board[x][y]=='n' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('n',Square(x,y)))
            #g. right 1 down 2
            x=file+1
            y=rank+2
            if x in onBoard and y in onBoard and self.board[x][y]=='n' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('n',Square(x,y)))
            #h. right 1 up 2
            x=file+1
            y=rank-2
            if x in onBoard and y in onBoard and self.board[x][y]=='n' and self.squareOutsideOfPin(file,rank,x,y)==False:
                pieceList.append(Piece('n',Square(x,y)))
        return pieceList

    def makeSquareTargeterList(self,file,rank): #i.e. who is looking at this square
        whiteList=[]
        whiteZombieList=[]
        blackList=[]
        blackZombieList=[]
        ##White##
        #first scan for white pieces targeting this square
        #1. diagonals
        #a. scan northeast (file(x) larger, rank(y) smaller)
        #b. scan northwest (file(x) smaller, rank(y) smaller)
        #c. scan southeast (file(x) larger, rank(y) larger)
        #d. scan southwest (file(x) smaller, rank(y) larger)
        diagonalListW = itertools.chain(self.scanDirectionSimple(file,rank,'w',1,-1),self.scanDirectionSimple(file,rank,'w',-1,-1),self.scanDirectionSimple(file,rank,'w',1,1),self.scanDirectionSimple(file,rank,'w',-1,1))
        #2. ranks and files
        #a. scan file right
        #b. scan file left
        #c. scan rank down (increasing)
        #d. scan rank up (decreasing)
        rankAndFileListW = itertools.chain(self.scanDirectionSimple(file,rank,'w',1,0),self.scanDirectionSimple(file,rank,'w',-1,0),self.scanDirectionSimple(file,rank,'w',0,1),self.scanDirectionSimple(file,rank,'w',0,-1))
        #3. knights
        knightListW = itertools.chain(self.knightScan(file,rank,'w'))
        whiteList = itertools.chain(diagonalListW,rankAndFileListW,knightListW)
        #4. Check for Zombies (i.e. pinned piece targeters)
        whiteList = list(whiteList)
        h=0
        while h<len(whiteList):
            #if the piece is pinned then take it out of the normal targeter list and add it to the zombie one
            if self.squareOutsideOfPin(file,rank,whiteList[h].square.file,whiteList[h].square.rank):
                whiteZombieList.append(whiteList[h])
                whiteList.remove(whiteList[h])
            else:
                h=h+1
                
        ##Black##
        #then scan for black pieces targeting this square
        #1. diagonals
        #a. scan northeast (file(x) larger, rank(y) smaller)
        #b. scan northwest (file(x) smaller, rank(y) smaller)
        #c. scan southeast (file(x) larger, rank(y) larger)
        #d. scan southwest (file(x) smaller, rank(y) larger)
        diagonalListB = itertools.chain(self.scanDirectionSimple(file,rank,'b',1,-1),self.scanDirectionSimple(file,rank,'b',-1,-1),self.scanDirectionSimple(file,rank,'b',1,1),self.scanDirectionSimple(file,rank,'b',-1,1))
        #2. ranks and files
        #a. scan file right
        #b. scan file left
        #c. scan rank down (increasing)
        #d. scan rank up (decreasing)
        rankAndFileListB = itertools.chain(self.scanDirectionSimple(file,rank,'b',1,0),self.scanDirectionSimple(file,rank,'b',-1,0),self.scanDirectionSimple(file,rank,'b',0,1),self.scanDirectionSimple(file,rank,'b',0,-1))
        #3. knights
        knightListB = itertools.chain(self.knightScan(file,rank,'b'))
        blackList = itertools.chain(diagonalListB,rankAndFileListB,knightListB)
        blackList = list(blackList)
        h=0
        while h<len(blackList):
            #if the piece is pinned then take it out of the normal targeter list and add it to the zombie one
            if self.squareOutsideOfPin(file,rank,blackList[h].square.file,blackList[h].square.rank):
                blackZombieList.append(blackList[h])
                blackList.remove(blackList[h])
            else:
                h=h+1

        return whiteList,list(whiteZombieList),blackList,list(blackZombieList)

    def inCheck(self,color):
        if color == 'w':
            if len(self.dynamicBoard[self.kingPosW.file][self.kingPosW.rank].blackTargeters) != 0:
                return True
            else:
                return False
        if color == 'b':
            if len(self.dynamicBoard[self.kingPosB.file][self.kingPosB.rank].whiteTargeters) != 0:
                return True
            else:
                return False

    def canMoveHereWhitesTurn(self,file,rank): #run through all of the targeted squares by white, on white's turn, then return all the pieces that can move to the square[file][rank]
        whiteList = []
        if self.pieceColor(self.board[file][rank]) == 'b' or self.pieceColor(self.board[file][rank]) == '': #can only move to this square if square is opposite color, empty and there are no pieces in the way
            ##White##
            #first scan for white pieces targeting this square
            #1. diagonals
            #a. scan northeast (file(x) larger, rank(y) smaller)
            #b. scan northwest (file(x) smaller, rank(y) smaller)
            #c. scan southeast (file(x) larger, rank(y) larger)
            #d. scan southwest (file(x) smaller, rank(y) larger)
            diagonalListW = itertools.chain(self.scanToMove(file,rank,'w',1,-1),self.scanToMove(file,rank,'w',-1,-1),self.scanToMove(file,rank,'w',1,1),self.scanToMove(file,rank,'w',-1,1))
            #2. ranks and files
            #a. scan file right
            #b. scan file left
            #c. scan rank down (increasing)
            #d. scan rank up (decreasing)
            rankAndFileListW = itertools.chain(self.scanToMove(file,rank,'w',1,0),self.scanToMove(file,rank,'w',-1,0),self.scanToMove(file,rank,'w',0,1),self.scanToMove(file,rank,'w',0,-1))
            #3. knights
            knightListW = itertools.chain(self.knightScanToMove(file,rank,'w'))#Scan central squares for possible pawn 2 square moves (only on first pawn move):
            whiteList = itertools.chain(diagonalListW,rankAndFileListW,knightListW)
            whiteList = list(whiteList)
            #4. Add initial 2 square pawn move if applicable
            if rank==4:
                if self.board[file][rank+2]=='P' and self.board[file][rank+1]=='.' and self.board[file][rank]=='.' and self.squareOutsideOfPin(file,rank,file,rank+2)==False:
                    whiteList.append(Piece('P',Square(file,rank+2)))
        return whiteList

    def canMoveHereBlacksTurn(self,file,rank): #same but when it is black to move
        blackList = []
        if self.pieceColor(self.board[file][rank]) == 'w' or self.pieceColor(self.board[file][rank]) == '': #can only move to this square if square is opposite color, empty and there are no pieces in the way
            ##Black##
            #then scan for black pieces targeting this square
            #1. diagonals
            #a. scan northeast (file(x) larger, rank(y) smaller)
            #b. scan northwest (file(x) smaller, rank(y) smaller)
            #c. scan southeast (file(x) larger, rank(y) larger)
            #d. scan southwest (file(x) smaller, rank(y) larger)
            diagonalListB = itertools.chain(self.scanToMove(file,rank,'b',1,-1),self.scanToMove(file,rank,'b',-1,-1),self.scanToMove(file,rank,'b',1,1),self.scanToMove(file,rank,'b',-1,1))
            #2. ranks and files
            #a. scan file right
            #b. scan file left
            #c. scan rank down (increasing)
            #d. scan rank up (decreasing)
            rankAndFileListB = itertools.chain(self.scanToMove(file,rank,'b',1,0),self.scanToMove(file,rank,'b',-1,0),self.scanToMove(file,rank,'b',0,1),self.scanToMove(file,rank,'b',0,-1))
            #3. knights
            knightListB = itertools.chain(self.knightScanToMove(file,rank,'b'))
            blackList = itertools.chain(diagonalListB,rankAndFileListB,knightListB)
            blackList = list(blackList)
            #4. Add initial 2 square pawn move if applicable
            if rank==3:
                if self.board[file][rank-2]=='p' and self.board[file][rank-1]=='.' and self.board[file][rank]=='.' and self.squareOutsideOfPin(file,rank,file,rank-2)==False:
                    blackList.append(Piece('p',Square(file,rank-2)))
        return blackList

    def printEvalMap(self):
        for y in range(0,8):
            for x in range(0,8):
                print(str(self.evalMap[x][y])+"\t",end='')
            print()
            print()
        
    def printInstantEval(self):
        print("Instant Evaluation: "+str(self.instantEval()))
    
    def instantEval(self):
        if self.GS=='whiteWin':
            eval = 100
        elif self.GS=='blackWin':
            eval = -100
        elif self.GS=='draw50':
            eval = 0
        elif self.GS=='insufficient':
            eval = 0
        elif self.GS=='stalemate':
            eval = 0
        else:
            sum = 0
            for x in range(0,8):
                for y in range(0,8):
                    sum = self.evalMap[x][y]+sum
            eval = np.round(sum,4)
        #bonuses for being able to castle
        if self.WCK==True:
            eval=eval+1
        if self.WCQ==True:
            eval=eval+1
        if self.BCK==True:
            eval=eval-1
        if self.BCQ==True:
            eval=eval-1
        return eval

    def createInstantEvalMap(self):
        #Instantaneous evaluation of a given position by summing up controlled squares, material on the board, and number of moves available
        squareControlWeightLinear = 0.5
        valueMap = np.array([[0.0 for _ in range(0,8)] for _ in range(0,8)])
        for x in range(0,4):
            for y in range(0,4):
                valueMap[x][y] = 0.11*(1.1**y)*(1.1**x)+squareControlWeightLinear
                valueMap[x][7-y] = 0.11*(1.1**y)*(1.1**x)+squareControlWeightLinear
                valueMap[7-x][y] = 0.11*(1.1**y)*(1.1**x)+squareControlWeightLinear
                valueMap[7-x][7-y] = 0.11*(1.1**y)*(1.1**x)+squareControlWeightLinear
        adjacentToKingValue = 2
        def adjacentToKing(x,y,color):
            if color=='w':
                if (((self.kingPosW.file+1==x and self.kingPosW.file+1 in range(0,8)) or (self.kingPosW.file-1==x and self.kingPosW.file-1 in range(0,8)) or self.kingPosW.file==x)
                and ((self.kingPosW.rank+1==x and self.kingPosW.rank+1 in range(0,8)) or (self.kingPosW.rank-1==x and self.kingPosW.rank-1 in range(0,8)) or self.kingPosW.rank==x)
                and not (self.kingPosW.file==x and self.kingPosW.rank==y)):
                    return True
                else:
                    return False
            elif color=='b':
                if (((self.kingPosW.file+1==x and self.kingPosW.file+1 in range(0,8)) or (self.kingPosW.file-1==x and self.kingPosW.file-1 in range(0,8)) or self.kingPosW.file==x)
                and ((self.kingPosW.rank+1==x and self.kingPosW.rank+1 in range(0,8)) or (self.kingPosW.rank-1==x and self.kingPosW.rank-1 in range(0,8)) or self.kingPosW.rank==x)
                and not (self.kingPosW.file==x and self.kingPosW.rank==y)):
                    return True
                else:
                    return False
            else:
                return False
            
        #Value for pieces
        for x in range(0,8):
            for y in range(0,8):
                if self.board[x][y] == 'n':
                    valueMap[x][y] = 3
                if self.board[x][y] == 'b':
                    valueMap[x][y] = 3
                if self.board[x][y] == 'r':
                    valueMap[x][y] = 5
                if self.board[x][y] == 'q':
                    valueMap[x][y] = 9
                if self.board[x][y] == 'N':
                    valueMap[x][y] = 3
                if self.board[x][y] == 'B':
                    valueMap[x][y] = 3
                if self.board[x][y] == 'R':
                    valueMap[x][y] = 5
                if self.board[x][y] == 'Q':
                    valueMap[x][y] = 9
        
        #Scaling value of pawns as they move up the board
        for x in range(0,8):
            for y in range(1,7):
                if self.board[x][y] == 'p':
                    valueMap[x][y] = valueMap[x][y]+1*(1.05**(3*(y-1)))
                if self.board[x][y] == 'P':
                    valueMap[x][y] = valueMap[x][y]+1*(1.05**(3*(6-y)))

        for x in range(0,8):
            for y in range(0,8):
                if len(self.dynamicBoard[x][y].whiteTargeters)<len(self.dynamicBoard[x][y].blackTargeters) and self.board[x][y]=='.': #more black targeters on this square
                    valueMap[x][y] = valueMap[x][y]*-1
                    if adjacentToKing(x,y,'w'):
                        valueMap[x][y] = valueMap[x][y]-adjacentToKingValue
                if len(self.dynamicBoard[x][y].whiteTargeters)>len(self.dynamicBoard[x][y].blackTargeters) and self.board[x][y]=='.': #more white targeters on this square
                    #valueMap[x][y] = valueMap[x][y]
                    if adjacentToKing(x,y,'b'):
                        valueMap[x][y] = valueMap[x][y]+adjacentToKingValue
                if len(self.dynamicBoard[x][y].whiteTargeters)==len(self.dynamicBoard[x][y].blackTargeters) and self.board[x][y]=='.': #equal targeters (or 0 total)
                    valueMap[x][y] = 0
                if len(self.dynamicBoard[x][y].whiteTargeters)<len(self.dynamicBoard[x][y].blackTargeters) and self.pieceColor(self.board[x][y])=='w': #more black targeters on this square and the square contains white piece
                    valueMap[x][y] = valueMap[x][y]/2
                if len(self.dynamicBoard[x][y].whiteTargeters)<len(self.dynamicBoard[x][y].blackTargeters) and self.pieceColor(self.board[x][y])=='b': #more black targeters on this square and the square contains black piece
                    valueMap[x][y] = valueMap[x][y]*-1.2
                if len(self.dynamicBoard[x][y].whiteTargeters)==len(self.dynamicBoard[x][y].blackTargeters) and self.pieceColor(self.board[x][y])=='b': #equal targeters (or 0 total) and the square contains black piece
                    valueMap[x][y] = valueMap[x][y]*-1
                if len(self.dynamicBoard[x][y].whiteTargeters)>len(self.dynamicBoard[x][y].blackTargeters) and self.pieceColor(self.board[x][y])=='w': #more white targeters on this square and the square contains white piece
                    valueMap[x][y] = valueMap[x][y]*1.2
                if len(self.dynamicBoard[x][y].whiteTargeters)>len(self.dynamicBoard[x][y].blackTargeters) and self.pieceColor(self.board[x][y])=='b': #more white targeters on this square and the square contains black piece
                    valueMap[x][y] = valueMap[x][y]/2
                #if len(self.dynamicBoard[x][y].whiteTargeters)==len(self.dynamicBoard[x][y].blackTargeters) and self.pieceColor(self.board[x][y])=='w': #equal targeters (or 0 total) and the square contains white piece
                    #valueMap[x][y] = valueMap[x][y]

        #Round map off to 4 decimal places        
        for x in range(0,8):
            for y in range(0,8):
                valueMap[x][y] = round(valueMap[x][y],4)
        return valueMap

    def checkInsufficientMaterial(self):
        pieceCountW = 0
        pieceCountB = 0
        knightsW = 0
        knightsB = 0
        bishopsW = 0
        bishopsB = 0
        rooksW = 0
        rooksB = 0
        pawnsW = 0
        pawnsB = 0
        queensW = 0
        queensB = 0
        bishopsDarkW = 0
        bishopsLightW = 0
        bishopsDarkB = 0
        bishopsLightB = 0
        for x in range(0,8):
            for y in range(0,8):
                #White#
                if self.board[x][y]=='Q':
                    pieceCountW=pieceCountW+1
                    queensW=queensW+1
                if self.board[x][y]=='B':
                    pieceCountW=pieceCountW+1
                    bishopsW=bishopsW+1
                    if (x+y)%2==0:
                        bishopsLightW=bishopsLightW+1
                    else:
                        bishopsDarkW=bishopsDarkW+1
                if self.board[x][y]=='N':
                    pieceCountW=pieceCountW+1
                    knightsW=knightsW+1
                if self.board[x][y]=='R':
                    pieceCountW=pieceCountW+1
                    rooksW=rooksW+1
                if self.board[x][y]=='P':
                    pieceCountW=pieceCountW+1
                    pawnsW=pawnsW+1
                #Black#
                if self.board[x][y]=='q':
                    pieceCountB=pieceCountB+1
                    queensB=queensB+1
                if self.board[x][y]=='b':
                    pieceCountB=pieceCountB+1
                    bishopsB=bishopsB+1
                    if (x+y)%2==0:
                        bishopsLightB=bishopsLightB+1
                    else:
                        bishopsDarkB=bishopsDarkB+1
                if self.board[x][y]=='n':
                    pieceCountB=pieceCountB+1
                    knightsB=knightsB+1
                if self.board[x][y]=='r':
                    pieceCountB=pieceCountB+1
                    rooksB=rooksB+1
                if self.board[x][y]=='p':
                    pieceCountB=pieceCountB+1
                    pawnsB=pawnsB+1
        #1. knight vs knight
        if knightsW==1 and pieceCountW==1 and knightsB==1 and pieceCountB==1:
            return True
        #2. knight vs bishop
        elif knightsW==1 and pieceCountW==1 and bishopsB==1 and pieceCountB==1:
            return True
        #3. bishop vs knight
        elif bishopsW==1 and pieceCountW==1 and knightsB==1 and pieceCountB==1:
            return True
        #4. bishop vs bishop
        elif bishopsW==1 and pieceCountW==1 and bishopsB==1 and pieceCountB==1:
            return True
        #5. king vs king
        elif pieceCountW==0 and pieceCountB==0:
            return True
        #6. king vs knight
        elif pieceCountW==0 and knightsB==1 and pieceCountB==1:
            return True
        #7. knight vs king
        elif pieceCountB==0 and knightsW==1 and pieceCountW==1:
            return True
        #8. king vs bishop
        elif pieceCountW==0 and bishopsB==1 and pieceCountB==1:
            return True
        #9. bishop vs king
        elif pieceCountB==0 and bishopsW==1 and pieceCountW==1:
            return True
        #10. two knights vs king
        elif knightsW==2 and pieceCountW==2 and pieceCountB==0:
            return True
        #11. king vs two knights
        elif knightsB==2 and pieceCountB==2 and pieceCountW==0:
            return True
        #12. two bishops same color vs king
        elif (bishopsDarkW==2 and bishopsLightW==0 and pieceCountW==2 and pieceCountB==0) or (bishopsLightW==2 and bishopsDarkW==0 and pieceCountW==2 and pieceCountB==0) or (bishopsLightB==2 and bishopsDarkB==0 and pieceCountB==2 and pieceCountW==0) or (bishopsDarkB==2 and bishopsLightB==0 and pieceCountB==2 and pieceCountW==0):
            return True
        else:
            return False
               
    def cleanIllegalMoves(self,color):
        def moveBlocksCheck(originalPiece,blockingPiece,attackingPiece):
            tempBoard = copy.deepcopy(self.board)
            tempBoard[originalPiece.square.file][originalPiece.square.rank] = '.'
            tempBoard[blockingPiece.square.file][blockingPiece.square.rank] = blockingPiece.type
            if (self.pinnedStatus(tempBoard,blockingPiece.square.file,blockingPiece.square.rank)!='none'
            and self.pinnedStatus(self.board,originalPiece.square.file,originalPiece.square.rank)=='none'): #if this theoretical move places the piece in question in a pin (and it wasn't previously in a pin) then it has effectively blocked the attacking piece
                return True
            else:
                return False
                
        if color=='w':
            #Remove all king moves to square with opposite piece targeters
            for x in range(0,8):
                for y in range(0,8):
                    if (len(self.dynamicBoard[x][y].blackTargeters)!=0 or len(self.dynamicBoard[x][y].blackZombieTargeters)!=0):
                        h=0
                        while h<len(self.dynamicBoard[x][y].nextMovePieces):
                            if self.dynamicBoard[x][y].nextMovePieces[h].type=='K':
                                self.dynamicBoard[x][y].nextMovePieces.remove(self.dynamicBoard[x][y].nextMovePieces[h])
                            else:
                                h=h+1
            if self.inCheck('w'):
                #1. find who is targeting king
                piecesCausingCheck = itertools.chain(self.dynamicBoard[self.kingPosW.file][self.kingPosW.rank].blackTargeters,self.dynamicBoard[self.kingPosW.file][self.kingPosW.rank].blackZombieTargeters)
                piecesCausingCheck = list(piecesCausingCheck)
                #2. if targeters > 1 then only moves are legal king moves
                if len(piecesCausingCheck)>1:
                    for x in range(0,8):
                        for y in range(0,8):
                            h=0
                            while h<len(self.dynamicBoard[x][y].nextMovePieces):
                                #remove moves that are not king moves
                                if self.dynamicBoard[x][y].nextMovePieces[h].type!='K':
                                    self.dynamicBoard[x][y].nextMovePieces.remove(self.dynamicBoard[x][y].nextMovePieces[h])
                                else:
                                    h=h+1
                #3. if knight then only captures and king moves
                elif len(piecesCausingCheck)==1 and piecesCausingCheck[0].type=='n':
                    for x in range(0,8):
                        for y in range(0,8):
                            h=0
                            while h<len(self.dynamicBoard[x][y].nextMovePieces):
                                #remove moves that are not king moves or moves that remove knight piece
                                if (self.dynamicBoard[x][y].nextMovePieces[h].type!='K' and (x!=piecesCausingCheck[0].square.file or y!=piecesCausingCheck[0].square.rank)):
                                    self.dynamicBoard[x][y].nextMovePieces.remove(self.dynamicBoard[x][y].nextMovePieces[h])
                                else:
                                    h=h+1
                    
                #4. all other situations must include king moves, captures, and blocks
                else:
                    for x in range(0,8):
                        for y in range(0,8):
                            h=0
                            while h<len(self.dynamicBoard[x][y].nextMovePieces):
                                #remove moves that are not king moves, or moves that remove the piece, or moves that are not in line with the king and that piece
                                if (self.dynamicBoard[x][y].nextMovePieces[h].type!='K'
                                and (x!=piecesCausingCheck[0].square.file or y!=piecesCausingCheck[0].square.rank)
                                and moveBlocksCheck(Piece(self.dynamicBoard[x][y].nextMovePieces[h].type,Square(self.dynamicBoard[x][y].nextMovePieces[h].square.file,self.dynamicBoard[x][y].nextMovePieces[h].square.rank)),Piece(self.dynamicBoard[x][y].nextMovePieces[h].type,Square(x,y)),piecesCausingCheck[0])==False):
                                    self.dynamicBoard[x][y].nextMovePieces.remove(self.dynamicBoard[x][y].nextMovePieces[h])
                                else:
                                    h=h+1
                            
                    
        if color=='b':
            #Remove all king moves to square with opposite piece targeters
            for x in range(0,8):
                for y in range(0,8):
                    if (len(self.dynamicBoard[x][y].whiteTargeters)!=0 or len(self.dynamicBoard[x][y].whiteZombieTargeters)!=0):
                        h=0
                        while h<len(self.dynamicBoard[x][y].nextMovePieces):
                            if self.dynamicBoard[x][y].nextMovePieces[h].type=='k':
                                self.dynamicBoard[x][y].nextMovePieces.remove(self.dynamicBoard[x][y].nextMovePieces[h])
                            else:
                                h=h+1
            if self.inCheck('b'):
                #1. find who is targeting king
                piecesCausingCheck = itertools.chain(self.dynamicBoard[self.kingPosB.file][self.kingPosB.rank].whiteTargeters,self.dynamicBoard[self.kingPosB.file][self.kingPosB.rank].whiteZombieTargeters)
                piecesCausingCheck = list(piecesCausingCheck)
                #2. if targeters > 1 then only moves are legal king moves
                if len(piecesCausingCheck)>1:
                    for x in range(0,8):
                        for y in range(0,8):
                            h=0
                            while h<len(self.dynamicBoard[x][y].nextMovePieces):
                                #remove moves that are not king moves
                                if self.dynamicBoard[x][y].nextMovePieces[h].type!='k':
                                    self.dynamicBoard[x][y].nextMovePieces.remove(self.dynamicBoard[x][y].nextMovePieces[h])
                                else:
                                    h=h+1
                #3. if knight then only captures and king moves
                elif len(piecesCausingCheck)==1 and piecesCausingCheck[0].type=='N':
                    for x in range(0,8):
                        for y in range(0,8):
                            h=0
                            while h<len(self.dynamicBoard[x][y].nextMovePieces):
                                #remove moves that are not king moves or moves that remove knight piece
                                if (self.dynamicBoard[x][y].nextMovePieces[h].type!='k' and (x!=piecesCausingCheck[0].square.file or y!=piecesCausingCheck[0].square.rank)):
                                    self.dynamicBoard[x][y].nextMovePieces.remove(self.dynamicBoard[x][y].nextMovePieces[h])
                                else:
                                    h=h+1
                    
                #4. all other situations must include king moves, captures, and blocks
                else:
                    for x in range(0,8):
                        for y in range(0,8):
                            h=0
                            while h<len(self.dynamicBoard[x][y].nextMovePieces):
                                #remove moves that are not king moves, or moves that remove the piece, or moves that are not in line with the king and that piece
                                print("line2003: if ("+str(self.dynamicBoard[x][y].nextMovePieces[h].type!='k')+" and ("+str(x!=piecesCausingCheck[0].square.file)+" or "+str(y!=piecesCausingCheck[0].square.rank)+") and "+str(moveBlocksCheck(Piece(self.dynamicBoard[x][y].nextMovePieces[h].type,Square(self.dynamicBoard[x][y].nextMovePieces[h].square.file,self.dynamicBoard[x][y].nextMovePieces[h].square.rank)),Piece(self.dynamicBoard[x][y].nextMovePieces[h].type,Square(x,y)),piecesCausingCheck[0]))+"==False):")
                                if (self.dynamicBoard[x][y].nextMovePieces[h].type!='k'
                                and (x!=piecesCausingCheck[0].square.file or y!=piecesCausingCheck[0].square.rank)
                                and moveBlocksCheck(Piece(self.dynamicBoard[x][y].nextMovePieces[h].type,Square(self.dynamicBoard[x][y].nextMovePieces[h].square.file,self.dynamicBoard[x][y].nextMovePieces[h].square.rank)),Piece(self.dynamicBoard[x][y].nextMovePieces[h].type,Square(x,y)),piecesCausingCheck[0])==False):
                                    print("removed: "+str(self.dynamicBoard[x][y].nextMovePieces[h].type)+"("+str(self.dynamicBoard[x][y].nextMovePieces[h].square.file)+", "+str(self.dynamicBoard[x][y].nextMovePieces[h].square.rank)+") to "+str(x)+", "+str(y))
                                    self.dynamicBoard[x][y].nextMovePieces.remove(self.dynamicBoard[x][y].nextMovePieces[h])
                                else:
                                    h=h+1

    def listAll(self):
        for x in range(0,8):
            for y in range(0,8):
                self.listSquare(x,y)
        print('\n')
    
    def listSquare(self,file,rank):
        print("\n"+self.board[file][rank]+fileNumToLetter(file)+str(rankNumToActualRank(rank))+": ",end='')
        for h in range(len(self.dynamicBoard[file][rank].whiteTargeters)):
            if h==0:
                print("\n\tWhite Targeters: ",end='')
            print(self.dynamicBoard[file][rank].whiteTargeters[h].type+fileNumToLetter(self.dynamicBoard[file][rank].whiteTargeters[h].square.file)+str(rankNumToActualRank(self.dynamicBoard[file][rank].whiteTargeters[h].square.rank))+", ",end='')

        for h in range(len(self.dynamicBoard[file][rank].whiteZombieTargeters)):
            if h==0:
                print("\n\tWhite Zombie Targeters: ",end='')
            print(self.dynamicBoard[file][rank].whiteZombieTargeters[h].type+fileNumToLetter(self.dynamicBoard[file][rank].whiteZombieTargeters[h].square.file)+str(rankNumToActualRank(self.dynamicBoard[file][rank].whiteZombieTargeters[h].square.rank))+", ",end='')
        for h in range(len(self.dynamicBoard[file][rank].blackTargeters)):
            if h==0:
                print("\n\tBlack Targeters: ",end='')
            print(self.dynamicBoard[file][rank].blackTargeters[h].type+fileNumToLetter(self.dynamicBoard[file][rank].blackTargeters[h].square.file)+str(rankNumToActualRank(self.dynamicBoard[file][rank].blackTargeters[h].square.rank))+", ",end='')
        for h in range(len(self.dynamicBoard[file][rank].blackZombieTargeters)):
            if h==0:
                print("\n\tBlack Zombie Targeters: ",end='')
            print(self.dynamicBoard[file][rank].blackZombieTargeters[h].type+fileNumToLetter(self.dynamicBoard[file][rank].blackZombieTargeters[h].square.file)+str(rankNumToActualRank(self.dynamicBoard[file][rank].blackZombieTargeters[h].square.rank))+", ",end='')
        for h in range(len(self.dynamicBoard[file][rank].nextMovePieces)):
            if h==0:
                print("\n\tCan Move Here: ",end='')
            print(self.dynamicBoard[file][rank].nextMovePieces[h].type+fileNumToLetter(self.dynamicBoard[file][rank].nextMovePieces[h].square.file)+str(rankNumToActualRank(self.dynamicBoard[file][rank].nextMovePieces[h].square.rank))+", ",end='')

    def listAllMoves(self):
        for x in range(0,8):
            for y in range(0,8):
                for h in range(len(self.dynamicBoard[x][y].nextMovePieces)):
                    if h==0:
                        print("\n"+self.board[x][y]+fileNumToLetter(x)+str(rankNumToActualRank(y))+": ",end='')
                        print("\n\tCan Move Here: ",end='')
                    print(self.dynamicBoard[x][y].nextMovePieces[h].type+fileNumToLetter(self.dynamicBoard[x][y].nextMovePieces[h].square.file)+str(rankNumToActualRank(self.dynamicBoard[x][y].nextMovePieces[h].square.rank))+", ",end='')
        print()

    def displayBoard(self):
        print(" \ta\tb\tc\td\te\tf\tg\th")
        for x in range(0,8):
            print(str(8-x)+"\t",end='')
            for y in range(0,8):
                print(self.board[y][x],end='\t')
            print()
            print()               

    def listPositionFlags(self):
        print("CTM: "+str(self.CTM))
        print("WCK: "+str(self.WCK))
        print("WCQ: "+str(self.WCQ))
        print("BCK: "+str(self.BCK))
        print("BCQ: "+str(self.BCQ))
        print("EPS: "+str(self.EPS))
        print("FRC: "+str(self.FRC))
        print("MC:  "+str(self.MC))
        print("In-Check: "+str(self.inCheck(self.CTM)))
        print("Game State: "+self.GS)

    def move(self,move):
        #1. Create speculative board that will be used to create the new position object
        speculativeBoard = copy.deepcopy(self.board)
        #2. Make changes based on contents of Move object
        speculativeBoard[move.pieceStart.square.file][move.pieceStart.square.rank] = '.'
        speculativeBoard[move.pieceEnd.square.file][move.pieceEnd.square.rank] = move.pieceEnd.type
        #4. Check certain qualities of the move so as to update the right flags related to the position
        if self.CTM == 'w':
            #a. check if en passant is played and update the board accordingly
            if move.pieceStart.type=='P' and self.EPS.file==move.pieceEnd.square.file and self.EPS.rank==move.pieceEnd.square.rank:
                speculativeBoard[move.pieceEnd.square.file][move.pieceEnd.square.rank+1] = '.'
            #b. check if pawn pushed or piece captured for updates to FRC (fifty rule counter)
            if move.pieceStart.type=='P' or self.pieceColor(self.board[move.pieceEnd.square.file][move.pieceEnd.square.rank])=='b': #pawn push, capture
                #c. check for en passant and update related flags
                if move.pieceStart.type=='P' and abs(move.pieceStart.square.rank-move.pieceEnd.square.rank)==2: #if piece is pawn and the pawn has moved 2 squares
                    position = Position(speculativeBoard,'b',self.WCK,self.WCQ,self.BCK,self.BCQ,Square(move.pieceEnd.square.file,move.pieceEnd.square.rank+1),0,self.MC)
                else:
                    position = Position(speculativeBoard,'b',self.WCK,self.WCQ,self.BCK,self.BCQ,Square(-1,-1),0,self.MC)
            else: #else move is not a capture or pawn push and the FRC should be updated
                #d. check if move is castling and if castling is allowed
                if (move.pieceStart.type=='K'
                and move.pieceStart.square.file==4
                and move.pieceStart.square.rank==7
                and move.pieceEnd.square.file==6
                and move.pieceEnd.square.rank==7
                and self.inCheck('w')==False
                and len(self.dynamicBoard[5][7].blackTargeters)==0 and len(self.dynamicBoard[5][7].blackZombieTargeters)==0
                and len(self.dynamicBoard[6][7].blackTargeters)==0 and len(self.dynamicBoard[6][7].blackZombieTargeters)==0
                and self.board[5][7]=='.' and self.board[6][7]=='.'
                and self.WCK
                ):
                    #execute king side castling
                    speculativeBoard[7][7] = '.'
                    speculativeBoard[5][7] = 'R'
                    
                elif (move.pieceStart.type=='K'
                and move.pieceStart.square.file==4
                and move.pieceStart.square.rank==7
                and move.pieceEnd.square.file==2
                and move.pieceEnd.square.rank==7
                and self.inCheck('w')==False
                and len(self.dynamicBoard[3][7].blackTargeters)==0 and len(self.dynamicBoard[3][7].blackZombieTargeters)==0
                and len(self.dynamicBoard[2][7].blackTargeters)==0 and len(self.dynamicBoard[2][7].blackZombieTargeters)==0
                and self.board[3][7]=='.' and self.board[2][7]=='.' and self.board[1][7]=='.'
                and self.WCQ
                ):
                    #execute queen side castling
                    speculativeBoard[0][7] = '.'
                    speculativeBoard[3][7] = 'R'
                position = Position(speculativeBoard,'b',self.WCK,self.WCQ,self.BCK,self.BCQ,Square(-1,-1),self.FRC+1,self.MC)
        else:
            #a. check if en passant is played and update the board accordingly
            if move.pieceStart.type=='p' and self.EPS.file==move.pieceEnd.square.file and self.EPS.rank==move.pieceEnd.square.rank:
                speculativeBoard[move.pieceEnd.square.file][move.pieceEnd.square.rank-1] = '.'
            #b. check if pawn pushed or piece captured for updates to FRC (fifty rule counter)
            if move.pieceStart.type=='p' or self.pieceColor(self.board[move.pieceEnd.square.file][move.pieceEnd.square.rank])=='w': #pawn push, capture
                #c. check for en passant and update related flags
                if move.pieceStart.type=='p' and abs(move.pieceStart.square.rank-move.pieceEnd.square.rank)==2:
                    position = Position(speculativeBoard,'w',self.WCK,self.WCQ,self.BCK,self.BCQ,Square(move.pieceEnd.square.file,move.pieceEnd.square.rank-1),0,self.MC+1)
                else:
                    position = Position(speculativeBoard,'w',self.WCK,self.WCQ,self.BCK,self.BCQ,Square(-1,-1),0,self.MC+1)
            else:
                #d. check if move is castling and if castling is allowed
                if (move.pieceStart.type=='k'
                and move.pieceStart.square.file==4
                and move.pieceStart.square.rank==0
                and move.pieceEnd.square.file==6
                and move.pieceEnd.square.rank==0
                and self.inCheck('b')==False
                and len(self.dynamicBoard[5][0].whiteTargeters)==0 and len(self.dynamicBoard[5][0].whiteZombieTargeters)==0
                and len(self.dynamicBoard[6][0].whiteTargeters)==0 and len(self.dynamicBoard[6][0].whiteZombieTargeters)==0
                and self.board[5][0]=='.' and self.board[6][0]=='.'
                and self.BCK
                ):
                    #execute king side castling
                    speculativeBoard[7][0] = '.'
                    speculativeBoard[5][0] = 'r'
                    
                elif (move.pieceStart.type=='k'
                and move.pieceStart.square.file==4
                and move.pieceStart.square.rank==0
                and move.pieceEnd.square.file==2
                and move.pieceEnd.square.rank==0
                and self.inCheck('b')==False
                and len(self.dynamicBoard[3][0].whiteTargeters)==0 and len(self.dynamicBoard[3][0].whiteZombieTargeters)==0
                and len(self.dynamicBoard[2][0].whiteTargeters)==0 and len(self.dynamicBoard[2][0].whiteZombieTargeters)==0
                and self.board[3][0]=='.' and self.board[2][0]=='.' and self.board[1][0]=='.'
                and self.BCQ
                ):
                    #execute queen side castling
                    speculativeBoard[0][0] = '.'
                    speculativeBoard[3][0] = 'r'
                position = Position(speculativeBoard,'w',self.WCK,self.WCQ,self.BCK,self.BCQ,Square(-1,-1),self.FRC+1,self.MC+1)
        return position
    
    #Deprecated Functions
    '''def scanDirectionComplex(self,file,rank,color,fileIncrement,rankIncrement):
        pieceList=[]    #collection of pieces in this scanned direction
        onBoard=range(0,8)
        ##White##
        if color=='w':
            #1. diagonals
            if fileIncrement!=0 and rankIncrement!=0:
                #a. scan northeast (file(x) larger, rank(y) smaller)
                #b. scan northwest (file(x) smaller, rank(y) smaller)
                if (fileIncrement==1 and rankIncrement==-1) or (fileIncrement==-1 and rankIncrement==-1):
                    boolTemp=True
                    x=file          #placeholder for file
                    y=rank          #and rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                        if self.board[x][y]=='K':
                            pieceList.append(Piece('K',Square(x,y)))
                        #Note: white pawns face north so if you scan north and there is a pawn infront then it is not targeting this square
                        if self.board[x][y]!='B' and self.board[x][y]!='Q' and self.board[x][y]!='K' and self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp):
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                        if self.board[x][y]!='B' and self.board[x][y]!='Q' and self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
                #c. scan southeast (file(x) larger, rank(y) larger)
                #d. scan southwest (file(x) smaller, rank(y) larger)
                if (fileIncrement==1 and rankIncrement==1) or (fileIncrement==-1 and rankIncrement==1):
                    boolTemp=True
                    x=file          #placeholder for file
                    y=rank          #and rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                        if self.board[x][y]=='K':
                            pieceList.append(Piece('K',Square(x,y)))
                        if self.board[x][y]=='P':
                            pieceList.append(Piece('P',Square(x,y)))
                        #Note: white pawns face north so if you scan south and there is a pawn behind (diagonally) then it is targeting this square
                        if self.board[x][y]!='B' and self.board[x][y]!='Q' and self.board[x][y]!='K' and self.board[x][y]!='P' and self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and king
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp):
                        if self.board[x][y]=='B':
                            pieceList.append(Piece('B',Square(x,y)))
                        if self.board[x][y]=='Q':
                            pieceList.append(Piece('Q',Square(x,y)))
                        if self.board[x][y]!='B' and self.board[x][y]!='Q' and self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
            #2. ranks and files
            if fileIncrement==0 or rankIncrement==0:
            #a. scan file right
            #b. scan file left
            #c. scan rank down (increasing)
            #d. scan rank up (decreasing)
                boolTemp=True
                x=file          #placeholder for file
                y=rank          #and rank
                x=x+fileIncrement
                y=y+rankIncrement
                if x in onBoard and y in onBoard:
                #1. first iteration includes king
                    if self.board[x][y]=='R':
                        pieceList.append(Piece('R',Square(x,y)))
                    if self.board[x][y]=='Q':
                        pieceList.append(Piece('Q',Square(x,y)))
                    if self.board[x][y]=='K':
                        pieceList.append(Piece('K',Square(x,y)))
                    if self.board[x][y]!='R' and self.board[x][y]!='Q' and self.board[x][y]!='K' and self.board[x][y]!='.':
                        boolTemp=False
                #2. additional iterations exclude pawns and king
                x=x+fileIncrement
                y=y+rankIncrement
                while(x in onBoard and y in onBoard and boolTemp):
                    if self.board[x][y]=='R':
                        pieceList.append(Piece('R',Square(x,y)))
                    if self.board[x][y]=='Q':
                        pieceList.append(Piece('Q',Square(x,y)))
                    if self.board[x][y]!='R' and self.board[x][y]!='Q' and self.board[x][y]!='.':
                        break
                    x=x+fileIncrement
                    y=y+rankIncrement
            
        ##Black##
        if color=='b':
             #1. diagonals
            if fileIncrement!=0 and rankIncrement!=0:
                #a. scan northeast (file(x) larger, rank(y) smaller)
                #b. scan northwest (file(x) smaller, rank(y) smaller)
                if (fileIncrement==1 and rankIncrement==-1) or (fileIncrement==-1 and rankIncrement==-1):
                    boolTemp=True
                    x=file          #placeholder for file
                    y=rank          #and rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                        if self.board[x][y]=='k':
                            pieceList.append(Piece('k',Square(x,y)))
                        if self.board[x][y]=='p':
                            pieceList.append(Piece('p',Square(x,y)))
                        #Note: black pawns face south so if you scan north (behind) and there is a pawn there then it is targeting this square
                        if self.board[x][y]!='b' and self.board[x][y]!='q' and self.board[x][y]!='k' and self.board[x][y]!='p' and self.board[x][y]!='.':
                            boolTemp=False
                    x=x+fileIncrement
                    y=y+rankIncrement
                    #2. additional iterations exclude pawns and king
                    while(x in onBoard and y in onBoard and boolTemp):
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                        if self.board[x][y]!='b' and self.board[x][y]!='q' and self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
                #c. scan southeast (file(x) larger, rank(y) larger)
                #d. scan southwest (file(x) smaller, rank(y) larger)
                if (fileIncrement==1 and rankIncrement==1) or (fileIncrement==-1 and rankIncrement==1):
                    boolTemp=True
                    x=file          #placeholder for file
                    y=rank          #and rank
                    x=x+fileIncrement
                    y=y+rankIncrement
                    if x in onBoard and y in onBoard:
                    #1. first iteration includes pawns and king
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                        if self.board[x][y]=='k':
                            pieceList.append(Piece('k',Square(x,y)))
                        #Note: black pawns face south so if you scan south (infront) then you are behind the pawn and it is not defending you
                        if self.board[x][y]!='b' and self.board[x][y]!='q' and self.board[x][y]!='k' and self.board[x][y]!='.':
                            boolTemp=False
                    #2. additional iterations exclude pawns and kingx=x+fileIncrement
                    x=x+fileIncrement
                    y=y+rankIncrement
                    while(x in onBoard and y in onBoard and boolTemp):
                        if self.board[x][y]=='b':
                            pieceList.append(Piece('b',Square(x,y)))
                        if self.board[x][y]=='q':
                            pieceList.append(Piece('q',Square(x,y)))
                        if self.board[x][y]!='b' and self.board[x][y]!='q' and self.board[x][y]!='.':
                            break
                        x=x+fileIncrement
                        y=y+rankIncrement
            #2. ranks and files
            if fileIncrement==0 or rankIncrement==0:
            #a. scan file right
            #b. scan file left
            #c. scan rank down (increasing)
            #d. scan rank up (decreasing)
                boolTemp=True
                x=file          #placeholder for file
                y=rank          #and rank
                x=x+fileIncrement
                y=y+rankIncrement
                if x in onBoard and y in onBoard:
                #1. first iteration includes king
                    if self.board[x][y]=='r':
                        pieceList.append(Piece('r',Square(x,y)))
                    if self.board[x][y]=='q':
                        pieceList.append(Piece('q',Square(x,y)))
                    if self.board[x][y]=='k':
                        pieceList.append(Piece('k',Square(x,y)))
                    if self.board[x][y]!='r' and self.board[x][y]!='q' and self.board[x][y]!='k' and self.board[x][y]!='.':
                        boolTemp=False
                #2. additional iterations exclude pawns and king
                x=x+fileIncrement
                y=y+rankIncrement
                while(x in onBoard and y in onBoard and boolTemp):
                    if self.board[x][y]=='r':
                        pieceList.append(Piece('r',Square(x,y)))
                    if self.board[x][y]=='q':
                        pieceList.append(Piece('q',Square(x,y)))
                    if self.board[x][y]!='r' and self.board[x][y]!='q' and self.board[x][y]!='.':
                        break
                    x=x+fileIncrement
                    y=y+rankIncrement
        return pieceList'''
    

##GENERAL FUNCTIONS##
def welcomeText():
    print("Welcome to MyChessEngine!\n")

def pickRandomColors():
    color = random.randrange(0,2)
    if color==0:
        return 'w','b'
    else:
        return 'b', 'w'

    def pieceColor(pieceString):
        whitePieces = ['K','Q','B','N','R','P']
        blackPieces = ['k','q','b','n','r','p']
        if pieceString in whitePieces: 
            return 'w'
        if pieceString in blackPieces:
            return 'b'
        else:
            return ''

def fileLetterToNum(fileLetter):
    if fileLetter == 'a':
        return 0
    if fileLetter == 'b':
        return 1
    if fileLetter == 'c':
        return 2
    if fileLetter == 'd':
        return 3
    if fileLetter == 'e':
        return 4
    if fileLetter == 'f':
        return 5
    if fileLetter == 'g':
        return 6
    if fileLetter == 'h':
        return 7

def fileNumToLetter(fileNum):
    if fileNum == 0:
        return 'a'
    if fileNum == 1:
        return 'b'
    if fileNum == 2:
        return 'c'
    if fileNum == 3:
        return 'd'
    if fileNum == 4:
        return 'e'
    if fileNum == 5:
        return 'f'
    if fileNum == 6:
        return 'g'
    if fileNum == 7:
        return 'h'

def rankNumToActualRank(rankNum):
    if rankNum == 0:
        return 8
    if rankNum == 1:
        return 7
    if rankNum == 2:
        return 6
    if rankNum == 3:
        return 5
    if rankNum == 4:
        return 4
    if rankNum == 5:
        return 3
    if rankNum == 6:
        return 2
    if rankNum == 7:
        return 1

def actualRankToRankNum(actualRank):
    if actualRank == 8:
        return 0
    if actualRank == 7:
        return 1
    if actualRank == 6:
        return 2
    if actualRank == 5:
        return 3
    if actualRank == 4:
        return 4
    if actualRank == 3:
        return 5
    if actualRank == 2:
        return 6
    if actualRank == 1:
        return 7

def FENToPosition(FENString):
    FENString = re.split('/| ', FENString)
    board = np.array([['.']*8]*8)
    CTM='w'
    WCK=False
    WCQ=False
    BCK=False
    BCQ=False
    EPS=Square(-1,-1)
    FRC=0
    MC=0
    
    #1.Pieces
    for x in range(0,8):
        file=0
        rank=0+x
        for y in range(len(FENString[x])):
            if FENString[x][y] not in ['1','2','3','4','5','6','7','8']:
                board[file][rank]=FENString[x][y]
                file=file+1
            else:
                #if it is a number then we need to jump that many spaces
                file=file+int(FENString[x][y])
    #2. ColorToMove
    CTM=FENString[8][0]
    #3. Castling
    if 'K' in FENString[9]:
        WCK=True
    if 'Q' in FENString[9]:
        WCQ=True
    if 'k' in FENString[9]:
        BCK=True
    if 'q' in FENString[9]:
        BCQ=True
    if FENString[9]=='-':
        WCK=False
        WCQ=False
        BCK=False
        BCQ=False
    #4. En Passant
    if FENString[10][0]=='-':
        EPS=Square(-1,-1)
    else:
        EPS=Square(fileLetterToNum(FENString[10][0]),actualRankToRankNum(int(FENString[10][1])))
    #5. 50 move rule
    FRC=int(FENString[11])
    #6. Number of complete moves
    MC=int(FENString[12])
    
    position = Position(board,CTM,WCK,WCQ,BCK,BCQ,EPS,FRC,MC)
    return position

def setupDefaultPosition():
    #default position: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    return FENToPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

def moveObjToMoveNotation(move,previousPosition,currentPosition):
    if move is None or currentPosition is None or previousPosition is None:
        return "NULL"
    else:
        if (move.pieceStart.type=='K'
        and move.pieceStart.square.file==4
        and move.pieceStart.square.rank==7
        and move.pieceEnd.square.file==6
        and move.pieceEnd.square.rank==7):
            return "o-o"
        elif (move.pieceStart.type=='K'
        and move.pieceStart.square.file==4
        and move.pieceStart.square.rank==7
        and move.pieceEnd.square.file==2
        and move.pieceEnd.square.rank==7):
            return "o-o-o"
        elif (move.pieceStart.type=='k'
        and move.pieceStart.square.file==4
        and move.pieceStart.square.rank==0
        and move.pieceEnd.square.file==6
        and move.pieceEnd.square.rank==0):
            return "o-o"
        elif (move.pieceStart.type=='k'
        and move.pieceStart.square.file==4
        and move.pieceStart.square.rank==0
        and move.pieceEnd.square.file==2
        and move.pieceEnd.square.rank==0):
            return "o-o-o"
        else:
            #move: move made
            #previousPosition: position before move made
            #currentPosition: resulting current position
            startingSquare = Square(move.pieceStart.square.file,move.pieceStart.square.rank)
            endingSquare = Square(move.pieceEnd.square.file,move.pieceEnd.square.rank)
            pieceTypeStart = move.pieceStart.type
            pieceTypeEnd = move.pieceEnd.type
            #1. Piece Type
            MoveNotation = pieceTypeStart
            #2. disambiguation
            #you know the move just check the square to see if there are any other pieces targeting that square and compare how they match to the piece you are moving
            fileDisambig = False
            rankDisambig = False
            for x in range(len(previousPosition.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces)):
                if (previousPosition.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[x].square.file!=move.pieceStart.square.file or previousPosition.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[x].square.rank!=move.pieceStart.square.rank) and previousPosition.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[x].type==pieceTypeStart:
                    if previousPosition.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[x].square.file==startingSquare.file:
                        rankDisambig = True
                    elif previousPosition.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[x].square.rank==startingSquare.rank:
                        fileDisambig = True
            if fileDisambig and pieceTypeStart==pieceTypeEnd:
                MoveNotation = MoveNotation+fileNumToLetter(startingSquare.file)
            if rankDisambig and pieceTypeStart==pieceTypeEnd:
                MoveNotation = MoveNotation+str(rankNumToActualRank(startingSquare.rank))
            #3. Capture
            if previousPosition.board[endingSquare.file][endingSquare.rank]!='.':
                MoveNotation = MoveNotation+"x"
            #4. Square
            MoveNotation = MoveNotation+fileNumToLetter(endingSquare.file)+str(rankNumToActualRank(endingSquare.rank))
            #5. Promotion
            if pieceTypeStart!=pieceTypeEnd:
                MoveNotation = MoveNotation+"="+pieceTypeEnd
            #6. Check or Checkmate
            if currentPosition.inCheck(currentPosition.CTM) and currentPosition.GS!='whiteWin' and currentPosition.GS!='blackWin':
                MoveNotation = MoveNotation+"+"
            elif currentPosition.GS=='whiteWin' or currentPosition.GS=='blackWin':
                MoveNotation = MoveNotation+"#"
            return MoveNotation

def moveNotationToLegalMoveObj(moveString,position):
    if re.search("^[Q,K,B,N,R,P,q,k,b,n,r,p][a,b,c,d,e,f,g,h]?[1,2,3,4,5,6,7,8]?x?[a,b,c,d,e,f,g,h][1,2,3,4,5,6,7,8][+|#]?$|^Px?[a,b,c,d,e,f,g,h]8=[Q,B,N,R][+|#]?$|^px?[a,b,c,d,e,f,g,h]1=[q,b,n,r][+|#]?$|^o-o$|^o-o-o$",moveString) is not None and position is not None:
        if re.search("^[Q,K,B,N,R,P]",moveString) is not None and position.CTM=='b':
            return -1
        if re.search("^[q,k,b,n,r,p]",moveString) is not None and position.CTM=='w':
            return -1
        #0. if castling then create special move object
        if moveString=="o-o" and position.CTM=='w':
            if position.WCK==False:
                return -1
            else:
                return Move(Piece('K',Square(4,7)),Piece('K',Square(6,7)))
        if moveString=="o-o-o" and position.CTM=='w':
            if position.WCQ==False:
                return -1
            else:
                return Move(Piece('K',Square(4,7)),Piece('K',Square(2,7)))
        if moveString=="o-o" and position.CTM=='b':
            if position.BCK==False:
                return -1
            else:
                return Move(Piece('k',Square(4,0)),Piece('k',Square(6,0)))
        if moveString=="o-o-o" and position.CTM=='b':
            if position.BCQ==False:
                return -1
            else:
                return Move(Piece('k',Square(4,0)),Piece('k',Square(2,0)))
        #1. remove + or # as they are not important to move calculation
        pieceTypeStart = moveString[:1]
        if moveString[-1:] == '+' or moveString[-1:] == '#':
            moveString = moveString[:-1]
        #2. remove x since it is not critical to move calculation
        if moveString.find('x')!=-1:
            moveString = moveString.replace('x','')
        #3. check to see if there is an = which would imply a promotion move
        promotionBool=False
        if moveString.find('=')!=-1:
            pieceTypeEnd = moveString[-1:]
            moveString = moveString[:-2] #remove the '=Q' at the end
            promotionBool=True
        else:
            pieceTypeEnd = pieceTypeStart
        #4. get the ending square
        endingSquare = Square(fileLetterToNum(moveString[-2:-1]),actualRankToRankNum(int(moveString[-1:])))
        startingSquare = None
        #5. disambiguation
        if (len(moveString)==3 or promotionBool):
            #a. no disambiguation so piece starting square can be found in the position
            for h in range(len(position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces)):
                if position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].type==pieceTypeStart:
                    startingSquare = Square(position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].square.file,position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].square.rank)
        elif len(moveString)==4:
            #b.1 file disambiuation so piece starting square can be found in specified file of position
            if moveString[1:2] in ['a','b','c','d','e','f','g','h']:
                for h in range(len(position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces)):
                    if position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].type==pieceTypeStart and position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].square.file==fileLetterToNum(moveString[1:2]):
                        startingSquare = Square(position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].square.file,position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].square.rank)
            #b.2 rank disambiuation so piece starting square can be found in specified rank of position
            else:
                for h in range(len(position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces)):
                    if position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].type==pieceTypeStart and position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].square.rank==actualRankToRankNum(moveString[1:2]):
                        startingSquare = Square(position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].square.file,position.dynamicBoard[endingSquare.file][endingSquare.rank].nextMovePieces[h].square.rank)
        elif len(moveString)==5:
            #c. double disambiguation so starting square is explicit in move notation
            startingSquare = Square(fileLetterToNum(moveString[1:2]),actualRankToRankNum(moveString[2:3]))
        else:
            print("a critical error has occured: moveString length was not 3,4, or 5")
            print("MoveString: "+str(moveString))
        if startingSquare is None:
            return -1
        else:
            return Move(Piece(pieceTypeStart,Square(startingSquare.file,startingSquare.rank)),Piece(pieceTypeEnd,Square(endingSquare.file,endingSquare.rank)))
    else: #moveString is invalid or position is None
        return -1

##DEBUG##
start_time = time.time()
currentPosition=FENToPosition("rnbqkb1r/ppp1pppp/5n2/1B1p4/8/2N1P3/PPPP1PPP/R1BQK1NR b KQkq - 1 3")
print("\n---\t\tPOSITION\t\t---")
currentPosition.displayBoard()
currentPosition.listAllMoves()
print("\n---Position Flags---")
currentPosition.listPositionFlags()
exit()

##PROGRAM##
#1. Setup
welcomeText()
userColor,computerColor = pickRandomColors()
#userColor='b'
#computerColor='w'
if userColor=='w':
    print("You are white")
else:
    print("You are black")

#2. Start Game
turn = 'w'
gameEnd = False
game = Game(setupDefaultPosition())
while not gameEnd:
    #Whose turn is it?
    if turn=='w':
        print("White to move")
    else:
        print("Black to move")
 
    if turn==userColor:
        #user moves     
        game.displayBoard()
        userMove=moveNotationToLegalMoveObj(input("Your move: "),game.currentPosition)
        if userMove==-1:
            print("Invalid move or move format")
            continue
        game.move(userMove)
    else:
        #computer moves
        game.calculateBestMoveAndEval(3)
        game.printBestMove(0)
        game.move(game.bestMove)
    #Change turns
    if turn=='w':
        turn='b'
    else:
        turn='w'
    #check game state
    if game.GS=='in-play':
        gameEnd=False
    else:
        game.displayBoard()
        if game.GS=='whiteWin':
            print("White has won.")
        elif game.GS=='blackWin':
            print("Black has won.")
        elif game.GS=='draw50':
            print("Draw (by 50 move rule)")
        elif game.GS=='stalemate':
            print("Draw (by stalemate)")
        elif game.GS=='insufficient':
            print("Draw (by insufficient material)")
        else:
            print("Error 2603")
        gameEnd=True
