
#include "tile.h"

void Tile::setPiece(int p, int c){
    piece = p;
    color = c;
    neverMoved = false;
}

void Tile::setPiece(Tile t){
    piece = t.piece;
    color = t.color;
    neverMoved = false;
    t.neverMoved = false;
}

Tile::Tile(){
    piece = NONE;
    color = -1;
    neverMoved = false;
}

Tile::Tile(int p, int color){setPiece(p, color);}

bool Tile::notBlockedBy(Tile t){
    return (t.piece == NONE || isEnnemy(t));
}

bool Tile::isEnnemy(Tile t){
    return ((color == WHITE && t.color == BLACK) || (color == BLACK && t.color == WHITE));
}

void Tile::addMove(Position p){
    legalMoves.push_back(p);
}

bool Tile::isInLegalMoves(Position p){

    for(Position& move : legalMoves){
        if(move == p) return true;
    }
    return false;
}