#ifndef _BOARD
#define _BOARD

#include "tile.h"
#include "position.h"
#include <vector>
#include <stack>



class Board{

    public: 
        Tile tiles[8][8];
        Position posTiles[8][8];
        std::vector<Position> posOfPieces;
        std::vector<std::string> positionGame;
        std::string lastPosition;

        std::stack<std::string> undoStack;
        std::stack<std::string> redoStack; 
        
        int turnToMove;
        bool enPassantActive;
        Position enPassantTarget;

        unsigned int nbMoves;
        unsigned int nbHalfMoves;



        Board();
        Board(const Board& other);
        void parseFENPosition(std::string pos);
        std::string posToFen();
        ~Board();

        void changeTurn();
        void generateMoves(Tile& tile);
        void generateAllMoves();
        void generateLegalMoves(Tile& tile);
        void tryMakeMove(Tile& start, Tile& target);
        void makePromotion(Tile& target, int choice);
        void makeMove(Tile& start, Tile& target);
        void undoMove();
        void redoMove();
        void displayBoard();
        void displayPosTiles();

        std::vector<Position> filterLegalMoves(Tile tile);
        bool isKingInCheck(unsigned int color);
    

};

std::vector<std::string> split(const std::string& str, char delimiter);
bool notOutOfBonds(Position p);
Position getTileTillOutBonds(Position p);
std::vector<Position> getDirectionSlidingPiece(Tile t);
char pieceToFen(Tile t);
Position translateAlgebraic(std::string s);


#endif