#ifndef TILE_MACMAHON
#define TILE_MACMAHON

#include <SDL2/SDL.h>
#include <vector> 
#include <string> 
#include <map>

class Tile {
    private: 
    char leftColor;
    char topColor;
    char rightColor;
    char bottomColor;
    int tileId;

    public:
    Tile();
    Tile(char, char, char, char, int);

    char getLeftColor();
    char getTopColor();
    char getRightColor();
    char getBottomColor();
    int getId();

    void setLeftColor(char color);
    void setTopColor(char color);
    void setRightColor(char color);
    void setBottomColor(char color);
    void setId(int number);

    std::string getColors();

    std::vector<std::vector<SDL_Vertex>> getVertex(float x, float y, int tileSize);
    std::vector<std::vector<int>> getLines(int x, int y, int tileSize);
    SDL_Rect getOutline(int x, int y, int tileSize);
};

#endif