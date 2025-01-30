
#include "chess.h"
#include <SDL2/SDL.h>
#include <iostream>
#include "def.h"

#include "draw.h"
#include "board.h"

using namespace std;



bool mousePressed = false;
bool positionStored = false;
bool clicked = false;
bool isPawnPromotion = false;
bool choiceMade = false;

int choice;

Position selectPos;
Position piecePosInArray;
Tile* selectedPieceDnD;
Tile* selectedClickedPiece;
Tile* promotionPawn;
Position mousePos;

Position posClickedPieceInArray;
Position posClickedPiece;

Position promotionChoicePos;



Chess::Chess(){
    int windowFlags = 0;
    int rendererFlags = SDL_RENDERER_ACCELERATED;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "Couldn't init SDL" << endl;
        exit(1);
    }

    window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, windowFlags);
    renderer = SDL_CreateRenderer(window, -1, rendererFlags);

    textures = loadAllTexture(renderer);
    quit = false;

    board = Board();

    board.displayBoard();

}

Chess::~Chess(){
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

}

void Chess::draw(){
    drawBackground(renderer);
    drawBoard(renderer, board, selectedClickedPiece);
    drawPieces(renderer, board, textures);

    if(isPawnPromotion) drawPromotion(renderer, board, *promotionPawn, textures);

    SDL_RenderPresent(renderer);
}




void Chess::gameLoop(){
    while(!quit){
        draw();
        doInput();
    }
}

void Chess::doInput(){

    SDL_Event event;

    while(SDL_PollEvent(&event)){

        switch(event.type){
            case SDL_QUIT:
                quit = true;
                exit(0);
                break;

            case SDL_MOUSEBUTTONDOWN:

                if (event.button.button == SDL_BUTTON_LEFT){

                    Position pressedAt = {event.button.x, event.button.y};

                    if(isPawnPromotion){

                        int x = (STARTING_POSX + (TILESIZE * BOARD)) / 2; // get the middle of the board (used it as a default value)
                        int y = STARTING_POSY - (20 + TILESIZE); // up the y of the board by 20, why 20? because it looks good

                        int width = (TILESIZE * 4); // 4 is the number of piece (q, b, n, r)
                        int height = TILESIZE;

                        SDL_Rect choiceRect = {x, y, width, height};

                        promotionChoicePos = pressedAt;
                        if(isBetweenZone(promotionChoicePos, choiceRect)){

                            // the choice is in the array [Q, R, B, N] (0-based)
                            choice = (promotionChoicePos.x / TILESIZE) - 6; // 6 big magic number
                            choiceMade = true;
                        } else{
                            choice = 0;
                            choiceMade = true;
                        }
                    }

                    if(isInBoard(pressedAt)){
                        posClickedPiece = {event.button.x, event.button.y};
                        posClickedPieceInArray = getPiecePosInArray(posClickedPiece);
                        break;
                    }
                }
                
                





                /*if (event.button.button == SDL_BUTTON_LEFT && !mousePressed){
                    mousePressed = true;

                    if(!positionStored){
                        selectPos = Position(event.button.x, event.button.y);
                        positionStored = true;
                        cout << "x, y " << selectPos.x << " " << selectPos.y << endl;

                        if(isInBoard(selectPos)){

                            piecePosInArray = getPiecePosInArray(selectPos);
                            selectedPieceDnD = &board.tiles[piecePosInArray.x][piecePosInArray.y];

                            board.generateMoves(*selectedPieceDnD);


                            displayTab(selectedPieceDnD->legalMoves);

                            //displayMovesInBoard(board.tiles[piecePosInArray.x][piecePosInArray.y]);

                            
                        }
                    }
                }*/
                break;
                

            case SDL_MOUSEMOTION: 
                if(mousePressed){

                    if(piecePosInArray.x != -1 && piecePosInArray.y != - 1){

                        board.posTiles[piecePosInArray.x][piecePosInArray.y] = Position(event.motion.x - TILESIZE / 2, event.motion.y - TILESIZE  / 2);
                    }
                    
                }
                
                break;

            
            case SDL_MOUSEBUTTONUP:

                
                if (event.button.button == SDL_BUTTON_LEFT){

                    Position releasedAt = {event.button.x, event.button.y};

                        if(choiceMade){

                            board.makePromotion(*promotionPawn, choice);
                            isPawnPromotion = false;
                            choiceMade = false;
                            promotionPawn = nullptr;
                        }

                    
                        Position temp = {event.button.x, event.button.y};
                        Position temp1 = getPiecePosInArray(temp);

                        if(temp1 == posClickedPieceInArray){
                            //  Here we clicked and stayed in the same tile
                            cout << "temp: " << temp1.x << " " << temp1.y << endl;

                            Tile* clickPiece = &board.tiles[temp1.x][temp1.y];
                            cout << "Clicked piece is " << clickPiece->currentPos.x << ", " << clickPiece->currentPos.y << endl;

                            
                                if(selectedClickedPiece == nullptr){

                                    if(clickPiece->piece != NONE){

                                        selectedClickedPiece = clickPiece;
                                        
                                        if(selectedClickedPiece->color == board.turnToMove){
                                            board.generateLegalMoves(*selectedClickedPiece);
                                            cout << "Current selected piece is " << selectedClickedPiece->currentPos.x << " " << selectedClickedPiece->currentPos.y << endl;
                                            displayTab(selectedClickedPiece->legalMoves);

                                            cout << "Current selected piece is " << selectedClickedPiece->currentPos.x << " " << selectedClickedPiece->currentPos.y << endl;
                                        }else{
                                            selectedClickedPiece->legalMoves.clear();
                                        }
                                        
                                    }
                                }
                                else{

                                    displayTab(selectedClickedPiece->legalMoves);

                                    if(selectedClickedPiece->isInLegalMoves(clickPiece->currentPos)){

                                        board.makeMove(*selectedClickedPiece, *clickPiece);
                                        

                                        if(isPromotion(*clickPiece)){
                                            isPawnPromotion = true;
                                            promotionPawn = clickPiece;
                                        }
                                        
                                        selectedClickedPiece = nullptr;

                                        board.displayBoard();
                                        cout << "Move completed" << endl;
                                    }
                                    else{
                                        selectedClickedPiece = nullptr;
                                        cout << "Piece deselected" << endl;
                                    }
                                    
                                }
                            

                            
                            

                            
                            
                        }
                        
                    



                    /*if(isInBoard(Position(event.button.x, event.button.y))){

                        board.posTiles[piecePosInArray.x][piecePosInArray.y] = Position(STARTING_POSX + (TILESIZE * piecePosInArray.y), STARTING_POSY + (TILESIZE  * piecePosInArray.x));

                        displayPosTiles();
                        

                        if(board.tiles[piecePosInArray.x][piecePosInArray.y].piece != NONE){

                            cout << "x,y " << event.button.x << " " << event.button.y << endl;
                            Position targetSquare = getClosestTile(Position(event.button.x, event.button.y));

                            cout << targetSquare.x << " " << targetSquare.y << endl;
                            
                            if(!(piecePosInArray == targetSquare)){
                                board.tiles[targetSquare.x][targetSquare.y].setPiece(board.tiles[piecePosInArray.x][piecePosInArray.y].piece, board.tiles[piecePosInArray.x][piecePosInArray.y].color);
                                board.tiles[piecePosInArray.x][piecePosInArray.y].piece = NONE;
                            }

                            displayBoard();
                        }
                    }
                    else{
                        board.posTiles[piecePosInArray.x][piecePosInArray.y] = Position(STARTING_POSX + (TILESIZE * piecePosInArray.y), STARTING_POSY + (TILESIZE  * piecePosInArray.x));
                    }*/


                    mousePressed = false;
                    positionStored = false;
                    cout << "mouse released\n";
                }
                break;

            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_LEFT){
                    cout << "lastPos: " << board.lastPosition << endl;
                    cout << "left pressed" << endl;
                    
                    board.undoMove();
                    board.displayBoard();
                }
                else if (event.key.keysym.sym == SDLK_RIGHT){

                    cout << "right pressed" << endl;

                    board.redoMove();
                    board.displayBoard();
                }
                
                break;

            default:
                break;
        }
    }
}

