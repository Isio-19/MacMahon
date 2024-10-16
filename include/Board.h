#ifndef BOARD_MACMAHON
#define BOARD_MACMAHON

#include <SDL2/SDL.h>

#include <iostream>
#include <fstream>
#include <locale>
#include <fcntl.h>

#include <thread>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <mutex>

#include "Tile.h"

class Board {
    private:
    int width;
    int height;
    Tile** board;   
    Tile* tilesToBePlaced;
    int sizeArray;
    char boardEdgeColor;
    bool solutionFound;
    
    public:
    Board();
    Board(std::string file);
    Board(Board &boardToCopy);
    ~Board();

    int getWidth();
    int getHeight();
    Tile** getBoard();
    Tile* getTilesToPlace();
    int getSizeArray();
    bool getSolution();

    bool checkColor(Tile tile, int posX, int posY);
    bool solveBoardSequential(int posX, int posY);
    bool solveBoardSequential(int index);

    void operator()(int type, int index);

    bool ThreadPoolFunction();
    bool solveBoardThreadPool(int nbThread);
    bool solveBoardForLoop();

    int printBoardTerminal();
    int printBoard();
};

#endif