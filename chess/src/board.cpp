#include "board.h"
#include "tile.h"
#include "def.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;



Board::Board(){

    //  Using FEN we can write any position and compute it easily 

    //string startingPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    //string startingPosition = "8/2p5/8/KP5r/8/8/8/7k b - - 0 1";
    string startingPosition = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";

    
    parseFENPosition(startingPosition);

    //  Here I'm initialising the position in the window of the board, used in drag and drop (although not used now)

    Position startingPos = Position(STARTING_POSX, STARTING_POSY);

    for(int i = 0; i < BOARD; i++){
        startingPos.x = STARTING_POSX;

        for(int j = 0; j < BOARD; j++){
            posTiles[i][j] = startingPos;
            tiles[i][j].currentPos = {i, j};

            startingPos.x += TILESIZE;
        }
        startingPos.y += TILESIZE;
    }

    positionGame.push_back(startingPosition);

}


Board::Board(const Board& other) {
    // 1. Copy `tiles` and `posTiles`
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            tiles[i][j] = other.tiles[i][j];
            posTiles[i][j] = other.posTiles[i][j];
        }
    }

    // 2. Copy vectors
    posOfPieces = other.posOfPieces;
    positionGame = other.positionGame;

    // 3. Copy strings and simple attributes
    lastPosition = other.lastPosition;
    nbMoves = other.nbMoves;
    turnToMove = other.turnToMove;
    nbHalfMoves = other.nbHalfMoves;

    // 4. enPassant
    enPassantActive = other.enPassantActive;
    enPassantTarget = other.enPassantTarget;

/*
    // 5. Copy `textures`
    if (other.textures) {
        textures = new SDL_Texture*[13]; // Assuming 13 textures
        for (int i = 0; i < 13; ++i) {
            textures[i] = other.textures[i]; // Shallow copy (adjust for deep copy if needed)
        }
    } else {
        textures = nullptr;
    }
*/
}

//  Insha allah no memory leaks

Board::~Board(){}



void Board::changeTurn(){
    turnToMove = (turnToMove == WHITE) ? BLACK : WHITE;
}


















