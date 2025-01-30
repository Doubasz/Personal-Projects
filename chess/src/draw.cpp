#include "draw.h"
#include "def.h"
#include "board.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <cstring>
#include <iostream>



int power(int a, int n){
    int temp = a;
    for(int i = 1; i < n; i++){
        a *= temp;
    }
    return a;
}

Color::Color(){
    hex = 0xFFFFFF;
    r = g = b = 255;
}

Color::Color(uint32_t hex) : hex(hex){
    b = (hex >> 16) & 0xFF;
    g = (hex >> 8) & 0xFF;
    r = hex & 0xFF;
}

Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b){
    hex = r * power(2, 16) + g * power(2, 8) + b;
}


SDL_Texture* loadTexture(SDL_Renderer* renderer, char* path){
    SDL_Texture* texture;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", path);

    texture = IMG_LoadTexture(renderer, path);

    return texture;
}



std::vector<SDL_Texture*> loadAllTexture(SDL_Renderer* renderer){
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    std::vector<SDL_Texture*> textures;

    for(int i = 0; i < NUM_PATHS; i++){

        const char* temp = TEXTURES_PATHS[i].c_str();

        char* path = new char[std::strlen(temp) + 1];
        std::strcpy(path, temp);

        textures.push_back(loadTexture(renderer, path));

        delete[] path;
    }
    std::cout << "size vector: " << textures.size() << std::endl; 
    return textures;
}



void drawBackground(SDL_Renderer*& renderer){

    Color bg = Color(BG_COLOR);

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
    SDL_RenderClear(renderer);

}



void drawBoard(SDL_Renderer*& renderer, Board& board, Tile* selectedPiece){

    Color black = Color(BLACK_TILE);
    Color white = Color(WHITE_TILE);
    Color red = Color(RED_MOVE);

    Tile temp;
    if (selectedPiece != nullptr) temp = *selectedPiece;


    SDL_Rect tile = SDL_Rect{STARTING_POSX, STARTING_POSY, TILESIZE, TILESIZE};

    for (int i = 0; i < BOARD; i++){
        tile.x = STARTING_POSX;

        for(int j = 0; j < BOARD; j++){
            Color currentColor = Color();
            currentColor = (((i + j) % 2) == 0) ? white : black;

            if(!temp.legalMoves.empty()){
                if(temp.isInLegalMoves({i, j})){
                    currentColor = red;
                }
            }

            drawRect(renderer, tile, currentColor);
            
            tile.x += 75;
        }
        tile.y += 75;
    }

}



void drawPieces(SDL_Renderer*& renderer, Board& board, std::vector<SDL_Texture*> textures){
    for(int i = 0; i < BOARD; i++){
        for(int j = 0; j < BOARD; j++){
            SDL_Rect tile = SDL_Rect{board.posTiles[i][j].x, board.posTiles[i][j].y, TILESIZE, TILESIZE};

            if (board.tiles[i][j].piece != 0) {
                drawPiece(renderer, textures[(board.tiles[i][j].piece + board.tiles[i][j].color) - 1], tile);
            }
        }
    }

}



void drawRect(SDL_Renderer*& renderer, SDL_Rect rect, Color color){

    SDL_SetRenderDrawColor(renderer, color.b, color.g, color.r, 255);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
}



void drawPiece(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect& rect){

    int textureWidth, textureHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}




void drawPromotion(SDL_Renderer*& renderer, Board& board, Tile target, std::vector<SDL_Texture*> textures){
    int x = (STARTING_POSX + (TILESIZE * BOARD)) / 2; // get the middle of the board (used it as a default value)
    int y = STARTING_POSY - (20 + TILESIZE); // up the y of the board by 20, why 20? because it looks good

    int width = (TILESIZE * 4); // 4 is the number of piece (q, b, n, r)
    int height = TILESIZE;

    Color colorRect = 0xD9D9D9;
    SDL_Rect rect = SDL_Rect{x, y, width, height};

    int colorPiece = target.color;

    drawRect(renderer, rect, colorRect);

    rect.w = TILESIZE;

    for(int i = 4; i > 0; i--){
        drawPiece(renderer, textures[i + colorPiece], rect);
        rect.x += TILESIZE;
    }

}