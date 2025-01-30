#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>
#include <vector>
#include "board.h"

struct Color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint32_t hex;

    Color();
    Color(uint8_t r, uint8_t g, uint8_t b);
    Color(uint32_t hexa);
};


void drawBackground(SDL_Renderer*& renderer);
void drawBoard(SDL_Renderer*& renderer, Board& board, Tile* selectedPiece);
void drawRect(SDL_Renderer*& renderer, SDL_Rect rect, Color color);
void drawPiece(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect& rect);
void drawPieces(SDL_Renderer*& renderer, Board& board, std::vector<SDL_Texture*> textures);
void drawPromotion(SDL_Renderer*& renderer, Board& board, Tile target, std::vector<SDL_Texture*> textures);
int power(int a, int n);
SDL_Texture* loadTexture(SDL_Renderer* renderer, char* path);
std::vector<SDL_Texture*> loadAllTexture(SDL_Renderer* renderer);

#endif