std::string Board::posToFen(){
    string fen = "";

    for(int i = 0; i < BOARD; i++){
        int emptyCount = 0;
        for(int j = 0; j < BOARD; j++){
            Tile t = tiles[i][j];
            if(t.piece == NONE){
                emptyCount++;
            } else{
                if(emptyCount > 0){
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                fen += pieceToFen(t);
            }
        }
        if(emptyCount > 0) fen += std::to_string(emptyCount);
        if (i < 7) fen += '/';
    }

    //  Color to move
    fen += " ";
    fen += (turnToMove == WHITE) ? 'w' : 'b';

    //  Castling rights
    fen += " ";
    fen += "-";

    //  En passant
    fen += " ";
    fen += "-";

    //  HalfMoves
    fen += " ";
    fen += std::to_string(nbHalfMoves);

    //  Moves
    fen += " ";
    fen += std::to_string(nbMoves);

    return fen;
}



char pieceToFen(Tile t){
    int color = (t.color == WHITE) ? 0 : 32;
    switch (t.piece){
        case PAWN:   return 'P' + color;
        case KNIGHT: return 'N' + color;
        case BISHOP: return 'B' + color;
        case ROOK:   return 'R' + color;
        case QUEEN:  return 'Q' + color;
        case KING:   return 'K' + color;
        default:     return '.';
    }
}



//  This initialise the piece array and their positions in board depending on the starting fen pos

void Board::parseFENPosition(string position){

    //  Used a hashMap because gpt said so apparently its O(1) to access elements

    std::unordered_map<char, std::pair<int, int>> pieceMap = {
        {'p', {PAWN, BLACK}}, {'n', {KNIGHT, BLACK}}, {'b', {BISHOP, BLACK}}, {'r', {ROOK, BLACK}},
        {'q', {QUEEN, BLACK}}, {'k', {KING, BLACK}}, {'P', {PAWN, WHITE}}, {'N', {KNIGHT, WHITE}},
        {'B', {BISHOP, WHITE}}, {'R', {ROOK, WHITE}}, {'Q', {QUEEN, WHITE}}, {'K', {KING, WHITE}}
    };

    vector<string> fenPosition;
    fenPosition = split(position, ' ');

    if(fenPosition.size() < 6){
        std::cout << "Something went wrong in the split in fenToPosition\n";
        exit(1);
    }


    string realPosition = fenPosition[0];

    unsigned int iTab = 0, jTab = 0;

    for(char ch : realPosition){

        if(pieceMap.find(ch) != pieceMap.end()){

            //  affect these two variables with  the value in the hashmap
            auto [pieceType, color] = pieceMap[ch];

            tiles[iTab][jTab] = Tile(pieceType, color);
            jTab++;
        }

        //  When reaching '/' we go to the next array in [i][j]
        else if(ch == '/') {
            iTab++;
            jTab = 0;
        }

        //  When reaching a digit that means the number of empty tile tile the next piece are equal to the number
        else if (isdigit(ch)){
            int step = ch - '0';

            for(int k = 0; k < step; k++){
                tiles[iTab][jTab].piece = NONE;
                jTab++;
            }
        }
    }

    for(int i = 0; i < BOARD; i++){
        for(int j = 0; j < BOARD; j++){
            if(tiles[i][j].piece != NONE){
                posOfPieces.push_back({i, j});
            }
            tiles[i][j].currentPos = {i, j};
        }
    }

    //  Setting the pawns neverMoved
    for(int i = 0; i < 8; i++){
        tiles[1][i].neverMoved = (tiles[1][i].piece == PAWN) ? true : false;
        tiles[6][i].neverMoved = (tiles[6][i].piece == PAWN) ? true : false;
    }

    //  if 'w' white to move and vice versa
    turnToMove = (fenPosition[1] == "w") ? WHITE : BLACK;

    //  Castling Rights

    tiles[7][4].neverMoved = (fenPosition[2].find('K') != string::npos || (fenPosition[2].find('Q') != string::npos)) ? true : false;
    tiles[0][4].neverMoved = (fenPosition[2].find('k') != string::npos || (fenPosition[2].find('q') != string::npos)) ? true : false;

    tiles[7][7].neverMoved = (fenPosition[2].find('K') != string::npos) ? true : false;
    tiles[7][0].neverMoved = (fenPosition[2].find('Q') != string::npos) ? true : false;
    tiles[0][7].neverMoved = (fenPosition[2].find('k') != string::npos) ? true : false;
    tiles[0][0].neverMoved = (fenPosition[2].find('q') != string::npos) ? true : false;

    //  En passant
    enPassantActive = (fenPosition[3].find('-') != string::npos) ? false : true;

    if(enPassantActive){
        enPassantTarget = translateAlgebraic(fenPosition[3]);
    }
    else{
        enPassantTarget = {-1, -1};
    }


    lastPosition = position;
    nbHalfMoves = std::stoi(fenPosition[4]);
    nbMoves = std::stoi(fenPosition[5]);

}

//  basically e4 -> 4, 4 

Position translateAlgebraic(string s){
    if(s.length() != 2){
        cout << "A problem in the en passant of the fen" << endl;
    }

    int i = s[0] - 'a';
    int j = s[1] - '0';
    return {i, j};
}

std::vector<std::string> split(const std::string& str, char delimiter) {

    //  Split a string and return a vector of strings

    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}




















void Board::generateAllMoves(){
    //  Init all moves in the board, currently not used anywhere I hope I wont need it

    for(int i = 0; i < BOARD; i++){
        for(int j = 0; j < BOARD; j++){
            if(tiles[i][j].piece != NONE){
                generateMoves(tiles[i][j]);
            }
        }
    }
}

void Board::generateLegalMoves(Tile& tile){
    generateMoves(tile);
    tile.legalMoves = filterLegalMoves(tile);
}


//  this function init the moves of a Tile object in the tiles[i][j] attribute of a board
//  It store the position (i, j) of the array (tiles) into the legalMoves vector attribute of the tile

void Board::generateMoves(Tile& tile){

    //  Clearing the array of moves to avoid unwanted problems

    tile.legalMoves.clear();

    Position startingSquare = tile.currentPos;
    vector<Position> targetSquares;

    
    //  Lambda functions used to init the moves of a Pawn

    //  Init the moves of a pawn moving Forward

    auto addTargetSquare = [&](const Position& pos){
        if(notOutOfBonds(pos) && tiles[pos.x][pos.y].piece == NONE){
            targetSquares.push_back(pos);
        }
    };

    //  Capturing Moves of a Pawn

    auto addCaptureSquare = [&](const Position& pos){
        if(notOutOfBonds(pos) && tile.isEnnemy(tiles[pos.x][pos.y])){
            targetSquares.push_back(pos);
        }
    };

    switch(tile.piece){
        case PAWN:{

            //  Depends on the color 
            int direction = (tile.color == WHITE) ? -1 : 1;

            //  Pretty straightforward

            Position forward = {startingSquare.x + direction, startingSquare.y};
            addTargetSquare(forward);


            //  if the pawn never moved he can move up 2 squares
            if(tile.neverMoved && tiles[forward.x][forward.y].piece == NONE){
                Position forward = {startingSquare.x + (direction * 2), startingSquare.y};
                addTargetSquare(forward);
            }

            Position captureLeft = {startingSquare.x + direction, startingSquare.y - 1};
            Position captureRight = {startingSquare.x + direction, startingSquare.y + 1};

            if(captureLeft == enPassantTarget) targetSquares.push_back(captureLeft);
            if(captureRight == enPassantTarget) targetSquares.push_back(captureRight);

            addCaptureSquare(captureLeft);
            addCaptureSquare(captureRight);

            break;
        }
        case KNIGHT:{

            //  Init the moves of Knight for now its the only thing that works correctly

            const Position directions[8] = {{-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}};

            for(int i = 0; i < 8; i++){
                Position pos = {startingSquare.x + directions[i].x, startingSquare.y + directions[i].y};

                if(notOutOfBonds(pos) && (tile.notBlockedBy(tiles[pos.x][pos.y]))){
                    targetSquares.push_back(pos);
                } // || tile.isEnnemy(tiles[pos.x][pos.y]))
            }
            break;
        }
        case BISHOP:
        case ROOK:
        case QUEEN:{

            //  Init the moves of Sliding pieces
            //  TODO the moves of the king are almost the same just with more conditions and castle

            const vector<Position> directions = getDirectionSlidingPiece(tile);
            

            for(const Position& dir : directions){
                Position moving = {startingSquare.x , startingSquare.y};
                
                while(true){

                    moving.x += dir.x;
                    moving.y += dir.y;

                    if(!notOutOfBonds(moving)) break;

                    if(!tile.notBlockedBy(tiles[moving.x][moving.y])) break;
                    if(tile.isEnnemy(tiles[moving.x][moving.y])){
                        targetSquares.push_back(moving);
                        break;
                    }

                    targetSquares.push_back(moving);
                }
            }
            break;
        }

        case KING:{
            Position directions[8] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}};

            for(int i = 0; i < 8; i++){
                Position moving = {startingSquare.x + directions[i].x, startingSquare.y + directions[i].y};

                if(notOutOfBonds(moving) && (tile.notBlockedBy(tiles[moving.x][moving.y]))){
                    targetSquares.push_back(moving);
                }
            }

            tile.legalMoves = targetSquares;

            if(tile.neverMoved){
                tile.legalMoves = filterLegalMoves(tile);

                int direction = 2;

                Position castleKingSide = {startingSquare.x, startingSquare.y + direction};
                Position castleQueenSide = {startingSquare.x, startingSquare.y - direction};

                
                if(tile.notBlockedBy(tiles[castleKingSide.x][castleKingSide.y])){

                    if(tile.isInLegalMoves({castleKingSide.x, castleKingSide.y - 1}) && tiles[castleKingSide.x][castleKingSide.y + 1].neverMoved){

                        if(!isKingInCheck(tile.color)){
                            tile.legalMoves.push_back(castleKingSide);
                        }
                    }
                }
                if (tile.notBlockedBy(tiles[castleQueenSide.x][castleQueenSide.y])){
                    if(tile.isInLegalMoves({castleQueenSide.x, castleQueenSide.y + 1}) && tiles[castleQueenSide.x][castleQueenSide.y - 2].neverMoved){
                        if(!isKingInCheck(tile.color)){
                            tile.legalMoves.push_back(castleQueenSide);
                        }
                    }
                }
            }
            


            break;
        }
        default:
            break;
    }

    //  Ugly but works
    if(tile.piece != KING){
        tile.legalMoves = targetSquares;
    }
    
}