void getClick(Position pressed, Position released){

}


bool isInBoard(Position selectPos){
    return selectPos.x > STARTING_POSX && selectPos.y > STARTING_POSY && selectPos.x < STARTING_POSX + (TILESIZE * BOARD) && selectPos.y < STARTING_POSY + (TILESIZE * BOARD);
}

bool isInTile(Position selectPos, Position tilePos){
    return selectPos.x - tilePos.x <= TILESIZE && selectPos.y - tilePos.y <= TILESIZE && selectPos.x - tilePos.x >= 0 && selectPos.y - tilePos.y >= 0;
}

bool isPromotion(Tile target){
    return (target.currentPos.x == 7 || target.currentPos.x == 0) && target.piece == PAWN; 
}

bool isBetweenZone(Position posToVerify, SDL_Rect zone){
    return posToVerify.x >= zone.x && posToVerify.x <= zone.x + zone.w && posToVerify.y >= zone.y && posToVerify.y <= zone.y + zone.h;
}

Position Chess::getPiecePosInArray(Position posInWindow){
    Position temp = Position(-1, -1);

    for(int i = 0; i < BOARD; i++){
        for(int j = 0; j < BOARD; j++){
            if(isInTile(posInWindow, board.posTiles[i][j])){
                temp.x = i;
                temp.y = j;
                break;
            }
        }
    }

    return temp;
}

Position Chess::getClosestTile(Position p){

    Position temp = Position(-1, -1);
    Position tile = Position(STARTING_POSX, STARTING_POSY);

    for(int i = 0; i < BOARD; i++){
        tile.x = STARTING_POSX;
        for(int j = 0; j < BOARD; j++){
            if(isInTile(p, tile)){
                temp.x = i;
                temp.y = j;
                break;
            }

            tile.x += TILESIZE;
        }
        tile.y += TILESIZE;
    }
    return temp;

}

