#ifndef _CHESS
#define _CHESS

#include <SDL2/SDL.h>
#include <vector>
#include "draw.h"
#include "board.h"




class Chess{
    public:
        SDL_Window* window;
        SDL_Renderer* renderer;
        std::vector<SDL_Texture*> textures;
        Board board;

        bool quit;

        Chess();
        ~Chess();

        void gameLoop();
        void draw();
        void doInput();

        Position getPiecePosInArray(Position posInBoard);
        Position getClosestTile(Position p);
};   



std::vector<SDL_Texture*> loadAllTexture();
bool isInBoard(Position selectPos);
bool isInTile(Position selectPos, Position tilePos);
bool isPromotion(Tile target);
bool isBetweenZone(Position posToVerify, SDL_Rect zone);


#endif