//  Out of Bonds 0 =< p < 8

bool notOutOfBonds(Position p){
    return p.x < BOARD && p.x >= 0 && p.y < BOARD && p.y >= 0;
}


//  return  the pos (forgot where its used) surely gonna delete it 

Position getTileTillOutBonds(Position p){
    return {BOARD - p.x, BOARD - p.y};
}


//  Return the directions in which a sliding piece moves

vector<Position> getDirectionSlidingPiece(Tile t){
    switch(t.piece){
        case BISHOP:
            return {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
        case ROOK:
            return {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        case QUEEN:
            return {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    }
    return {{0, 0}};
}
















//  This function is used to make a move in the board, the things that are changing are the board.tiles and board.posPiece
//  the tiles used as parameter, I choose to parse by reference to change board.tiles, it basically swap two tiles when making a move, or create a whole new tile with the piece needed

void Board::tryMakeMove(Tile& start, Tile& target){

    if(start.piece == KING){

        //  Castling (might create a new function for better readability)
        if(abs(target.currentPos.y - start.currentPos.y) == 2){

            int rookPosY = ((target.currentPos.y - start.currentPos.y) > 0) ? 1 : -2;
            int rookNewPos = (rookPosY > 0) ? -1 : 1;

            tiles[target.currentPos.x][target.currentPos.y + rookPosY].setPiece(NONE, -1);
            tiles[target.currentPos.x][target.currentPos.y + rookNewPos].setPiece(ROOK, start.color);

            target.setPiece(start);
            start.setPiece(NONE, -1);

            std::replace(posOfPieces.begin(), posOfPieces.end(), tiles[target.currentPos.x][target.currentPos.y + rookPosY].currentPos, tiles[target.currentPos.x][target.currentPos.y + rookNewPos].currentPos);
            std::replace(posOfPieces.begin(), posOfPieces.end(), start.currentPos, target.currentPos);

            return;
        }
    }

    if(start.piece == PAWN){
        int direction = (start.color == WHITE) ? -1 : 1;



        //  En passant move
        if(enPassantActive && (target.currentPos == enPassantTarget)){
            
            std::replace(posOfPieces.begin(), posOfPieces.end(), start.currentPos, target.currentPos);
            posOfPieces.erase(std::remove(posOfPieces.begin(), posOfPieces.end(), tiles[target.currentPos.x - direction][target.currentPos.y].currentPos), posOfPieces.end());

            target.setPiece(start);
            tiles[target.currentPos.x - direction][target.currentPos.y].setPiece(NONE, -1);
            start.setPiece(NONE, -1);

            enPassantActive = false;
            enPassantTarget = {-1, -1};

            return;
        }



        //  Double pawn push
        if(abs(target.currentPos.x - start.currentPos.x) == 2){
            target.setPiece(start);
            start.setPiece(NONE, -1);
            
            enPassantActive = true;
            enPassantTarget = {start.currentPos.x + direction, start.currentPos.y};

            return;
        }


        //  Pawn promotion

    }

    //  if another move is played when an enpassant is possible we loose the ability to do it

    if(enPassantActive){
        enPassantActive = false;
        enPassantTarget = {-1, 1};
    }

    //  Any move that is not enPassant, double pawn push or Castling

    target.setPiece(start);
    start.setPiece(NONE, -1);
    std::replace(posOfPieces.begin(), posOfPieces.end(), start.currentPos, target.currentPos);

}

void Board::makePromotion(Tile& target, int choice){

        int pieceChoosed = (choice == 0) ? QUEEN : (choice == 1) ? ROOK : (choice == 2) ? BISHOP : (choice == 3) ? KNIGHT : NONE;
        cout << "pieceChoosed: " << pieceChoosed << endl;

        target.setPiece(pieceChoosed, target.color);
    
}


//  Splited this concept into 2 different functions to use the one that really make moves in the filter legalMoves function
//  get the pos of the game to maybe in the future write it into a file that contains all the moves of the game

void Board::makeMove(Tile& start, Tile& target){
    lastPosition = posToFen();
    
    undoStack.push(lastPosition);

    tryMakeMove(start, target);

    while(!redoStack.empty()) redoStack.pop();

    std::replace(posOfPieces.begin(), posOfPieces.end(), start.currentPos, target.currentPos);
    changeTurn();

    positionGame.push_back(lastPosition);
}



void Board::undoMove(){
    if(!undoStack.empty()){
        string lastMove = undoStack.top();
        redoStack.push(posToFen());

        parseFENPosition(lastMove);
        undoStack.pop();
    }
    
}

void Board::redoMove(){
    if(!redoStack.empty()){
        string redo = redoStack.top();
        undoStack.push(posToFen());

        parseFENPosition(redo);
        redoStack.pop();
    }
}


//  its takes as a parameter a tile that is already initialised with pseudo-legal moves
//  for every legal moves we make them and create another board where the move happened and check every opponent responses
//  if one of the opponent response is a king capture we delete the move from the tile from the parameter

vector<Position> Board::filterLegalMoves(Tile t){
    unsigned int opponent = (t.color == WHITE) ? BLACK : WHITE;
    vector<Position> validMoves;
    

    for(const Position& moveSelPiece : t.legalMoves){
        Board b = Board(*this);

        b.tryMakeMove(b.tiles[t.currentPos.x][t.currentPos.y], b.tiles[moveSelPiece.x][moveSelPiece.y]);
        bool isKingSafe = true;

        for(Position& opponentPos : b.posOfPieces){
            
            Tile oppPiece = b.tiles[opponentPos.x][opponentPos.y];

            if(oppPiece.color == opponent){
                b.generateMoves(oppPiece);

                for(Position opp : oppPiece.legalMoves){
                    if(b.tiles[opp.x][opp.y].piece == KING){
                        isKingSafe = false;
                        break;
                    }
                }
            }

            if (!isKingSafe) break;
        }

        if(isKingSafe) validMoves.push_back(moveSelPiece);
    }

    return validMoves;
}




bool Board::isKingInCheck(unsigned int colorPiece){
    unsigned int colorOpp = (colorPiece == WHITE) ? BLACK : WHITE;

    for(Position opp : posOfPieces){
        Tile oppPiece = tiles[opp.x][opp.y];

        if(oppPiece.color == colorOpp && oppPiece.piece != KING){
            generateMoves(oppPiece);

            for(Position move : oppPiece.legalMoves){
                if(tiles[move.x][move.y].piece == KING){
                    return true;
                }
            }
        }
    }

    return false;
}













void Board::displayBoard(){

    cout << "Board:\n";
    for(int i = 0; i < BOARD; i++){
        for(int j = 0; j < BOARD; j++){
            cout << tiles[i][j].piece;
            if(j != BOARD - 1){
                cout << ", ";
            }
        }
        cout << "\n";
    }

    cout << "isTheKingInCheck: " << isKingInCheck(WHITE) << endl;
}

void Board::displayPosTiles(){
    cout << "Board:\n";
    for(int i = 0; i < BOARD; i++){
        for(int j = 0; j < BOARD; j++){
            cout << posTiles[i][j];
            if(j != BOARD - 1){
                cout << ", ";
            }
        }
        cout << "\n";
    }
}