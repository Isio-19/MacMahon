#include "../include/Board.h"

#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>

int main (int argc, char* args[]) {
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;    
    std::chrono::duration<double, std::milli> duration;
    
    
    std::string path = "../grids/";
    std::vector<std::string> Boards;
    // Boards array
    for (auto entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory())
            continue;

        Boards.push_back(entry.path());
    }

    char solvingMethod = char(args[1][0]);
    switch (solvingMethod) {
        case 's':
        case 't':
        case 'f':
            break;
        
        default:
            std::cout << "Solving method argument not recognized" << std::endl;
            exit(1);
            break;
    }

    char displayMethod = char(args[2][0]);
    switch (displayMethod) {
    case 'i':
    case 't':
        break;
    
    default:
        std::cout << "Display method argument not recognized" << std::endl;
        break;
    }

    for (std::string board : Boards) {
        Board currentBoard(path+board);
        bool solutionNotFound = false;

        start = std::chrono::high_resolution_clock::now();

        switch (solvingMethod) {
            case 's':
                solutionNotFound = currentBoard.solveBoardSequential(0, 0);
                break;
            case 't':
                solutionNotFound = currentBoard.solveBoardThreadPool(8);
                std::cout << "augh";
                break;
            case 'f':
                solutionNotFound = currentBoard.solveBoardForLoop();
                break;
        }

        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        if (solutionNotFound) {
            std::cerr << "No solution found for " << board << std::endl;
            continue;
        }

        std::cout << "Solution found for " << board << " in " << duration.count() << " milliseconds." << std::endl;

        switch (displayMethod) {
        case 'i':
            currentBoard.printBoard();
            break;
        case 't':
            currentBoard.printBoardTerminal();        
        }
    }

    return 0;
}