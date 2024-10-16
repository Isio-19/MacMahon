#include "../include/Board.h"

#include <iostream>
#include <string>
#include <chrono>

int main (int argc, char* args[]) {
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;    
    std::chrono::duration<double, std::milli> duration;
    
    std::string path = "../";
    std::string Boards[4] = {
        path+"grids/4x4.txt", 
        path+"grids/5x5.txt", 
        path+"grids/6x4.txt", 
        path+"grids/6x6.txt"
    };

    for (std::string board : Boards) {
        Board currentBoard(board);
        bool solutionNotFound = false;

        start = std::chrono::high_resolution_clock::now();

        if (std::string(args[1]) == "-s")
            if (!currentBoard.solveBoardSequential(0, 0))
                solutionNotFound = true;
        if (std::string(args[1]) == "-t") 
            if (!currentBoard.solveBoardThreadPool(8))
                solutionNotFound = true;
        if (std::string(args[1]) == "-f") 
            if (!currentBoard.solveBoardForLoop())
                solutionNotFound = true;
        
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        if (solutionNotFound) {
            std::cerr << "Not solution found for " << board << std::endl;
            continue;
        }

        std::cout << "Solution found for " << board << " in " << duration.count() << " milliseconds." << std::endl;

        if (std::string(args[2]) == "-i")
            currentBoard.printBoard();
        else 
            currentBoard.printBoardTerminal();
    }

    return 0;
}